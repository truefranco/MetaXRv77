// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationUtil.h"

namespace OculusXRColocation
{
	EColocationResult GetResult(ovrpResult OVRResult)
	{
		switch (OVRResult)
		{
			case ovrpSuccess:
				return EColocationResult::Success;
			case ovrpSuccess_ColocationDiscoveryAlreadyAdvertising:
				return EColocationResult::Success_AlreadyAdvertising;
			case ovrpSuccess_ColocationDiscoveryAlreadyDiscovering:
				return EColocationResult::Success_AlreadyDiscovering;
			case ovrpFailure:
				return EColocationResult::Failure;
			case ovrpFailure_InvalidParameter:
				return EColocationResult::FailureInvalidParameter;
			case ovrpFailure_DataIsInvalid:
				return EColocationResult::FailureDataIsInvalid;
			case ovrpFailure_SpacePermissionInsufficient:
				return EColocationResult::InsufficientPermissions;
			case ovrpFailure_SpaceCloudStorageDisabled:
				return EColocationResult::CloudStorageDisabled;
			case ovrpFailure_SpaceNetworkTimeout:
				return EColocationResult::NetworkTimeout;
			case ovrpFailure_SpaceNetworkRequestFailed:
				return EColocationResult::NetworkRequestFailed;
			case ovrpFailure_ColocationDiscoveryNetworkFailed:
				return EColocationResult::NetworkRequestFailed;
			case ovrpFailure_ColocationDiscoveryNoDiscoveryMethodAvailable:
				return EColocationResult::NoDiscoveryMethodAvailable;
			default:
				return OVRP_SUCCESS(OVRResult) ? EColocationResult::Success : EColocationResult::Failure;
		}
	}

	EColocationResult GetResult(XrResult XRResult)
	{
		switch (XRResult)
		{
			case XR_SUCCESS:
				return EColocationResult::Success;
			case XR_COLOCATION_DISCOVERY_ALREADY_ADVERTISING_META:
				return EColocationResult::Success_AlreadyAdvertising;
			case XR_COLOCATION_DISCOVERY_ALREADY_DISCOVERING_META:
				return EColocationResult::Success_AlreadyDiscovering;
			case XR_ERROR_RUNTIME_FAILURE:
				return EColocationResult::Failure;
			case XR_ERROR_VALIDATION_FAILURE:
				return EColocationResult::FailureInvalidParameter;
			case XR_ERROR_SPACE_PERMISSION_INSUFFICIENT_META:
				return EColocationResult::InsufficientPermissions;
			case XR_ERROR_SPACE_CLOUD_STORAGE_DISABLED_FB:
				return EColocationResult::CloudStorageDisabled;
			case XR_ERROR_SPACE_NETWORK_TIMEOUT_FB:
				return EColocationResult::NetworkTimeout;
			case XR_ERROR_SPACE_NETWORK_REQUEST_FAILED_FB:
				return EColocationResult::NetworkRequestFailed;
			case XR_ERROR_COLOCATION_DISCOVERY_NETWORK_FAILED_META:
				return EColocationResult::NetworkRequestFailed;
			case XR_ERROR_COLOCATION_DISCOVERY_NO_DISCOVERY_METHOD_META:
				return EColocationResult::NoDiscoveryMethodAvailable;
			default:
				return XR_SUCCEEDED(XRResult) ? EColocationResult::Success : EColocationResult::Failure;
		}
	}

	const FString& ToString(EColocationResult Result)
	{
		// We could use UEnum::GetDisplayValueAsText but that will allocate!
		const static TMap<EColocationResult, FString> Mapping = {
			{ EColocationResult::Success, "Success" },
			{ EColocationResult::Success_AlreadyAdvertising, "Success_AlreadyAdvertising" },
			{ EColocationResult::Success_AlreadyDiscovering, "Success_AlreadyDiscovering" },
			{ EColocationResult::Failure, "Failure" },
			{ EColocationResult::FailureDataIsInvalid, "FailureDataIsInvalid" },
			{ EColocationResult::FailureInvalidParameter, "FailureInvalidParameter" },
			{ EColocationResult::InsufficientPermissions, "InsufficientPermissions" },
			{ EColocationResult::CloudStorageDisabled, "CloudStorageDisabled" },
			{ EColocationResult::NetworkRequestFailed, "NetworkRequestFailed" },
			{ EColocationResult::NetworkTimeout, "NetworkTimeout" },
			{ EColocationResult::NoDiscoveryMethodAvailable, "NoDiscoveryMethodAvailable" }
		};

		const static FString Invalid = "EColocationResult(Unknown)";
		return Mapping.Contains(Result) ? Mapping[Result] : Invalid;
	}

	bool IsResultSuccess(EColocationResult Result)
	{
		return Result == EColocationResult::Success || Result == EColocationResult::Success_AlreadyAdvertising || Result == EColocationResult::Success_AlreadyDiscovering;
	}
} // namespace OculusXRColocation
