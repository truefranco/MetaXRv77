// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRAnchorsUtil.h"
#include <khronos/openxr/openxr.h>

namespace OculusXRAnchors
{
	EOculusXRAnchorResult::Type GetResultFromOVRResult(ovrpResult OVRResult)
	{
		switch (OVRResult)
		{
			case ovrpSuccess:
				return EOculusXRAnchorResult::Success;
			case ovrpWarning_BoundaryVisibilitySuppressionNotAllowed:
				return EOculusXRAnchorResult::Warning_BoundaryVisibilitySuppressionNotAllowed;

			case ovrpFailure:
				return EOculusXRAnchorResult::Failure;
			case ovrpFailure_InvalidParameter:
				return EOculusXRAnchorResult::Failure_InvalidParameter;
			case ovrpFailure_NotInitialized:
				return EOculusXRAnchorResult::Failure_NotInitialized;
			case ovrpFailure_InvalidOperation:
				return EOculusXRAnchorResult::Failure_InvalidOperation;
			case ovrpFailure_Unsupported:
				return EOculusXRAnchorResult::Failure_Unsupported;
			case ovrpFailure_NotYetImplemented:
				return EOculusXRAnchorResult::Failure_NotYetImplemented;
			case ovrpFailure_OperationFailed:
				return EOculusXRAnchorResult::Failure_OperationFailed;
			case ovrpFailure_InsufficientSize:
				return EOculusXRAnchorResult::Failure_InsufficientSize;
			case ovrpFailure_DataIsInvalid:
				return EOculusXRAnchorResult::Failure_DataIsInvalid;
			case ovrpFailure_DeprecatedOperation:
				return EOculusXRAnchorResult::Failure_DeprecatedOperation;
			case ovrpFailure_ErrorLimitReached:
				return EOculusXRAnchorResult::Failure_ErrorLimitReached;
			case ovrpFailure_ErrorInitializationFailed:
				return EOculusXRAnchorResult::Failure_ErrorInitializationFailed;

				// Query Spaces
			case ovrpFailure_SpaceCloudStorageDisabled:
				return EOculusXRAnchorResult::Failure_SpaceCloudStorageDisabled;
			case ovrpFailure_SpaceMappingInsufficient:
				return EOculusXRAnchorResult::Failure_SpaceMappingInsufficient;
			case ovrpFailure_SpaceLocalizationFailed:
				return EOculusXRAnchorResult::Failure_SpaceLocalizationFailed;
			case ovrpFailure_SpaceNetworkTimeout:
				return EOculusXRAnchorResult::Failure_SpaceNetworkTimeout;
			case ovrpFailure_SpaceNetworkRequestFailed:
				return EOculusXRAnchorResult::Failure_SpaceNetworkRequestFailed;

				// APD
			case ovrpFailure_SpaceInsufficientResources:
				return EOculusXRAnchorResult::Failure_SpaceInsufficientResources;
			case ovrpFailure_SpaceStorageAtCapacity:
				return EOculusXRAnchorResult::Failure_SpaceStorageAtCapacity;
			case ovrpFailure_SpaceInsufficientView:
				return EOculusXRAnchorResult::Failure_SpaceInsufficientView;
			case ovrpFailure_SpacePermissionInsufficient:
				return EOculusXRAnchorResult::Failure_SpacePermissionInsufficient;
			case ovrpFailure_SpaceRateLimited:
				return EOculusXRAnchorResult::Failure_SpaceRateLimited;
			case ovrpFailure_SpaceTooDark:
				return EOculusXRAnchorResult::Failure_SpaceTooDark;
			case ovrpFailure_SpaceTooBright:
				return EOculusXRAnchorResult::Failure_SpaceTooBright;

			default:
				return OVRP_SUCCESS(OVRResult) ? EOculusXRAnchorResult::Success : EOculusXRAnchorResult::Failure;
		}
	}

