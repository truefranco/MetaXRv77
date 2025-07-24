// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRFaceTrackingXR.h"
#include "OpenXRCore.h"
#include "IOpenXRHMDModule.h"
#include "OpenXRHMD.h"
#include "OculusXRMovementLog.h"
#include "OpenXR/OculusXROpenXRUtilities.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

namespace XRMovement
{
	PFN_xrCreateFaceTracker2FB xrCreateFaceTracker2FB = nullptr;
	PFN_xrDestroyFaceTracker2FB xrDestroyFaceTracker2FB = nullptr;
	PFN_xrGetFaceExpressionWeights2FB xrGetFaceExpressionWeights2FB = nullptr;

	FFaceTrackingXR::FFaceTrackingXR()
		: bExtFaceTrackingSupported(false)
		, bExtFaceTrackingVisemesSupported(false)
		, bVisemesEnabled(false)
		, OpenXRHMD(nullptr)
		, FaceTracker(nullptr)
	{
		CachedFaceState.ExpressionWeights.SetNum(XR_FACE_EXPRESSION2_COUNT_FB);
		CachedFaceState.ExpressionWeightConfidences.SetNum(XR_FACE_CONFIDENCE2_COUNT_FB);
		CachedVisemeState.ExpressionVisemeWeights.SetNum(XR_FACE_TRACKING_VISEME_COUNT_METAX1);
	}

	FFaceTrackingXR::~FFaceTrackingXR()
	{
	}

	void FFaceTrackingXR::RegisterAsOpenXRExtension()
	{
		RegisterOpenXRExtensionModularFeature();
	}

