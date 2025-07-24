// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAnchorFunctions.h"

class OCULUSXRANCHORS_API FOculusXRAnchorFunctionsOVR : public IOculusXRAnchorFunctions
{
public:
	virtual EOculusXRAnchorResult::Type CreateAnchor(const FTransform& InTransform, uint64& OutRequestId, const FTransform& CameraTransform) override;
	virtual EOculusXRAnchorResult::Type DestroyAnchor(uint64 AnchorHandle) override;

	virtual EOculusXRAnchorResult::Type TryGetAnchorTransform(uint64 AnchorHandle, FTransform& OutTransform, FOculusXRAnchorLocationFlags& OutLocationFlags, EOculusXRAnchorSpace Space) override;
	virtual EOculusXRAnchorResult::Type SetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool Enable, float Timeout, uint64& OutRequestId) override;
	virtual EOculusXRAnchorResult::Type GetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool& OutEnabled, bool& OutChangePending) override;
	virtual EOculusXRAnchorResult::Type GetSupportedAnchorComponents(uint64 AnchorHandle, TArray<EOculusXRSpaceComponentType>& OutSupportedTypes) override;
	virtual EOculusXRAnchorResult::Type GetAnchorContainerUUIDs(uint64 AnchorHandle, TArray<FOculusXRUUID>& OutUUIDs) override;

	virtual EOculusXRAnchorResult::Type SaveAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, EOculusXRSpaceStoragePersistenceMode StoragePersistenceMode, uint64& OutRequestId) override;
	virtual EOculusXRAnchorResult::Type SaveAnchorList(const TArray<uint64>& AnchorHandles, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId) override;
	virtual EOculusXRAnchorResult::Type SaveAnchors(const TArray<uint64>& AnchorHandles, uint64& OutRequestId) override;

	virtual EOculusXRAnchorResult::Type DiscoverAnchors(const FOculusXRSpaceDiscoveryInfo& DiscoveryInfo, uint64& OutRequestId) override;
	virtual EOculusXRAnchorResult::Type QueryAnchors(const FOculusXRSpaceQueryInfo& QueryInfo, uint64& OutRequestId) override;
	virtual EOculusXRAnchorResult::Type ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<uint64>& UserIds, uint64& OutRequestId) override;
	virtual EOculusXRAnchorResult::Type ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<FOculusXRUUID>& Groups, uint64& OutRequestId) override;

	virtual EOculusXRAnchorResult::Type EraseAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId) override;
	virtual EOculusXRAnchorResult::Type EraseAnchors(const TArray<FOculusXRUInt64>& AnchorHandles, const TArray<FOculusXRUUID>& UUIDs, uint64& OutRequestId) override;
};
