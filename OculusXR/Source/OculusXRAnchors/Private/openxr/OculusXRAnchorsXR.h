// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAnchorsXRIncludes.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRAnchorTypes.h"

#define LOCTEXT_NAMESPACE "OculusXRAnchors"

class FOpenXRHMD;

namespace XRAnchors
{
	extern PFN_xrCreateSpatialAnchorFB xrCreateSpatialAnchorFB;
	extern PFN_xrSetSpaceComponentStatusFB xrSetSpaceComponentStatusFB;
	extern PFN_xrGetSpaceComponentStatusFB xrGetSpaceComponentStatusFB;
	extern PFN_xrEnumerateSpaceSupportedComponentsFB xrEnumerateSpaceSupportedComponentsFB;
	extern PFN_xrGetSpaceUuidFB xrGetSpaceUuidFB;
	extern PFN_xrGetSpaceContainerFB xrGetSpaceContainerFB;
	extern PFN_xrQuerySpacesFB xrQuerySpacesFB;
	extern PFN_xrRetrieveSpaceQueryResultsFB xrRetrieveSpaceQueryResultsFB;
	extern PFN_xrShareSpacesFB xrShareSpacesFB;
	extern PFN_xrShareSpacesMETA xrShareSpacesMETA;
	extern PFN_xrSaveSpaceFB xrSaveSpaceFB;
	extern PFN_xrEraseSpaceFB xrEraseSpaceFB;
	extern PFN_xrSaveSpaceListFB xrSaveSpaceListFB;
	extern PFN_xrCreateSpaceUserFB xrCreateSpaceUserFB;
	extern PFN_xrDestroySpaceUserFB xrDestroySpaceUserFB;
	extern PFN_xrGetSpaceUserIdFB xrGetSpaceUserIdFB;
	extern PFN_xrSaveSpacesMETA xrSaveSpacesMETA;
	extern PFN_xrEraseSpacesMETA xrEraseSpacesMETA;
	extern PFN_xrDiscoverSpacesMETA xrDiscoverSpacesMETA;
	extern PFN_xrRetrieveSpaceDiscoveryResultsMETA xrRetrieveSpaceDiscoveryResultsMETA;

	class FAnchorsXR : public IOpenXRExtensionPlugin
	{
	public:
		// IOculusXROpenXRHMDPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual const void* OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext) override;
		virtual const void* OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext) override;
		virtual void OnDestroySession(XrSession InSession) override;
		virtual void OnEvent(XrSession InSession, const XrEventDataBaseHeader* InHeader) override;

	public:
		FAnchorsXR();
		virtual ~FAnchorsXR();
		void RegisterAsOpenXRExtension();

		bool IsAnchorExtensionSupported() const { return bExtAnchorsEnabled; }
		bool IsContainerExtensionSupported() const { return bExtContainerEnabled; }
		bool IsQueryExtensionSupported() const { return bExtQueryEnabled; }
		bool IsSharingExtensionSupported() const { return bExtSharingEnabled; }
		bool IsStorageExtensionSupported() const { return bExtStorageEnabled; }
		bool IsStorageBatchExtensionSupported() const { return bExtStorageBatchEnabled; }
		bool IsUserExtensionSupported() const { return bExtUserEnabled; }
		bool IsDiscoveryExtensionSupported() const { return bExtDiscoveryEnabled; }
		bool IsPersistenceExtensionSupported() const { return bExtPersistenceEnabled; }
		bool IsSharingMetaExtensionSupported() const { return bExtSharingMetaEnabled; }
		bool IsGroupSharingExtensionSupported() const { return bExtGroupSharingEnabled; }

		XrResult CreateSpatialAnchor(const FTransform& InTransform, uint64& OutRequestId, const FTransform& CameraTransform);
		XrResult DestroySpatialAnchor(uint64 AnchorHandle);

		XrResult TryGetAnchorTransform(uint64 AnchorHandle, FTransform& OutTransform, FOculusXRAnchorLocationFlags& OutLocationFlags, EOculusXRAnchorSpace Space);
		XrResult SetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool Enable, float Timeout, uint64& OutRequestId);
		XrResult GetAnchorComponentStatus(uint64 AnchorHandle, EOculusXRSpaceComponentType ComponentType, bool& OutEnabled, bool& OutChangePending);
		XrResult GetSupportedAnchorComponents(uint64 AnchorHandle, TArray<EOculusXRSpaceComponentType>& OutSupportedTypes);
		XrResult GetAnchorContainerUUIDs(uint64 AnchorHandle, TArray<FOculusXRUUID>& OutUUIDs);

		XrResult SaveAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, EOculusXRSpaceStoragePersistenceMode StoragePersistenceMode, uint64& OutRequestId);
		XrResult SaveAnchorList(const TArray<uint64>& AnchorHandles, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId);
		XrResult SaveAnchors(const TArray<uint64>& AnchorHandles, uint64& OutRequestId);

		XrResult DiscoverAnchors(const FOculusXRSpaceDiscoveryInfo& DiscoveryInfo, uint64& OutRequestId);
		XrResult QueryAnchors(const FOculusXRSpaceQueryInfo& QueryInfo, uint64& OutRequestId);

		XrResult ShareAnchors(const TArray<uint64>& AnchorHandles, const TArray<uint64>& UserIds, uint64& OutRequestId);
		XrResult ShareAnchorsWithGroups(const TArray<uint64>& AnchorHandles, const TArray<FOculusXRUUID>& Groups, uint64& OutRequestId);

		XrResult CreateSpaceUser(uint64 SpaceUserId, SpaceUser& OutUser);
		XrResult GetSpaceUserId(const SpaceUser& User, uint64& OutId);
		XrResult DestroySpaceUser(const SpaceUser& User);

		XrResult EraseAnchor(uint64 AnchorHandle, EOculusXRSpaceStorageLocation StorageLocation, uint64& OutRequestId);
		XrResult EraseAnchors(const TArray<FOculusXRUInt64>& AnchorHandles, const TArray<FOculusXRUUID>& UUIDs, uint64& OutRequestId);

	private:
		void InitOpenXRFunctions(XrInstance InInstance);

		bool bExtAnchorsEnabled;
		bool bExtContainerEnabled;
		bool bExtQueryEnabled;
		bool bExtSharingEnabled;
		bool bExtStorageEnabled;
		bool bExtStorageBatchEnabled;
		bool bExtUserEnabled;
		bool bExtDiscoveryEnabled;
		bool bExtPersistenceEnabled;
		bool bExtSharingMetaEnabled;
		bool bExtGroupSharingEnabled;

		FOpenXRHMD* OpenXRHMD;
	};

} // namespace XRAnchors

#undef LOCTEXT_NAMESPACE
