// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRMovementFunctions.h"

struct FOculusXRMovementFunctionsOpenXR : public IOculusXRMovementFunctions
{
public:
	virtual bool GetBodyState(FOculusXRBodyState& outState, float WorldToMeters) override;
	virtual bool GetBodySkeleton(FOculusXRBodySkeleton& outSkeleton, float WorldToMeters) override;

	virtual bool IsBodyTrackingSupported() override;
	virtual bool IsBodyTrackingEnabled() override;
	// virtual bool IsFullBodyTrackingSupported() override;
	virtual bool IsFullBodyTrackingEnabled() override;

	virtual bool StartBodyTracking() override;
	virtual bool StartBodyTrackingByJointSet(EOculusXRBodyJointSet jointSet) override;
	virtual bool StopBodyTracking() override;

	virtual bool RequestBodyTrackingFidelity(EOculusXRBodyTrackingFidelity fidelity) override;
	virtual bool ResetBodyTrackingCalibration() override;
	virtual bool SuggestBodyTrackingCalibrationOverride(float height) override;

	virtual bool GetFaceState(FOculusXRFaceState& outOculusXRFaceState) override;
	virtual bool IsFaceTrackingEnabled() override;
	virtual bool IsFaceTrackingSupported() override;
	virtual bool StartFaceTracking() override;
	virtual bool StopFaceTracking() override;

	virtual bool SetFaceTrackingVisemesEnabled(bool enabled) override;
	virtual bool GetFaceVisemesState(FOculusXRFaceVisemesState& outOculusXRFaceVisemesState) override;
	virtual bool IsFaceTrackingVisemesEnabled() override;
	virtual bool IsFaceTrackingVisemesSupported() override;

	virtual bool GetEyeGazesState(FOculusXREyeGazesState& outOculusXREyeGazesState, float WorldToMeters) override;
	virtual bool IsEyeTrackingEnabled() override;
	virtual bool IsEyeTrackingSupported() override;
	virtual bool StartEyeTracking() override;
	virtual bool StopEyeTracking() override;
};
