// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRAnchorsRequests.h"
#include "OculusXRHMDModule.h"
#include "OculusXRAnchorsUtil.h"
#include "OculusXRAnchorsModule.h"
#include "OculusXRAnchorManager.h"
#include "OculusXRAnchorDelegates.h"

namespace OculusXRAnchors
{
	bool FAsyncResultAnchorSuccess::operator()(EOculusXRAnchorResult::Type Val)
	{
		return IsAnchorResultSuccess(Val);
	}

	FShareAnchorsWithGroups::FShareAnchorsWithGroups(const TArray<FOculusXRUUID>& TargetGroups, const TArray<FOculusXRUInt64>& AnchorsToShare)
		: Groups(TargetGroups)
		, Anchors(AnchorsToShare)
	{
		CallbackHandle = FOculusXRAnchorEventDelegates::OculusShareAnchorsComplete.AddStatic(&FShareAnchorsWithGroups::OnShareComplete);
	}

	FShareAnchorsWithGroups::~FShareAnchorsWithGroups()
	{
		FOculusXRAnchorEventDelegates::OculusShareAnchorsComplete.Remove(CallbackHandle);
	}

	void FShareAnchorsWithGroups::OnInitRequest()
	{
		TArray<uint64> anchorHandles;
		Algo::Transform(Anchors, anchorHandles, [](const FOculusXRUInt64& In) { return In.GetValue(); });

		uint64 requestId;
		auto result = FOculusXRAnchorManager::ShareAnchors(anchorHandles, Groups, requestId);

		SetRequestId(DetermineRequestId(result, requestId));
		SetInitialResult(result);

		UE_LOG(LogOculusXRAnchors, Log, TEXT("Started FShareAnchorsWithGroups: RequestId: %llu -- EventId: %llu -- Result: %s"),
			GetRequestId().Id, GetEventId().Id, *GetStringFromResult(result));
	}

	void FShareAnchorsWithGroups::OnShareComplete(FOculusXRUInt64 RequestId, EOculusXRAnchorResult::Type Result)
	{
		auto taskPtr = OculusXR::FAsyncRequestSystem::GetRequest<FShareAnchorsWithGroups>(
			OculusXR::FAsyncRequestBase::RequestId{ RequestId });

		if (taskPtr.IsValid())
		{
			OculusXR::FAsyncRequestSystem::CompleteRequest<FShareAnchorsWithGroups>(
				taskPtr->GetEventId(),
				FShareAnchorsWithGroups::FResultType::FromResult(
					Result,
					FShareAnchorsWithGroups::FResultValueType(taskPtr->GetGroups(), taskPtr->GetAnchors())));
		}
	}

	FGetAnchorsSharedWithGroup::FGetAnchorsSharedWithGroup(const FOculusXRUUID& TargetGroup, const TArray<FOculusXRUUID>& WantedAnchors)
		: Group(TargetGroup)
		, RequestedAnchors(WantedAnchors)
	{
		CallbackHandleComplete = FOculusXRAnchorEventDelegates::OculusSpaceQueryComplete.AddStatic(&FGetAnchorsSharedWithGroup::OnQueryComplete);
		CallbackHandleResults = FOculusXRAnchorEventDelegates::OculusSpaceQueryResult.AddStatic(&FGetAnchorsSharedWithGroup::OnQueryResultAvailable);
	}

	FGetAnchorsSharedWithGroup::~FGetAnchorsSharedWithGroup()
	{
		FOculusXRAnchorEventDelegates::OculusSpaceQueryComplete.Remove(CallbackHandleComplete);
		FOculusXRAnchorEventDelegates::OculusSpaceQueryResult.Remove(CallbackHandleResults);
	}

	void FGetAnchorsSharedWithGroup::OnResultsAvailable(const TArray<FOculusXRAnchor>& Results)
	{
		RetrievedAnchors += Results;
	}

	void FGetAnchorsSharedWithGroup::OnInitRequest()
	{
		constexpr int32 maxSpaces = 1024;
		constexpr double timeout = 0;

		FOculusXRSpaceQueryInfo queryInfo;
		queryInfo.FilterType = EOculusXRSpaceQueryFilterType::FilterByGroup;
		queryInfo.GroupUUIDFilter = Group;
		queryInfo.Location = EOculusXRSpaceStorageLocation::Cloud;
		queryInfo.MaxQuerySpaces = maxSpaces;
		queryInfo.Timeout = timeout;
		queryInfo.IDFilter = RequestedAnchors;

		uint64 requestId;
		auto result = FOculusXRAnchorManager::QueryAnchors(queryInfo, requestId);

		SetRequestId(DetermineRequestId(result, requestId));
		SetInitialResult(result);

		UE_LOG(LogOculusXRAnchors, Log, TEXT("Started FGetAnchorsSharedWithGroup: RequestId: %llu -- EventId: %llu -- Result: %s"),
			GetRequestId().Id, GetEventId().Id, *GetStringFromResult(result));
	}

	void FGetAnchorsSharedWithGroup::OnQueryComplete(FOculusXRUInt64 RequestId, EOculusXRAnchorResult::Type Result)
	{
		auto taskPtr = OculusXR::FAsyncRequestSystem::GetRequest<FGetAnchorsSharedWithGroup>(
			OculusXR::FAsyncRequestBase::RequestId{ RequestId.GetValue() });

		// If there is a valid get shared anchors request we can complete and exit without firing legacy event delegates
		if (taskPtr.IsValid())
		{
			OculusXR::FAsyncRequestSystem::CompleteRequest<FGetAnchorsSharedWithGroup>(
				taskPtr->GetEventId(),
				FGetAnchorsSharedWithGroup::FResultType::FromResult(Result, taskPtr->GetRetrievedAnchors()));

			return;
		}
	}

	void FGetAnchorsSharedWithGroup::OnQueryResultAvailable(FOculusXRUInt64 RequestId, FOculusXRUInt64 AnchorHandle, FOculusXRUUID AnchorUuid)
	{
		auto taskPtr = OculusXR::FAsyncRequestSystem::GetRequest<FGetAnchorsSharedWithGroup>(
			OculusXR::FAsyncRequestBase::RequestId{ RequestId.GetValue() });

		if (taskPtr.IsValid())
		{
			TArray<EOculusXRSpaceComponentType> supportedTypes;
			UE_LOG(LogOculusXRAnchors, Verbose, TEXT("	Found Element: Space: %llu  --  UUID: %s"), AnchorHandle.Value, *AnchorUuid.ToString());

			uint64 tempOut;
			FOculusXRAnchorManager::GetSupportedAnchorComponents(AnchorHandle, supportedTypes);

			if (supportedTypes.Contains(EOculusXRSpaceComponentType::Locatable))
			{
				FOculusXRAnchorManager::SetAnchorComponentStatus(AnchorHandle, EOculusXRSpaceComponentType::Locatable, true, 0.0f, tempOut);
			}

			if (supportedTypes.Contains(EOculusXRSpaceComponentType::Sharable))
			{
				FOculusXRAnchorManager::SetAnchorComponentStatus(AnchorHandle, EOculusXRSpaceComponentType::Sharable, true, 0.0f, tempOut);
			}

			if (supportedTypes.Contains(EOculusXRSpaceComponentType::Storable))
			{
				FOculusXRAnchorManager::SetAnchorComponentStatus(AnchorHandle, EOculusXRSpaceComponentType::Storable, true, 0.0f, tempOut);
			}

			taskPtr->OnResultsAvailable({ FOculusXRAnchor(AnchorHandle, AnchorUuid) });

			return;
		}
	}

} // namespace OculusXRAnchors