	EOculusXRAnchorResult::Type GetResultFromXrResult(XrResult Result)
	{
		switch (Result)
		{
			case XR_SUCCESS:
				return EOculusXRAnchorResult::Success;
			case XR_BOUNDARY_VISIBILITY_SUPPRESSION_NOT_ALLOWED_META:
				return EOculusXRAnchorResult::Warning_BoundaryVisibilitySuppressionNotAllowed;

			case XR_ERROR_VALIDATION_FAILURE:
				return EOculusXRAnchorResult::Failure_InvalidParameter;
			case XR_ERROR_RUNTIME_FAILURE:
				return EOculusXRAnchorResult::Failure_OperationFailed;
			case XR_ERROR_FEATURE_UNSUPPORTED:
				return EOculusXRAnchorResult::Failure_Unsupported;
			case XR_ERROR_FUNCTION_UNSUPPORTED:
				return EOculusXRAnchorResult::Failure_NotYetImplemented;
			case XR_ERROR_SIZE_INSUFFICIENT:
				return EOculusXRAnchorResult::Failure_InsufficientSize;
			case XR_ERROR_LIMIT_REACHED:
				return EOculusXRAnchorResult::Failure_ErrorLimitReached;
			case XR_ERROR_INITIALIZATION_FAILED:
				return EOculusXRAnchorResult::Failure_ErrorInitializationFailed;

				// Query Spaces
			case XR_ERROR_SPACE_CLOUD_STORAGE_DISABLED_FB:
				return EOculusXRAnchorResult::Failure_SpaceCloudStorageDisabled;
			case XR_ERROR_SPACE_MAPPING_INSUFFICIENT_FB:
				return EOculusXRAnchorResult::Failure_SpaceMappingInsufficient;
			case XR_ERROR_SPACE_LOCALIZATION_FAILED_FB:
				return EOculusXRAnchorResult::Failure_SpaceLocalizationFailed;
			case XR_ERROR_SPACE_NETWORK_TIMEOUT_FB:
				return EOculusXRAnchorResult::Failure_SpaceNetworkTimeout;
			case XR_ERROR_SPACE_NETWORK_REQUEST_FAILED_FB:
				return EOculusXRAnchorResult::Failure_SpaceNetworkRequestFailed;

				// APD
			case XR_ERROR_SPACE_INSUFFICIENT_RESOURCES_META:
				return EOculusXRAnchorResult::Failure_SpaceInsufficientResources;
			case XR_ERROR_SPACE_STORAGE_AT_CAPACITY_META:
				return EOculusXRAnchorResult::Failure_SpaceStorageAtCapacity;
			case XR_ERROR_SPACE_INSUFFICIENT_VIEW_META:
				return EOculusXRAnchorResult::Failure_SpaceInsufficientView;
			case XR_ERROR_SPACE_PERMISSION_INSUFFICIENT_META:
				return EOculusXRAnchorResult::Failure_SpacePermissionInsufficient;
			case XR_ERROR_SPACE_RATE_LIMITED_META:
				return EOculusXRAnchorResult::Failure_SpaceRateLimited;
			case XR_ERROR_SPACE_TOO_DARK_META:
				return EOculusXRAnchorResult::Failure_SpaceTooDark;
			case XR_ERROR_SPACE_TOO_BRIGHT_META:
				return EOculusXRAnchorResult::Failure_SpaceTooBright;

			default:
				return XR_SUCCEEDED(Result) ? EOculusXRAnchorResult::Success : EOculusXRAnchorResult::Failure;
		}
	}

	OCULUSXRANCHORS_API bool IsAnchorResultSuccess(EOculusXRAnchorResult::Type Result)
	{
		return Result == EOculusXRAnchorResult::Success || Result == EOculusXRAnchorResult::Warning_BoundaryVisibilitySuppressionNotAllowed;
	}

	EOculusXRSpaceComponentType ToComponentType(XrSpaceComponentTypeFB XrType)
	{
		switch (XrType)
		{
			case XR_SPACE_COMPONENT_TYPE_LOCATABLE_FB:
				return EOculusXRSpaceComponentType::Locatable;
			case XR_SPACE_COMPONENT_TYPE_STORABLE_FB:
				return EOculusXRSpaceComponentType::Storable;
			case XR_SPACE_COMPONENT_TYPE_SHARABLE_FB:
				return EOculusXRSpaceComponentType::Sharable;
			case XR_SPACE_COMPONENT_TYPE_BOUNDED_2D_FB:
				return EOculusXRSpaceComponentType::ScenePlane;
			case XR_SPACE_COMPONENT_TYPE_BOUNDED_3D_FB:
				return EOculusXRSpaceComponentType::SceneVolume;
			case XR_SPACE_COMPONENT_TYPE_SEMANTIC_LABELS_FB:
				return EOculusXRSpaceComponentType::SemanticClassification;
			case XR_SPACE_COMPONENT_TYPE_ROOM_LAYOUT_FB:
				return EOculusXRSpaceComponentType::RoomLayout;
			case XR_SPACE_COMPONENT_TYPE_SPACE_CONTAINER_FB:
				return EOculusXRSpaceComponentType::SpaceContainer;
			case XR_SPACE_COMPONENT_TYPE_TRIANGLE_MESH_META:
				return EOculusXRSpaceComponentType::TriangleMesh;
			default:
				return EOculusXRSpaceComponentType::Undefined;
		}
	}

