// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXREyeTrackingXR.h"
#include "OpenXRCore.h"
#include "IOpenXRHMDModule.h"
#include "OpenXRHMD.h"
#include "OculusXRMovementLog.h"
#include "OpenXR/OculusXROpenXRUtilities.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

namespace XRMovement
{
	PFN_xrCreateEyeTrackerFB xrCreateEyeTrackerFB = nullptr;
	PFN_xrDestroyEyeTrackerFB xrDestroyEyeTrackerFB = nullptr;
	PFN_xrGetEyeGazesFB xrGetEyeGazesFB = nullptr;

	FEyeTrackingXR::FEyeTrackingXR()
		: bExtEyeTrackingEnabled(false)
		, OpenXRHMD(nullptr)
		, EyeTracker(nullptr)
	{
		CachedEyeState.EyeGazes.SetNum(2);
	}

	FEyeTrackingXR::~FEyeTrackingXR()
	{
	}

	void FEyeTrackingXR::RegisterAsOpenXRExtension()
	{
		RegisterOpenXRExtensionModularFeature();
	}

	bool FEyeTrackingXR::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_FB_EYE_TRACKING_SOCIAL_EXTENSION_NAME);
		return true;
	}

	bool FEyeTrackingXR::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		return true;
	}

	const void* FEyeTrackingXR::OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext)
	{
		if (InModule != nullptr)
		{
			bExtEyeTrackingEnabled = InModule->IsExtensionEnabled(XR_FB_EYE_TRACKING_SOCIAL_EXTENSION_NAME);
		}
		return InNext;
	}

	const void* FEyeTrackingXR::OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext)
	{
		InitOpenXRFunctions(InInstance);

		OpenXRHMD = (FOpenXRHMD*)GEngine->XRSystem.Get();

		return InNext;
	}

	void FEyeTrackingXR::OnDestroySession(XrSession InSession)
	{
		OpenXRHMD = nullptr;
	}

	void FEyeTrackingXR::OnBeginRendering_GameThread(XrSession InSession, FSceneViewFamily& InViewFamily, TArrayView<const uint32> VisibleLayers)
	{
		Update_GameThread(InSession);
	}

	XrResult FEyeTrackingXR::StartEyeTracking()
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartEyeTracking] Cannot start eye tracking, the instance or session is null."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsEyeTrackingSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartEyeTracking] Cannot start eye tracking, body tracking is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (EyeTracker != XR_NULL_HANDLE)
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartEyeTracking] Cannot start eye tracking, body tracking is already started."));
			return XR_SUCCESS;
		}

		XrEyeTrackerCreateInfoFB createInfo = { XR_TYPE_EYE_TRACKER_CREATE_INFO_FB, nullptr };

		auto result = XRMovement::xrCreateEyeTrackerFB(OpenXRHMD->GetSession(), &createInfo, &EyeTracker);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartEyeTracking] Failed to start eye tracking. Result(%d)"), result);
		}

		return result;
	}

	XrResult FEyeTrackingXR::StopEyeTracking()
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopEyeTracking] Cannot stop eye tracking, the instance or session is null."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsEyeTrackingSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopEyeTracking] Cannot stop eye tracking, eye tracking is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrResult result = XR_SUCCESS;
		if (IsEyeTrackingEnabled())
		{
			result = XRMovement::xrDestroyEyeTrackerFB(EyeTracker);
			if (XR_FAILED(result))
			{
				UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopEyeTracking] Failed to stop eye tracking. Result(%d)"), result);
			}
		}

		EyeTracker = XR_NULL_HANDLE;

		return result;
	}

	XrResult FEyeTrackingXR::GetCachedEyeState(FOculusXREyeGazesState& OutState)
	{
		if (!IsEyeTrackingEnabled())
		{
			return XR_ERROR_VALIDATION_FAILURE;
		}

		OutState = CachedEyeState;
		return XR_SUCCESS;
	}

	void FEyeTrackingXR::InitOpenXRFunctions(XrInstance InInstance)
	{
		// XR_FB_Eye_Tracking_Social
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrCreateEyeTrackerFB", &xrCreateEyeTrackerFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrDestroyEyeTrackerFB", &xrDestroyEyeTrackerFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetEyeGazesFB", &xrGetEyeGazesFB);
	}

	void FEyeTrackingXR::Update_GameThread(XrSession InSession)
	{
		check(IsInGameThread());

		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession() || !IsEyeTrackingSupported() || !IsEyeTrackingEnabled())
		{
			return;
		}

		XrEyeGazesInfoFB info{ XR_TYPE_EYE_GAZES_INFO_FB, nullptr };
		info.baseSpace = OpenXRHMD->GetTrackingSpace();
		info.time = OpenXRHMD->GetDisplayTime();

		XrEyeGazesFB gazes{ XR_TYPE_EYE_GAZES_FB, nullptr };

		auto result = XRMovement::xrGetEyeGazesFB(EyeTracker, &info, &gazes);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[EyeGazeStateUpdate] Failed to get gazes state. Result(%d)"), result);
			return;
		}

		auto ApplyGazeToUEType = [this](const XrEyeGazeFB& xrGaze, FOculusXREyeGazeState& outUEGaze) {
			outUEGaze.bIsValid = static_cast<bool>(xrGaze.isValid);
			outUEGaze.Confidence = xrGaze.gazeConfidence;
			outUEGaze.Orientation = FRotator(ToFQuat(xrGaze.gazePose.orientation));
			outUEGaze.Position = ToFVector(xrGaze.gazePose.position) * OpenXRHMD->GetWorldToMetersScale();
		};

		ApplyGazeToUEType(gazes.gaze[0], CachedEyeState.EyeGazes[0]);
		ApplyGazeToUEType(gazes.gaze[1], CachedEyeState.EyeGazes[1]);
	}

} // namespace XRMovement

#undef LOCTEXT_NAMESPACE