	bool FFaceTrackingXR::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_FB_FACE_TRACKING2_EXTENSION_NAME);
		return true;
	}

	bool FFaceTrackingXR::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_METAX1_FACE_TRACKING_VISEMES_EXTENSION_NAME);
		return true;
	}

	const void* FFaceTrackingXR::OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext)
	{
		if (InModule != nullptr)
		{
			bExtFaceTrackingSupported = InModule->IsExtensionEnabled(XR_FB_FACE_TRACKING2_EXTENSION_NAME);
			bExtFaceTrackingVisemesSupported = InModule->IsExtensionEnabled(XR_METAX1_FACE_TRACKING_VISEMES_EXTENSION_NAME);
		}
		return InNext;
	}

	const void* FFaceTrackingXR::OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext)
	{
		InitOpenXRFunctions(InInstance);

		OpenXRHMD = (FOpenXRHMD*)GEngine->XRSystem.Get();

		return InNext;
	}

	void FFaceTrackingXR::OnDestroySession(XrSession InSession)
	{
		OpenXRHMD = nullptr;
	}

	void FFaceTrackingXR::OnBeginRendering_GameThread(XrSession InSession, FSceneViewFamily& InViewFamily, TArrayView<const uint32> VisibleLayers)
	{
		Update_GameThread(InSession);
	}

	XrResult FFaceTrackingXR::StartFaceTracking()
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartFaceTracking] Cannot start face tracking, the instance or session is null."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsFaceTrackingSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartFaceTracking] Cannot start face tracking, face tracking is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (FaceTracker != XR_NULL_HANDLE)
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartFaceTracking] Cannot start face tracking, face tracking is already started."));
			return XR_SUCCESS;
		}

		XrFaceTrackerCreateInfo2FB createInfo = { XR_TYPE_FACE_TRACKER_CREATE_INFO2_FB, nullptr };

		auto result = XRMovement::xrCreateFaceTracker2FB(OpenXRHMD->GetSession(), &createInfo, &FaceTracker);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartFaceTracking] Failed to start face tracking. Result(%d)"), result);
		}

		return result;
	}

	XrResult FFaceTrackingXR::StopFaceTracking()
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopFaceTracking] Cannot stop face tracking, the instance or session is null."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsFaceTrackingSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopFaceTracking] Cannot stop face tracking, face tracking is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrResult result = XR_SUCCESS;
		if (IsFaceTrackingEnabled())
		{
			result = XRMovement::xrDestroyFaceTracker2FB(FaceTracker);
			if (XR_FAILED(result))
			{
				UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopFaceTracking] Failed to stop face tracking. Result(%d)"), result);
			}
		}

		FaceTracker = XR_NULL_HANDLE;

		return result;
	}

	XrResult FFaceTrackingXR::GetCachedFaceState(FOculusXRFaceState& OutState)
	{
		if (!IsFaceTrackingEnabled())
		{
			return XR_ERROR_VALIDATION_FAILURE;
		}

		OutState = CachedFaceState;
		return XR_SUCCESS;
	}

	XrResult FFaceTrackingXR::SetVisemesEnabled(bool enabled)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[SetVisemesEnabled] Cannot change viseme state, the instance of session is null."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsFaceTrackingVisemesSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[SetVisemesEnabled] Cannot change viseme state, visemes are not supported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		UE_LOG(LogOculusXRMovement, Log, TEXT("[SetVisemesEnabled] Changing visemes enabled state: %hs"), enabled ? "TRUE" : "FALSE");
		bVisemesEnabled = enabled;

		return XR_SUCCESS;
	}

	XrResult FFaceTrackingXR::GetCachedVisemeState(FOculusXRFaceVisemesState& OutState)
	{
		if (!IsFaceTrackingEnabled() || !IsFaceTrackingVisemesEnabled())
		{
			return XR_ERROR_VALIDATION_FAILURE;
		}

		OutState = CachedVisemeState;
		return XR_SUCCESS;
	}

	void FFaceTrackingXR::InitOpenXRFunctions(XrInstance InInstance)
	{
		// XR_FB_Eye_Tracking_Social
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrCreateFaceTracker2FB", &xrCreateFaceTracker2FB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrDestroyFaceTracker2FB", &xrDestroyFaceTracker2FB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetFaceExpressionWeights2FB", &xrGetFaceExpressionWeights2FB);
	}

	void FFaceTrackingXR::Update_GameThread(XrSession InSession)
	{
		check(IsInGameThread());

		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession() || !IsFaceTrackingSupported() || !IsFaceTrackingEnabled())
		{
			return;
		}

		XrFaceExpressionInfo2FB info{ XR_TYPE_FACE_EXPRESSION_INFO2_FB, nullptr };
		info.time = OpenXRHMD->GetDisplayTime();

		float weightsArray[XR_FACE_EXPRESSION2_COUNT_FB];
		float confidencesArray[XR_FACE_CONFIDENCE2_COUNT_FB];

		XrFaceExpressionWeights2FB weights{ XR_TYPE_FACE_EXPRESSION_WEIGHTS2_FB, nullptr };
		weights.weights = weightsArray;
		weights.weightCount = XR_FACE_EXPRESSION2_COUNT_FB;
		weights.confidences = confidencesArray;
		weights.confidenceCount = XR_FACE_CONFIDENCE2_COUNT_FB;

		bool useVisemes = IsFaceTrackingVisemesSupported() && IsFaceTrackingVisemesEnabled();
		XrFaceTrackingVisemesMETAX1 faceTrackingVisemes{ XR_TYPE_FACE_TRACKING_VISEMES_METAX1 };
		if (useVisemes)
		{
			weights.next = &faceTrackingVisemes;
		}

		auto result = XRMovement::xrGetFaceExpressionWeights2FB(FaceTracker, &info, &weights);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[FaceExpressionStateUpdate] Failed to get face tracking state. Result(%d)"), result);
			return;
		}

		CachedFaceState.bIsValid = (weights.isValid == XR_TRUE);
		CachedFaceState.bIsEyeFollowingBlendshapesValid = (weights.isEyeFollowingBlendshapesValid == XR_TRUE);
		CachedFaceState.Time = OculusXR::FromXrTime(weights.time);

		switch (weights.dataSource)
		{
			case XR_FACE_TRACKING_DATA_SOURCE2_AUDIO_FB:
				CachedFaceState.DataSource = EFaceTrackingDataSource::Audio;
				break;
			case XR_FACE_TRACKING_DATA_SOURCE2_VISUAL_FB:
				CachedFaceState.DataSource = EFaceTrackingDataSource::Visual;
				break;
			case XR_FACE_TRACKING_DATA_SOURCE_2FB_MAX_ENUM_FB:
				CachedFaceState.DataSource = EFaceTrackingDataSource::MAX;
				break;
		}

		FMemory::Memcpy(CachedFaceState.ExpressionWeights.GetData(), weights.weights, XR_FACE_EXPRESSION2_COUNT_FB);
		FMemory::Memcpy(CachedFaceState.ExpressionWeightConfidences.GetData(), weights.confidences, XR_FACE_CONFIDENCE2_COUNT_FB);

		if (useVisemes)
		{
			CachedVisemeState.bIsValid = (faceTrackingVisemes.isValid == XR_TRUE);
			CachedVisemeState.Time = CachedFaceState.Time;
			FMemory::Memcpy(CachedVisemeState.ExpressionVisemeWeights.GetData(), faceTrackingVisemes.visemes, XR_FACE_TRACKING_VISEME_COUNT_METAX1);
		}
	}

} // namespace XRMovement

#undef LOCTEXT_NAMESPACE
