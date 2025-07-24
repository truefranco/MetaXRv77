// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRAnchorManager.h"

#include <vector>

#include "OculusXRHMD.h"
#include "OculusXRAnchorsModule.h"
#include "OculusXRAnchorDelegates.h"
#include "OculusXRAnchorBPFunctionLibrary.h"
#include "OculusXRAnchorTypesPrivate.h"

#include "OculusXRAnchorFunctionsOVR.h"
#include "OculusXRAnchorFunctionsOpenXR.h"

namespace OculusXRAnchors
{
	EOculusXRAnchorResult::Type FOculusXRAnchorManager::CreateAnchor(const FTransform& InTransform, uint64& OutRequestId, const FTransform& CameraTransform)
	{
		return GetOculusXRAnchorFunctionsImpl()->CreateAnchor(InTransform, OutRequestId, CameraTransform);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::DestroyAnchor(uint64 AnchorHandle)
	{
		return GetOculusXRAnchorFunctionsImpl()->DestroyAnchor(AnchorHandle);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::TryGetAnchorTransform(uint64 AnchorHandle, FTransform& OutTransform, FOculusXRAnchorLocationFlags& OutLocationFlags, EOculusXRAnchorSpace Space)
	{
		return GetOculusXRAnchorFunctionsImpl()->TryGetAnchorTransform(AnchorHandle, OutTransform, OutLocationFlags, Space);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::SetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool Enable, float Timeout, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->SetAnchorComponentStatus(AnchorHandle, ComponentType, Enable, Timeout, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool& OutEnabled, bool& OutChangePending)
	{
		return GetOculusXRAnchorFunctionsImpl()->GetAnchorComponentStatus(AnchorHandle, ComponentType, OutEnabled, OutChangePending);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetSupportedAnchorComponents(uint64 AnchorHandle, TArray<EOculusXRSpaceComponentType>& OutSupportedTypes)
	{
		return GetOculusXRAnchorFunctionsImpl()->GetSupportedAnchorComponents(AnchorHandle, OutSupportedTypes);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::GetAnchorContainerUUIDs(uint64 AnchorHandle, TArray<FOculusXRUUID>& OutUUIDs)
	{
		return GetOculusXRAnchorFunctionsImpl()->GetAnchorContainerUUIDs(AnchorHandle, OutUUIDs);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::SaveAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, EOculusXRSpaceStoragePersistenceMode StoragePersistenceMode, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->SaveAnchor(AnchorHandle, StorageLocation, StoragePersistenceMode, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::SaveAnchorList(const TArray<uint64>& AnchorHandles, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->SaveAnchorList(AnchorHandles, StorageLocation, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::SaveAnchors(const TArray<uint64>& AnchorHandles, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->SaveAnchors(AnchorHandles, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::DiscoverAnchors(const FOculusXRSpaceDiscoveryInfo& DiscoveryInfo, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->DiscoverAnchors(DiscoveryInfo, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::QueryAnchors(const FOculusXRSpaceQueryInfo& QueryInfo, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->QueryAnchors(QueryInfo, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<uint64>& UserIds, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->ShareAnchors(AnchorHandles, UserIds, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::ShareAnchors(const TArray<uint64>& AnchorHandles, FOculusXRUUID GroupId, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->ShareAnchors(AnchorHandles, { GroupId }, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<FOculusXRUUID>& Groups, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->ShareAnchors(AnchorHandles, Groups, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::EraseAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->EraseAnchor(AnchorHandle, StorageLocation, OutRequestId);
	}

	EOculusXRAnchorResult::Type FOculusXRAnchorManager::EraseAnchors(const TArray<FOculusXRUInt64>& AnchorHandles, const TArray<FOculusXRUUID>& UUIDs, uint64& OutRequestId)
	{
		return GetOculusXRAnchorFunctionsImpl()->EraseAnchors(AnchorHandles, UUIDs, OutRequestId);
	}

	TSharedPtr<IOculusXRAnchorFunctions> FOculusXRAnchorManager::AnchorFunctionsImpl = nullptr;
	TSharedPtr<IOculusXRAnchorFunctions> FOculusXRAnchorManager::GetOculusXRAnchorFunctionsImpl()
	{
		if (AnchorFunctionsImpl == nullptr)
		{
			const FName SystemName(TEXT("OpenXR"));
			const bool IsOpenXR = GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName);
			if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
			{
				AnchorFunctionsImpl = MakeShared<FOculusXRAnchorFunctionsOVR>();
			}
			else if (IsOpenXR)
			{
				AnchorFunctionsImpl = MakeShared<FOculusXRAnchorFunctionsOpenXR>();
			}
		}

		check(AnchorFunctionsImpl);
		return AnchorFunctionsImpl;
	}

} // namespace OculusXRAnchors
