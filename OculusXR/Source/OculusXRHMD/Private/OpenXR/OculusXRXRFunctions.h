// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "khronos/openxr/openxr.h"
#include "Misc/Optional.h"

namespace OculusXR
{
	extern TOptional<PFN_xrPerfSettingsSetPerformanceLevelEXT> xrPerfSettingsSetPerformanceLevelEXT;
	extern TOptional<PFN_xrQueryPerformanceMetricsCounterMETA> xrQueryPerformanceMetricsCounterMETA;
	extern TOptional<PFN_xrEnumeratePerformanceMetricsCounterPathsMETA> xrEnumeratePerformanceMetricsCounterPathsMETA;
	extern TOptional<PFN_xrSetPerformanceMetricsStateMETA> xrSetPerformanceMetricsStateMETA;
	extern TOptional<PFN_xrGetDisplayRefreshRateFB> xrGetDisplayRefreshRateFB;
	extern TOptional<PFN_xrRequestDisplayRefreshRateFB> xrRequestDisplayRefreshRateFB;
	extern TOptional<PFN_xrEnumerateDisplayRefreshRatesFB> xrEnumerateDisplayRefreshRatesFB;
	extern TOptional<PFN_xrSetColorSpaceFB> xrSetColorSpaceFB;
	extern TOptional<PFN_xrGetPassthroughPreferencesMETA> xrGetPassthroughPreferencesMETA;
	extern TOptional<PFN_xrGetRecommendedLayerResolutionMETA> xrGetRecommendedLayerResolutionMETA;

	void InitOpenXRFunctions(XrInstance InInstance);
} // namespace OculusXR
