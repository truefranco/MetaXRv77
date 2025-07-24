// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationRequests.h"
#include "OculusXRColocationModule.h"
#include "OculusXRColocationUtil.h"
#include "OculusXRColocationSession.h"
#include "OculusXRColocationEventDelegates.h"
#include "OculusXRColocationSubsystem.h"
#include "OculusXRColocationFunctions.h"
#include "OculusXRHMDModule.h"

namespace OculusXRColocation
{
	FDiscoverSessionsRequest::FDiscoverSessionsRequest()
	{
		OnStartCompleteHandle = FOculusXRColocationEventDelegates::StartColocationDiscoveryComplete.AddStatic(
			&FDiscoverSessionsRequest::OnStartComplete);

		OnSessionFoundHandle = FOculusXRColocationEventDelegates::ColocationDiscoveryResultAvailable.AddStatic(
			&FDiscoverSessionsRequest::OnResultAvailable);

		OnStopCompleteHandle = FOculusXRColocationEventDelegates::StopColocationDiscoveryComplete.AddStatic(
			&FDiscoverSessionsRequest::OnDiscoveryComplete);

		OnDiscoveryCompleteHandle = FOculusXRColocationEventDelegates::StopColocationDiscoveryComplete.AddStatic(
			&FDiscoverSessionsRequest::OnDiscoveryComplete);
	}

	FDiscoverSessionsRequest::~FDiscoverSessionsRequest()
	{
	}

	void FDiscoverSessionsRequest::BindOnSessionFound(const FOculusXRColocationSessionFoundDelegate& OnSessionFound)
	{
		OnFoundSessionCallback = OnSessionFound;
	}

	void FDiscoverSessionsRequest::OnSessionFound(FOculusXRColocationSession&& Session)
	{
		auto index = FoundSessions.Emplace(Session);
		OnFoundSessionCallback.ExecuteIfBound(FoundSessions[index]);
	}

	void FDiscoverSessionsRequest::OnInitRequest()
	{
		uint64 requestId;
		auto result = IOculusXRColocationFunctions::GetOculusXRColocationFunctionsImpl()->StartColocationDiscovery(requestId);
		UE_LOG(LogOculusXRColocation, Log, TEXT("Starting colocation session discovery. RequestID: %llu, Launch async result: %d"), requestId, result);

		SetRequestId(DetermineRequestId(result, requestId));
		SetInitialResult(result);
	}

	void FDiscoverSessionsRequest::OnStartComplete(FOculusXRUInt64 RequestId, EColocationResult Result)
	{
		auto taskPtr = OculusXR::FAsyncRequestSystem::GetRequest<FDiscoverSessionsRequest>(
			OculusXR::FAsyncRequestBase::RequestId{ RequestId.GetValue() });

		if (!taskPtr.IsValid())
		{
			return;
		}

		// If result succeeded we don't have complete the task but we do update the subsystem
		if (IsResultSuccess(Result))
		{
			UOculusXRColocationSubsystem::Get()->SetDiscoveryRequest(taskPtr);
			return;
		}

		OculusXR::FAsyncRequestSystem::CompleteRequest<FDiscoverSessionsRequest>(
			taskPtr->GetEventId(),
			FDiscoverSessionsRequest::FResultType::FromError(Result));
	}

	void FDiscoverSessionsRequest::OnResultAvailable(FOculusXRUInt64 RequestId, FOculusXRUUID Uuid, const TArray<uint8>& Metadata)
	{
		auto taskPtr = OculusXR::FAsyncRequestSystem::GetRequest<FDiscoverSessionsRequest>(
			OculusXR::FAsyncRequestBase::RequestId{ RequestId.GetValue() });

		if (!taskPtr.IsValid())
		{
			return;
		}

		FOculusXRColocationSession session;
		session.Uuid = Uuid;
		session.Metadata = Metadata;

		taskPtr->OnSessionFound(std::move(session));
	}

