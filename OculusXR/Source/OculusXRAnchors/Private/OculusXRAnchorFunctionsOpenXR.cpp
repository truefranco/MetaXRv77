// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRAnchorFunctionsOpenXR.h"
#include "OculusXRHMD.h"
#include "OculusXRAnchorsModule.h"
#include "OculusXRAnchorDelegates.h"
#include "OculusXRAnchorBPFunctionLibrary.h"
#include "OculusXRAnchorTypesPrivate.h"
#include "OculusXRAnchorsUtil.h"

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::CreateAnchor(const FTransform& InTransform, uint64& OutRequestId, const FTransform& CameraTransform)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->CreateSpatialAnchor(InTransform, OutRequestId, CameraTransform);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::DestroyAnchor(uint64 AnchorHandle)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->DestroySpatialAnchor(AnchorHandle);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::TryGetAnchorTransform(uint64 AnchorHandle, FTransform& OutTransform, FOculusXRAnchorLocationFlags& OutLocationFlags, EOculusXRAnchorSpace Space)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->TryGetAnchorTransform(AnchorHandle, OutTransform, OutLocationFlags, Space);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::SetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool Enable, float Timeout, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->SetAnchorComponentStatus(AnchorHandle, ComponentType, Enable, Timeout, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::GetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool& OutEnabled, bool& OutChangePending)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->GetAnchorComponentStatus(AnchorHandle, ComponentType, OutEnabled, OutChangePending);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::GetSupportedAnchorComponents(uint64 AnchorHandle, TArray<EOculusXRSpaceComponentType>& OutSupportedTypes)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->GetSupportedAnchorComponents(AnchorHandle, OutSupportedTypes);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::GetAnchorContainerUUIDs(uint64 AnchorHandle, TArray<FOculusXRUUID>& OutUUIDs)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->GetAnchorContainerUUIDs(AnchorHandle, OutUUIDs);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::SaveAnchor(uint64 AnchorHandle,
	EOculusXRSpaceStorageLocation StorageLocation,
	EOculusXRSpaceStoragePersistenceMode StoragePersistenceMode, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->SaveAnchor(AnchorHandle, StorageLocation, StoragePersistenceMode, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::SaveAnchorList(const TArray<uint64>& AnchorHandles, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->SaveAnchorList(AnchorHandles, StorageLocation, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::SaveAnchors(const TArray<uint64>& AnchorHandles, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->SaveAnchors(AnchorHandles, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::DiscoverAnchors(const FOculusXRSpaceDiscoveryInfo& DiscoveryInfo, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->DiscoverAnchors(DiscoveryInfo, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::QueryAnchors(const FOculusXRSpaceQueryInfo& QueryInfo, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->QueryAnchors(QueryInfo, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<uint64>& UserIds, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->ShareAnchors(AnchorHandles, UserIds, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<FOculusXRUUID>& Groups, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->ShareAnchorsWithGroups(AnchorHandles, Groups, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::EraseAnchor(uint64 AnchorHandle,
	EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->EraseAnchor(AnchorHandle, StorageLocation, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}

EOculusXRAnchorResult::Type FOculusXRAnchorFunctionsOpenXR::EraseAnchors(const TArray<FOculusXRUInt64>& AnchorHandles, const TArray<FOculusXRUUID>& UUIDs, uint64& OutRequestId)
{
	auto result = FOculusXRAnchorsModule::Get().GetXrAnchors()->EraseAnchors(AnchorHandles, UUIDs, OutRequestId);
	return OculusXRAnchors::GetResultFromXrResult(result);
}
