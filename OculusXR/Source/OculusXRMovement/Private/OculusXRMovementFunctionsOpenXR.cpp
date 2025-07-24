// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRMovementFunctionsOpenXR.h"
#include "OculusXRMovementLog.h"
#include "OculusXRMovementModule.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRHMD.h"
#include "OpenXRHMD.h"
#include "Logging/MessageLog.h"
#include "OpenXR/OculusXROpenXRUtilities.h"
#include "openxr/OculusXRBodyTrackingXR.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

bool FOculusXRMovementFunctionsOpenXR::GetBodyState(FOculusXRBodyState& outState, float WorldToMeters)
{
	auto result = FOculusXRMovementModule::Get().GetXrBodyTracker()->GetCachedBodyState(outState);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::GetBodySkeleton(FOculusXRBodySkeleton& outSkeleton, float WorldToMeters)
{
	auto result = FOculusXRMovementModule::Get().GetXrBodyTracker()->GetBodySkeleton(outSkeleton);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::IsBodyTrackingEnabled()
{
	return FOculusXRMovementModule::Get().GetXrBodyTracker()->IsBodyTrackingEnabled();
}

bool FOculusXRMovementFunctionsOpenXR::IsBodyTrackingSupported()
{
	return FOculusXRMovementModule::Get().GetXrBodyTracker()->IsBodyTrackingSupported();
}

bool FOculusXRMovementFunctionsOpenXR::IsFullBodyTrackingEnabled()
{
	return FOculusXRMovementModule::Get().GetXrBodyTracker()->IsFullBodyTrackingEnabled();
}

bool FOculusXRMovementFunctionsOpenXR::StartBodyTracking()
{
	auto result = FOculusXRMovementModule::Get().GetXrBodyTracker()->StartBodyTracking();
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::StartBodyTrackingByJointSet(EOculusXRBodyJointSet jointSet)
{
	auto result = FOculusXRMovementModule::Get().GetXrBodyTracker()->StartBodyTrackingByJointSet(jointSet);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::StopBodyTracking()
{
	auto result = FOculusXRMovementModule::Get().GetXrBodyTracker()->StopBodyTracking();
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::RequestBodyTrackingFidelity(EOculusXRBodyTrackingFidelity fidelity)
{
	auto result = FOculusXRMovementModule::Get().GetXrBodyTracker()->RequestBodyTrackingFidelity(fidelity);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::ResetBodyTrackingCalibration()
{
	auto result = FOculusXRMovementModule::Get().GetXrBodyTracker()->ResetBodyTrackingFidelity();
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::SuggestBodyTrackingCalibrationOverride(float height)
{
	auto result = FOculusXRMovementModule::Get().GetXrBodyTracker()->SuggestBodyTrackingCalibrationOverride(height);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::GetFaceState(FOculusXRFaceState& outOculusXRFaceState)
{
	auto result = FOculusXRMovementModule::Get().GetXrFaceTracker()->GetCachedFaceState(outOculusXRFaceState);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::IsFaceTrackingEnabled()
{
	return FOculusXRMovementModule::Get().GetXrFaceTracker()->IsFaceTrackingEnabled();
}

bool FOculusXRMovementFunctionsOpenXR::IsFaceTrackingSupported()
{
	return FOculusXRMovementModule::Get().GetXrFaceTracker()->IsFaceTrackingSupported();
}

bool FOculusXRMovementFunctionsOpenXR::StartFaceTracking()
{
	auto result = FOculusXRMovementModule::Get().GetXrFaceTracker()->StartFaceTracking();
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::StopFaceTracking()
{
	auto result = FOculusXRMovementModule::Get().GetXrFaceTracker()->StopFaceTracking();
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::SetFaceTrackingVisemesEnabled(bool enabled)
{
	auto result = FOculusXRMovementModule::Get().GetXrFaceTracker()->SetVisemesEnabled(enabled);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::GetFaceVisemesState(FOculusXRFaceVisemesState& outOculusXRFaceVisemesState)
{
	auto result = FOculusXRMovementModule::Get().GetXrFaceTracker()->GetCachedVisemeState(outOculusXRFaceVisemesState);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::IsFaceTrackingVisemesEnabled()
{
	return FOculusXRMovementModule::Get().GetXrFaceTracker()->IsFaceTrackingVisemesEnabled();
}

bool FOculusXRMovementFunctionsOpenXR::IsFaceTrackingVisemesSupported()
{
	return FOculusXRMovementModule::Get().GetXrFaceTracker()->IsFaceTrackingVisemesSupported();
}

bool FOculusXRMovementFunctionsOpenXR::GetEyeGazesState(FOculusXREyeGazesState& outOculusXREyeGazesState, float WorldToMeters)
{
	auto result = FOculusXRMovementModule::Get().GetXrEyeTracker()->GetCachedEyeState(outOculusXREyeGazesState);
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::IsEyeTrackingEnabled()
{
	return FOculusXRMovementModule::Get().GetXrEyeTracker()->IsEyeTrackingEnabled();
}

bool FOculusXRMovementFunctionsOpenXR::IsEyeTrackingSupported()
{
	return FOculusXRMovementModule::Get().GetXrEyeTracker()->IsEyeTrackingSupported();
}

bool FOculusXRMovementFunctionsOpenXR::StartEyeTracking()
{
	auto result = FOculusXRMovementModule::Get().GetXrEyeTracker()->StartEyeTracking();
	return XR_SUCCEEDED(result);
}

bool FOculusXRMovementFunctionsOpenXR::StopEyeTracking()
{
	auto result = FOculusXRMovementModule::Get().GetXrEyeTracker()->StopEyeTracking();
	return XR_SUCCEEDED(result);
}

#undef LOCTEXT_NAMESPACE
