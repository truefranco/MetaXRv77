// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAnchorTypes.h"
#include "OculusXRColocationTypes.generated.h"

UENUM(BlueprintType)
enum class EColocationResult : uint8
{
	Success,
	Success_AlreadyAdvertising,
	Success_AlreadyDiscovering,
	Failure,
	FailureInvalidParameter,
	FailureDataIsInvalid,
	NetworkTimeout,
	NetworkRequestFailed,
	InsufficientPermissions,
	CloudStorageDisabled,
	NoDiscoveryMethodAvailable,
};