	XrSpaceComponentTypeFB ToComponentType(EOculusXRSpaceComponentType ComponentType)
	{
		switch (ComponentType)
		{
			case EOculusXRSpaceComponentType::Locatable:
				return XR_SPACE_COMPONENT_TYPE_LOCATABLE_FB;
			case EOculusXRSpaceComponentType::Storable:
				return XR_SPACE_COMPONENT_TYPE_STORABLE_FB;
			case EOculusXRSpaceComponentType::Sharable:
				return XR_SPACE_COMPONENT_TYPE_SHARABLE_FB;
			case EOculusXRSpaceComponentType::ScenePlane:
				return XR_SPACE_COMPONENT_TYPE_BOUNDED_2D_FB;
			case EOculusXRSpaceComponentType::SceneVolume:
				return XR_SPACE_COMPONENT_TYPE_BOUNDED_3D_FB;
			case EOculusXRSpaceComponentType::SemanticClassification:
				return XR_SPACE_COMPONENT_TYPE_SEMANTIC_LABELS_FB;
			case EOculusXRSpaceComponentType::RoomLayout:
				return XR_SPACE_COMPONENT_TYPE_ROOM_LAYOUT_FB;
			case EOculusXRSpaceComponentType::SpaceContainer:
				return XR_SPACE_COMPONENT_TYPE_SPACE_CONTAINER_FB;
			case EOculusXRSpaceComponentType::TriangleMesh:
				return XR_SPACE_COMPONENT_TYPE_TRIANGLE_MESH_META;
			default:
				return XR_SPACE_COMPONENT_TYPE_MAX_ENUM_FB;
		}
	}

	EOculusXRSpaceStorageLocation ToStorageLocation(XrSpaceStorageLocationFB XrStorageLocation)
	{
		switch (XrStorageLocation)
		{
			case XR_SPACE_STORAGE_LOCATION_LOCAL_FB:
				return EOculusXRSpaceStorageLocation::Local;
			case XR_SPACE_STORAGE_LOCATION_CLOUD_FB:
				return EOculusXRSpaceStorageLocation::Cloud;
			default:
				return EOculusXRSpaceStorageLocation::Invalid;
		}
	}

	XrSpaceStorageLocationFB ToStorageLocation(EOculusXRSpaceStorageLocation StorageLocation)
	{
		switch (StorageLocation)
		{
			case EOculusXRSpaceStorageLocation::Local:
				return XR_SPACE_STORAGE_LOCATION_LOCAL_FB;
			case EOculusXRSpaceStorageLocation::Cloud:
				return XR_SPACE_STORAGE_LOCATION_CLOUD_FB;
			default:
				return XR_SPACE_STORAGE_LOCATION_INVALID_FB;
		}
	}

	FOculusXRUUID ToUuid(const XrUuidEXT& XrUuid)
	{
		return FOculusXRUUID(XrUuid.data);
	}

	XrUuidEXT ToUuid(const FOculusXRUUID& Uuid)
	{
		XrUuidEXT result;
		FMemory::Memcpy(result.data, Uuid.UUIDBytes);
		return result;
	}

	FString GetStringFromResult(EOculusXRAnchorResult::Type Result)
	{
		return UEnum::GetDisplayValueAsText(Result).ToString();
	}

	FString ToString(EOculusXRSpaceComponentType ComponentType)
	{
		// Todo: More performant to use const strings and a case statement?
		return UEnum::GetDisplayValueAsText(ComponentType).ToString();
	}

	FString ToString(EOculusXRSpaceStorageLocation StorageLocation)
	{
		// Todo: More performant to use const strings and a case statement?
		return UEnum::GetDisplayValueAsText(StorageLocation).ToString();
	}
} // namespace OculusXRAnchors
