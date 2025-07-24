// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRMovementTypes.h"

class OCULUSXRMOVEMENT_API IOculusXRMovementFunctions
{
public:
	virtual bool GetBodyState(FOculusXRBodyState& outOculusXRBodyState, float WorldToMeters) = 0;
	virtual bool IsBodyTrackingEnabled() = 0;
	virtual bool IsBodyTrackingSupported() = 0;
	virtual bool StartBodyTracking() = 0;
	virtual bool StopBodyTracking() = 0;
	virtual bool StartBodyTrackingByJointSet(EOculusXRBodyJointSet jointSet) = 0;
	virtual bool RequestBodyTrackingFidelity(EOculusXRBodyTrackingFidelity fidelity) = 0;
	virtual bool ResetBodyTrackingCalibration() = 0;
	virtual bool SuggestBodyTrackingCalibrationOverride(float height) = 0;
	virtual bool GetBodySkeleton(FOculusXRBodySkeleton& outOculusXRBodyState, float WorldToMeters) = 0;

	virtual bool GetFaceState(FOculusXRFaceState& outOculusXRFaceState) = 0;
	virtual bool IsFaceTrackingEnabled() = 0;
	virtual bool IsFaceTrackingSupported() = 0;
	virtual bool StartFaceTracking() = 0;
	virtual bool StopFaceTracking() = 0;

	virtual bool SetFaceTrackingVisemesEnabled(bool enabled) = 0;
	virtual bool GetFaceVisemesState(FOculusXRFaceVisemesState& outOculusXRFaceVisemesState) = 0;
	virtual bool IsFaceTrackingVisemesEnabled() = 0;
	virtual bool IsFaceTrackingVisemesSupported() = 0;

	virtual bool GetEyeGazesState(FOculusXREyeGazesState& outOculusXREyeGazesState, float WorldToMeters) = 0;
	virtual bool IsEyeTrackingEnabled() = 0;
	virtual bool IsEyeTrackingSupported() = 0;
	virtual bool StartEyeTracking() = 0;
	virtual bool StopEyeTracking() = 0;

	virtual bool IsFullBodyTrackingEnabled() = 0;
};
