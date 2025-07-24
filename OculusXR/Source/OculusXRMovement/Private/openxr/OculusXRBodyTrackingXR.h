// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRMovementXRIncludes.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRMovementTypes.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

class FOpenXRHMD;

namespace XRMovement
{
	extern PFN_xrCreateBodyTrackerFB xrCreateBodyTrackerFB;
	extern PFN_xrDestroyBodyTrackerFB xrDestroyBodyTrackerFB;
	extern PFN_xrLocateBodyJointsFB xrLocateBodyJointsFB;
	extern PFN_xrGetBodySkeletonFB xrGetBodySkeletonFB;
	extern PFN_xrRequestBodyTrackingFidelityMETA xrRequestBodyTrackingFidelityMETA;
	extern PFN_xrSuggestBodyTrackingCalibrationOverrideMETA xrSuggestBodyTrackingCalibrationOverrideMETA;
	extern PFN_xrResetBodyTrackingCalibrationMETA xrResetBodyTrackingCalibrationMETA;

	class FBodyTrackingXR : public IOpenXRExtensionPlugin
	{
	public:
		// IOculusXROpenXRHMDPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual const void* OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext) override;
		virtual const void* OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext) override;
		virtual void OnDestroySession(XrSession InSession) override;
		virtual void OnBeginRendering_GameThread(XrSession InSession, FSceneViewFamily& InViewFamily, TArrayView<const uint32> VisibleLayers) override;

	public:
		FBodyTrackingXR();
		virtual ~FBodyTrackingXR();
		void RegisterAsOpenXRExtension();

		bool IsBodyTrackingSupported() const { return bExtBodyTrackingEnabled; }
		bool IsFullBodySupported() const { return bExtBodyTrackingFullBodyEnabled; }
		bool IsFidelitySupported() const { return bExtBodyTrackingFidelityEnabled; }
		bool IsCalibrationSupported() const { return bExtBodyTrackingCalibrationEnabled; }

		bool IsBodyTrackingEnabled() const { return BodyTracker != XR_NULL_HANDLE; }
		bool IsFullBodyTrackingEnabled() const { return FullBodyTracking; }

		XrResult StartBodyTracking();
		XrResult StartBodyTrackingByJointSet(EOculusXRBodyJointSet jointSet);
		XrResult StopBodyTracking();
		XrResult GetCachedBodyState(FOculusXRBodyState& OutState);
		XrResult GetBodySkeleton(FOculusXRBodySkeleton& OutSkeleton);

		XrResult RequestBodyTrackingFidelity(EOculusXRBodyTrackingFidelity Fidelity);
		XrResult ResetBodyTrackingFidelity();
		XrResult SuggestBodyTrackingCalibrationOverride(float height);

	private:
		void InitOpenXRFunctions(XrInstance InInstance);
		void Update_GameThread(XrSession InSession);

		bool bExtBodyTrackingEnabled;
		bool bExtBodyTrackingFullBodyEnabled;
		bool bExtBodyTrackingFidelityEnabled;
		bool bExtBodyTrackingCalibrationEnabled;

		FOpenXRHMD* OpenXRHMD;
		FOculusXRBodyState CachedBodyState;
		XrBodyTrackerFB BodyTracker = XR_NULL_HANDLE;
		bool FullBodyTracking{ false };
	};

} // namespace XRMovement

#undef LOCTEXT_NAMESPACE
