// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "khronos/openxr/openxr.h"

namespace OculusXRInput
{
	extern PFN_xrGetDeviceSampleRateFB xrGetDeviceSampleRateFB;

	void InitOpenXRFunctions(XrInstance InInstance);
} // namespace OculusXRInput