	void FDiscoverSessionsRequest::OnDiscoveryComplete(FOculusXRUInt64 RequestId, EColocationResult Result)
	{
		auto taskPtr = OculusXR::FAsyncRequestSystem::GetRequest<FDiscoverSessionsRequest>(
			OculusXR::FAsyncRequestBase::RequestId{ RequestId.GetValue() });

		if (!taskPtr.IsValid())
		{
			return;
		}

		UOculusXRColocationSubsystem::Get()->ClearDiscoveryRequest();

		OculusXR::FAsyncRequestSystem::CompleteRequest<FDiscoverSessionsRequest>(
			taskPtr->GetEventId(),
			FDiscoverSessionsRequest::FResultType::FromResult(Result, taskPtr->GetFoundSessions()));
	}

	FStartSessionAdvertisementRequest::FStartSessionAdvertisementRequest(const TArray<uint8>& SessionData)
		: Data(SessionData)
	{
		OnStartCompleteHandle = FOculusXRColocationEventDelegates::StartColocationAdvertisementComplete.AddStatic(
			&FStartSessionAdvertisementRequest::OnStartComplete);
	}

	FStartSessionAdvertisementRequest::~FStartSessionAdvertisementRequest()
	{
		FOculusXRColocationEventDelegates::StartColocationAdvertisementComplete.Remove(OnStartCompleteHandle);
	}

	void FStartSessionAdvertisementRequest::OnInitRequest()
	{
		uint64 requestId;
		auto result = IOculusXRColocationFunctions::GetOculusXRColocationFunctionsImpl()->StartColocationAdvertisement(Data, requestId);

		SetRequestId(DetermineRequestId(result, requestId));
		SetInitialResult(result);
	}

	void FStartSessionAdvertisementRequest::OnStartComplete(FOculusXRUInt64 RequestId, FOculusXRUUID Uuid, EColocationResult Result)
	{
		auto taskPtr = OculusXR::FAsyncRequestSystem::GetRequest<FStartSessionAdvertisementRequest>(
			OculusXR::FAsyncRequestBase::RequestId{ RequestId.GetValue() });

		if (!taskPtr.IsValid())
		{
			return;
		}

		FOculusXRColocationSession session;
		session.Uuid = Uuid;
		session.Metadata = taskPtr->Data;

		OculusXR::FAsyncRequestSystem::CompleteRequest<FStartSessionAdvertisementRequest>(
			taskPtr->GetEventId(),
			FStartSessionAdvertisementRequest::FResultType::FromResult(
				Result,
				session));
	}

	FStopSessionAdvertisementRequest::FStopSessionAdvertisementRequest()
	{
		OnStopCompleteHandle = FOculusXRColocationEventDelegates::StopColocationAdvertisementComplete.AddStatic(
			&FStopSessionAdvertisementRequest::OnStopComplete);

		OnCompleteHandle = FOculusXRColocationEventDelegates::ColocationAdvertisementComplete.AddStatic(
			&FStopSessionAdvertisementRequest::OnStopComplete);
	}

	FStopSessionAdvertisementRequest::~FStopSessionAdvertisementRequest()
	{
		FOculusXRColocationEventDelegates::StopColocationAdvertisementComplete.Remove(OnStopCompleteHandle);
		FOculusXRColocationEventDelegates::StopColocationAdvertisementComplete.Remove(OnCompleteHandle);
	}

	void FStopSessionAdvertisementRequest::OnInitRequest()
	{
		uint64 requestId;
		auto result = IOculusXRColocationFunctions::GetOculusXRColocationFunctionsImpl()->StopColocationAdvertisement(requestId);

		SetRequestId(DetermineRequestId(result, requestId));
		SetInitialResult(result);
	}

	void FStopSessionAdvertisementRequest::OnStopComplete(FOculusXRUInt64 RequestId, EColocationResult Result)
	{
		auto taskPtr = OculusXR::FAsyncRequestSystem::GetRequest<FStopSessionAdvertisementRequest>(
			OculusXR::FAsyncRequestBase::RequestId{ RequestId.GetValue() });

		if (!taskPtr.IsValid())
		{
			return;
		}

		OculusXR::FAsyncRequestSystem::CompleteRequest<FStopSessionAdvertisementRequest>(
			taskPtr->GetEventId(),
			FStartSessionAdvertisementRequest::FResultType::FromResult(Result, UOculusXRColocationSubsystem::Get()->GetLocalSession()));
	}
} // namespace OculusXRColocation
