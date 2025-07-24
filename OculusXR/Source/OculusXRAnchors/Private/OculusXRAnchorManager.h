// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "CoreMinimal.h"
#include "OculusXRAnchorComponent.h"
#include "OculusXRHMDPrivate.h"
#include "OVR_Plugin_Types.h"
#include "OculusXRAnchorFunctions.h"

namespace OculusXRAnchors
{
	struct OCULUSXRANCHORS_API FOculusXRAnchorManager
	{
	public:
		static EOculusXRAnchorResult::Type CreateAnchor(const FTransform& InTransform, uint64& OutRequestId, const FTransform& CameraTransform);
		static EOculusXRAnchorResult::Type DestroyAnchor(uint64 AnchorHandle);

		static EOculusXRAnchorResult::Type TryGetAnchorTransform(uint64 AnchorHandle, FTransform& OutTransform, FOculusXRAnchorLocationFlags& OutLocationFlags, EOculusXRAnchorSpace Space);
		static EOculusXRAnchorResult::Type SetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool Enable, float Timeout, uint64& OutRequestId);
		static EOculusXRAnchorResult::Type GetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool& OutEnabled, bool& OutChangePending);
		static EOculusXRAnchorResult::Type GetSupportedAnchorComponents(uint64 AnchorHandle, TArray<EOculusXRSpaceComponentType>& OutSupportedTypes);
		static EOculusXRAnchorResult::Type GetAnchorContainerUUIDs(uint64 AnchorHandle, TArray<FOculusXRUUID>& OutUUIDs);

		static EOculusXRAnchorResult::Type SaveAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, EOculusXRSpaceStoragePersistenceMode StoragePersistenceMode, uint64& OutRequestId);
		static EOculusXRAnchorResult::Type SaveAnchorList(const TArray<uint64>& AnchorHandles, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId);
		static EOculusXRAnchorResult::Type SaveAnchors(const TArray<uint64>& AnchorHandles, uint64& OutRequestId);

		static EOculusXRAnchorResult::Type DiscoverAnchors(const FOculusXRSpaceDiscoveryInfo& DiscoveryInfo, uint64& OutRequestId);
		static EOculusXRAnchorResult::Type QueryAnchors(const FOculusXRSpaceQueryInfo& QueryInfo, uint64& OutRequestId);
		static EOculusXRAnchorResult::Type ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<uint64>& UserIds, uint64& OutRequestId);
		static EOculusXRAnchorResult::Type ShareAnchors(const TArray<uint64>& AnchorHandles, FOculusXRUUID GroupId, uint64& OutRequestId);
		static EOculusXRAnchorResult::Type ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<FOculusXRUUID>& Groups, uint64& OutRequestId);

		static EOculusXRAnchorResult::Type EraseAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId);
		static EOculusXRAnchorResult::Type EraseAnchors(const TArray<FOculusXRUInt64>& AnchorHandles, const TArray<FOculusXRUUID>& UUIDs, uint64& OutRequestId);

	private:
		static TSharedPtr<IOculusXRAnchorFunctions> GetOculusXRAnchorFunctionsImpl();
		static TSharedPtr<IOculusXRAnchorFunctions> AnchorFunctionsImpl;
	};
} // namespace OculusXRAnchors
