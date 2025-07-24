// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRXRFunctions.h"
#include "OpenXRCore.h"
#include "OculusXROpenXRUtilities.h"

namespace OculusXR
{
	TOptional<PFN_xrPerfSettingsSetPerformanceLevelEXT> xrPerfSettingsSetPerformanceLevelEXT = nullptr;
	TOptional<PFN_xrQueryPerformanceMetricsCounterMETA> xrQueryPerformanceMetricsCounterMETA = nullptr;
	TOptional<PFN_xrEnumeratePerformanceMetricsCounterPathsMETA> xrEnumeratePerformanceMetricsCounterPathsMETA = nullptr;
	TOptional<PFN_xrSetPerformanceMetricsStateMETA> xrSetPerformanceMetricsStateMETA = nullptr;
	TOptional<PFN_xrGetDisplayRefreshRateFB> xrGetDisplayRefreshRateFB = nullptr;
	TOptional<PFN_xrRequestDisplayRefreshRateFB> xrRequestDisplayRefreshRateFB = nullptr;
	TOptional<PFN_xrEnumerateDisplayRefreshRatesFB> xrEnumerateDisplayRefreshRatesFB = nullptr;
	TOptional<PFN_xrSetColorSpaceFB> xrSetColorSpaceFB = nullptr;
	TOptional<PFN_xrGetPassthroughPreferencesMETA> xrGetPassthroughPreferencesMETA = nullptr;
	TOptional<PFN_xrGetRecommendedLayerResolutionMETA> xrGetRecommendedLayerResolutionMETA = nullptr;

	void InitOpenXRFunctions(XrInstance InInstance)
	{
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrPerfSettingsSetPerformanceLevelEXT", &xrPerfSettingsSetPerformanceLevelEXT);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrQueryPerformanceMetricsCounterMETA", &xrQueryPerformanceMetricsCounterMETA);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrEnumeratePerformanceMetricsCounterPathsMETA", &xrEnumeratePerformanceMetricsCounterPathsMETA);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrSetPerformanceMetricsStateMETA", &xrSetPerformanceMetricsStateMETA);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetDisplayRefreshRateFB", &xrGetDisplayRefreshRateFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrRequestDisplayRefreshRateFB", &xrRequestDisplayRefreshRateFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrEnumerateDisplayRefreshRatesFB", &xrEnumerateDisplayRefreshRatesFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrSetColorSpaceFB", &xrSetColorSpaceFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetPassthroughPreferencesMETA", &xrGetPassthroughPreferencesMETA);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetRecommendedLayerResolutionMETA", &xrGetRecommendedLayerResolutionMETA);
	}

} // namespace OculusXR
