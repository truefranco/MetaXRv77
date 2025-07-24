// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "OculusXRInput.h"

class FOculusXRInputModule;

namespace OculusXRInput
{
	class FOculusXRInputOpenXR : public IOculusXRInputBase, public FTickableGameObject
	{
	public:
		// IOculusXRInputBase overrides
		virtual void PlayHapticEffect(UHapticFeedbackEffect_Base* HapticEffect,
			EControllerHand Hand,
			EOculusXRHandHapticsLocation Location = EOculusXRHandHapticsLocation::Hand,
			bool bAppend = false,
			float Scale = 1.f,
			bool bLoop = false) override;
		virtual void PlayAmplitudeEnvelopeHapticEffect(EControllerHand Hand, int SamplesCount, void* Samples, int SampleRate = -1) override;
		virtual void SetHapticsByValue(float Frequency, float Amplitude, EControllerHand Hand, EOculusXRHandHapticsLocation Location = EOculusXRHandHapticsLocation::Hand) override;

		virtual float GetControllerSampleRateHz(EControllerHand Hand) const override;
		virtual int GetMaxHapticDuration(EControllerHand Hand) const override;

		// FTickableGameObject overrides
		virtual void Tick(float DeltaTime) override;
		virtual ETickableTickType GetTickableTickType() const override;
		virtual TStatId GetStatId() const override;

	private:
		void SetHapticFeedbackValues(EControllerHand Hand, const FHapticFeedbackValues& Values, FOculusXRHapticsDesc* HapticsDesc);

		TSharedPtr<FActiveHapticFeedbackEffect> ActiveHapticEffect_Left;
		TSharedPtr<FActiveHapticFeedbackEffect> ActiveHapticEffect_Right;
		TSharedPtr<FOculusXRHapticsDesc> HapticsDesc_Left;
		TSharedPtr<FOculusXRHapticsDesc> HapticsDesc_Right;
	};
} // namespace OculusXRInput
