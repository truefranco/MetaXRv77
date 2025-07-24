// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRMovementXRIncludes.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRMovementTypes.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

class FOpenXRHMD;

namespace XRMovement
{
	extern PFN_xrCreateFaceTracker2FB xrCreateEyeTracker2FB;
	extern PFN_xrDestroyFaceTracker2FB xrDestroyEyeTracker2FB;
	extern PFN_xrGetFaceExpressionWeights2FB xrGetFaceExpressionWeights2FB;

	class FFaceTrackingXR : public IOpenXRExtensionPlugin
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
		FFaceTrackingXR();
		virtual ~FFaceTrackingXR();
		void RegisterAsOpenXRExtension();

		bool IsFaceTrackingSupported() const { return bExtFaceTrackingSupported; }
		bool IsFaceTrackingEnabled() const { return FaceTracker != XR_NULL_HANDLE; }
		bool IsFaceTrackingVisemesSupported() const { return bExtFaceTrackingVisemesSupported; }
		bool IsFaceTrackingVisemesEnabled() const { return bVisemesEnabled; }

		XrResult StartFaceTracking();
		XrResult StopFaceTracking();
		XrResult GetCachedFaceState(FOculusXRFaceState& OutState);
		XrResult SetVisemesEnabled(bool enabled);
		XrResult GetCachedVisemeState(FOculusXRFaceVisemesState& OutState);

	private:
		void InitOpenXRFunctions(XrInstance InInstance);
		void Update_GameThread(XrSession InSession);

		bool bExtFaceTrackingSupported;
		bool bExtFaceTrackingVisemesSupported;
		bool bVisemesEnabled;

		FOpenXRHMD* OpenXRHMD;
		FOculusXRFaceState CachedFaceState;
		FOculusXRFaceVisemesState CachedVisemeState;

		XrFaceTracker2FB FaceTracker = XR_NULL_HANDLE;
	};

} // namespace XRMovement

#undef LOCTEXT_NAMESPACE
