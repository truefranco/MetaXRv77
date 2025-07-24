// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "openxr/OculusXRColocationXRIncludes.h"
#include "OculusXRAnchorTypes.h"
#include "OculusXRColocationTypes.h"
#include "OculusXRHMDPrivate.h"

namespace OculusXRColocation
{
	EColocationResult GetResult(ovrpResult OVRResult);
	EColocationResult GetResult(XrResult XRResult);

	const FString& ToString(EColocationResult Result);

	bool IsResultSuccess(EColocationResult Result);
} // namespace OculusXRColocation
