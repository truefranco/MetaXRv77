// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAsyncRequestSystem.h"
#include "OculusXRAsyncRequest.h"
#include "OculusXRColocationSession.h"
#include "OculusXRColocationTypes.h"
#include "OculusXRColocationUtil.h"

DECLARE_DELEGATE_OneParam(FOculusXRColocationSessionFoundDelegate, const FOculusXRColocationSession&);

namespace OculusXRColocation
{
	struct FAsyncResultColocationDiscoverySuccess
	{
		bool operator()(EColocationResult Val) { return OculusXRColocation::IsResultSuccess(Val); }
	};

	template <typename TDerived, typename TValueType>
	using FAsyncColocationRequest = OculusXR::FAsyncRequest<TDerived, EColocationResult, TValueType, FAsyncResultColocationDiscoverySuccess>;

	// Discover nearby sessions
	struct OCULUSXRCOLOCATION_API FDiscoverSessionsRequest :
		FAsyncColocationRequest<FDiscoverSessionsRequest, TArray<FOculusXRColocationSession>>
	{
	public:
		FDiscoverSessionsRequest();
		~FDiscoverSessionsRequest();

		void BindOnSessionFound(const FOculusXRColocationSessionFoundDelegate& OnSessionFound);
		void OnSessionFound(FOculusXRColocationSession&& Session);

		const TArray<FOculusXRColocationSession>& GetFoundSessions() const { return FoundSessions; }

	protected:
		virtual void OnInitRequest() override;

	private:
		static void OnStartComplete(FOculusXRUInt64 RequestId, EColocationResult Result);
		static void OnResultAvailable(FOculusXRUInt64 RequestId, FOculusXRUUID Uuid, const TArray<uint8>& Metadata);
		static void OnDiscoveryComplete(FOculusXRUInt64 RequestId, EColocationResult Result);

		FDelegateHandle OnStartCompleteHandle;
		FDelegateHandle OnSessionFoundHandle;
		FDelegateHandle OnStopCompleteHandle;
		FDelegateHandle OnDiscoveryCompleteHandle;

		FOculusXRColocationSessionFoundDelegate OnFoundSessionCallback;
		TArray<FOculusXRColocationSession> FoundSessions;
	};

	// Start advertisement, creates a session internally
	struct OCULUSXRCOLOCATION_API FStartSessionAdvertisementRequest :
		FAsyncColocationRequest<FStartSessionAdvertisementRequest, FOculusXRColocationSession>
	{
	public:
		FStartSessionAdvertisementRequest(const TArray<uint8>& SessionData);
		~FStartSessionAdvertisementRequest();

		const TArray<uint8>& GetData() const { return Data; }

	protected:
		virtual void OnInitRequest() override;

	private:
		static void OnStartComplete(FOculusXRUInt64 RequestId, FOculusXRUUID Uuid, EColocationResult Result);

		FDelegateHandle OnStartCompleteHandle;
		TArray<uint8> Data;
	};

	// Stop advertisement, stops advertising and destroys the internal session
	struct OCULUSXRCOLOCATION_API FStopSessionAdvertisementRequest :
		FAsyncColocationRequest<FStopSessionAdvertisementRequest, FOculusXRColocationSession>
	{
	public:
		FStopSessionAdvertisementRequest();
		~FStopSessionAdvertisementRequest();

	protected:
		virtual void OnInitRequest() override;

	private:
		static void OnStopComplete(FOculusXRUInt64 RequestId, EColocationResult Result);

		FDelegateHandle OnStopCompleteHandle;
		FDelegateHandle OnCompleteHandle;
	};
} // namespace OculusXRColocation
