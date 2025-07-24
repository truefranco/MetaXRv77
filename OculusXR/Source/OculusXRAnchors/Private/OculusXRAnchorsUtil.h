// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "openxr/OculusXRAnchorsXRIncludes.h"
#include "OculusXRAnchorTypes.h"
#include "OculusXRHMDPrivate.h"

namespace OculusXRAnchors
{
	OCULUSXRANCHORS_API EOculusXRAnchorResult::Type GetResultFromOVRResult(ovrpResult OVRResult);
	OCULUSXRANCHORS_API EOculusXRAnchorResult::Type GetResultFromXrResult(XrResult Result);
	OCULUSXRANCHORS_API bool IsAnchorResultSuccess(EOculusXRAnchorResult::Type Result);

	OCULUSXRANCHORS_API EOculusXRSpaceComponentType ToComponentType(XrSpaceComponentTypeFB XrComponentType);
	OCULUSXRANCHORS_API XrSpaceComponentTypeFB ToComponentType(EOculusXRSpaceComponentType ComponentType);

	OCULUSXRANCHORS_API EOculusXRSpaceStorageLocation ToStorageLocation(XrSpaceStorageLocationFB XrStorageLocation);
	OCULUSXRANCHORS_API XrSpaceStorageLocationFB ToStorageLocation(EOculusXRSpaceStorageLocation StorageLocation);

	OCULUSXRANCHORS_API FOculusXRUUID ToUuid(const XrUuidEXT& XrUuid);
	OCULUSXRANCHORS_API XrUuidEXT ToUuid(const FOculusXRUUID& Uuid);

	OCULUSXRANCHORS_API FString GetStringFromResult(EOculusXRAnchorResult::Type Result);
	OCULUSXRANCHORS_API FString ToString(EOculusXRSpaceComponentType ComponentType);
	OCULUSXRANCHORS_API FString ToString(EOculusXRSpaceStorageLocation StorageLocation);
} // namespace OculusXRAnchors
