// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRMovementXRIncludes.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRMovementTypes.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

class FOpenXRHMD;

namespace XRMovement
{
	extern PFN_xrCreateEyeTrackerFB xrCreateEyeTrackerFB;
	extern PFN_xrDestroyEyeTrackerFB xrDestroyEyeTrackerFB;
	extern PFN_xrGetEyeGazesFB xrGetEyeGazesFB;

	class FEyeTrackingXR : public IOpenXRExtensionPlugin
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
		FEyeTrackingXR();
		virtual ~FEyeTrackingXR();
		void RegisterAsOpenXRExtension();

		bool IsEyeTrackingSupported() const { return bExtEyeTrackingEnabled; }
		bool IsEyeTrackingEnabled() const { return EyeTracker != XR_NULL_HANDLE; }

		XrResult StartEyeTracking();
		XrResult StopEyeTracking();
		XrResult GetCachedEyeState(FOculusXREyeGazesState& OutState);

	private:
		void InitOpenXRFunctions(XrInstance InInstance);
		void Update_GameThread(XrSession InSession);

		bool bExtEyeTrackingEnabled;

		FOpenXRHMD* OpenXRHMD;
		FOculusXREyeGazesState CachedEyeState;
		XrEyeTrackerFB EyeTracker = XR_NULL_HANDLE;
	};

} // namespace XRMovement

#undef LOCTEXT_NAMESPACE
