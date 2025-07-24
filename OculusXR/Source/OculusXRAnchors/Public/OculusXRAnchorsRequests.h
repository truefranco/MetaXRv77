// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAsyncRequestSystem.h"
#include "OculusXRAsyncRequest.h"
#include "OculusXRAnchorTypes.h"

namespace OculusXRAnchors
{
	struct OCULUSXRANCHORS_API FAsyncResultAnchorSuccess
	{
		bool operator()(EOculusXRAnchorResult::Type Val);
	};

	template <typename TDerived, typename TValueType>
	using FAsyncAnchorRequest = OculusXR::FAsyncRequest<TDerived, EOculusXRAnchorResult::Type, TValueType, FAsyncResultAnchorSuccess>;

	// Share anchors with group
	struct OCULUSXRANCHORS_API FShareAnchorsWithGroups :
		FAsyncAnchorRequest<FShareAnchorsWithGroups, TTuple<TArray<FOculusXRUUID>, TArray<FOculusXRUInt64>>>
	{
	public:
		FShareAnchorsWithGroups(const TArray<FOculusXRUUID>& TargetGroups, const TArray<FOculusXRUInt64>& AnchorsToShare);
		~FShareAnchorsWithGroups();

		const TArray<FOculusXRUUID>& GetGroups() const { return Groups; }
		const TArray<FOculusXRUInt64>& GetAnchors() const { return Anchors; }

	protected:
		virtual void OnInitRequest() override;

	private:
		static void OnShareComplete(FOculusXRUInt64 RequestId, EOculusXRAnchorResult::Type Result);

		TArray<FOculusXRUUID> Groups;
		TArray<FOculusXRUInt64> Anchors;
		FDelegateHandle CallbackHandle;
	};

	// Get shared anchors from group
	struct OCULUSXRANCHORS_API FGetAnchorsSharedWithGroup :
		FAsyncAnchorRequest<FGetAnchorsSharedWithGroup, TArray<FOculusXRAnchor>>
	{
	public:
		FGetAnchorsSharedWithGroup(const FOculusXRUUID& TargetGroup, const TArray<FOculusXRUUID>& WantedAnchors = {});
		~FGetAnchorsSharedWithGroup();

		void OnResultsAvailable(const TArray<FOculusXRAnchor>& Results);
		const TArray<FOculusXRAnchor>& GetRetrievedAnchors() const { return RetrievedAnchors; }

	protected:
		virtual void OnInitRequest() override;

	private:
		static void OnQueryComplete(FOculusXRUInt64 RequestId, EOculusXRAnchorResult::Type Result);
		static void OnQueryResultAvailable(FOculusXRUInt64 RequestId, FOculusXRUInt64 AnchorHandle, FOculusXRUUID AnchorUuid);

		FOculusXRUUID Group;
		TArray<FOculusXRUUID> RequestedAnchors;
		TArray<FOculusXRAnchor> RetrievedAnchors;
		FDelegateHandle CallbackHandleComplete;
		FDelegateHandle CallbackHandleResults;
	};
} // namespace OculusXRAnchors
