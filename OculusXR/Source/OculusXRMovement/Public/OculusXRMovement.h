// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRMovementTypes.h"
#include "OculusXRMovementFunctions.h"

struct OCULUSXRMOVEMENT_API OculusXRMovement
{
public:
	static bool GetBodyState(FOculusXRBodyState& outOculusXRBodyState, float WorldToMeters = 100.0f);
	static bool IsBodyTrackingEnabled();
	static bool IsBodyTrackingSupported();
	static bool StartBodyTracking();
	static bool StopBodyTracking();
	static bool StartBodyTrackingByJointSet(EOculusXRBodyJointSet jointSet);
	static bool RequestBodyTrackingFidelity(EOculusXRBodyTrackingFidelity fidelity);
	static bool ResetBodyTrackingCalibration();
	static bool SuggestBodyTrackingCalibrationOverride(float height);
	static bool GetBodySkeleton(FOculusXRBodySkeleton& outOculusXRBodyState, float WorldToMeters = 100.0f);

	static bool GetFaceState(FOculusXRFaceState& outOculusXRFaceState);
	static bool IsFaceTrackingEnabled();
	static bool IsFaceTrackingSupported();
	static bool StartFaceTracking();
	static bool StopFaceTracking();

	static bool IsFaceTrackingVisemesEnabled();
	static bool IsFaceTrackingVisemesSupported();
	static bool SetFaceTrackingVisemesEnabled(bool enabled);
	static bool GetFaceVisemesState(FOculusXRFaceVisemesState& outOculusXRFaceVisemesState);

	static bool GetEyeGazesState(FOculusXREyeGazesState& outOculusXREyeGazesState, float WorldToMeters = 100.0f);
	static bool IsEyeTrackingEnabled();
	static bool IsEyeTrackingSupported();
	static bool StartEyeTracking();
	static bool StopEyeTracking();

	static bool IsFullBodyTrackingEnabled();

private:
	static TSharedPtr<IOculusXRMovementFunctions> GetOculusXRMovementFunctionsImpl();
	static TSharedPtr<IOculusXRMovementFunctions> MovementFunctionsImpl;
};
