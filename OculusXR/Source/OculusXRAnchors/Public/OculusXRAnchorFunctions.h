// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAnchorTypes.h"

class OCULUSXRANCHORS_API IOculusXRAnchorFunctions
{
public:
	virtual EOculusXRAnchorResult::Type CreateAnchor(const FTransform& InTransform, uint64& OutRequestId, const FTransform& CameraTransform) = 0;
	virtual EOculusXRAnchorResult::Type DestroyAnchor(uint64 AnchorHandle) = 0;

	/**
	 * Try to get the anchors transform. The transform may not always be a available.
	 *
	 * @param AnchorHandle The Anchor handle.
	 * @param OutTransform (out) The anchors transform.
	 * @param OutLocationFlags (out) The location flags.
	 * @param Space The space in which this transform should be returned.
	 *
	 * @return Whether or not the transform could be retrieved.
	 */
	virtual EOculusXRAnchorResult::Type TryGetAnchorTransform(uint64 AnchorHandle, FTransform& OutTransform, FOculusXRAnchorLocationFlags& OutLocationFlags, EOculusXRAnchorSpace Space) = 0;
	virtual EOculusXRAnchorResult::Type SetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool Enable, float Timeout, uint64& OutRequestId) = 0;
	virtual EOculusXRAnchorResult::Type GetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool& OutEnabled, bool& OutChangePending) = 0;
	virtual EOculusXRAnchorResult::Type GetSupportedAnchorComponents(uint64 AnchorHandle, TArray<EOculusXRSpaceComponentType>& OutSupportedTypes) = 0;
	virtual EOculusXRAnchorResult::Type GetAnchorContainerUUIDs(uint64 AnchorHandle, TArray<FOculusXRUUID>& OutUUIDs) = 0;

	virtual EOculusXRAnchorResult::Type SaveAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, EOculusXRSpaceStoragePersistenceMode StoragePersistenceMode, uint64& OutRequestId) = 0;
	virtual EOculusXRAnchorResult::Type SaveAnchorList(const TArray<uint64>& AnchorHandles, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId) = 0;
	virtual EOculusXRAnchorResult::Type SaveAnchors(const TArray<uint64>& AnchorHandles, uint64& OutRequestId) = 0;

	virtual EOculusXRAnchorResult::Type DiscoverAnchors(const FOculusXRSpaceDiscoveryInfo& DiscoveryInfo, uint64& OutRequestId) = 0;
	virtual EOculusXRAnchorResult::Type QueryAnchors(const FOculusXRSpaceQueryInfo& QueryInfo, uint64& OutRequestId) = 0;
	virtual EOculusXRAnchorResult::Type ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<uint64>& UserIds, uint64& OutRequestId) = 0;
	virtual EOculusXRAnchorResult::Type ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<FOculusXRUUID>& Groups, uint64& OutRequestId) = 0;

	virtual EOculusXRAnchorResult::Type EraseAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId) = 0;
	virtual EOculusXRAnchorResult::Type EraseAnchors(const TArray<FOculusXRUInt64>& AnchorHandles, const TArray<FOculusXRUUID>& UUIDs, uint64& OutRequestId) = 0;
};
