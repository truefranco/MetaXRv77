// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRInputOVR.h"

#include "Haptics/HapticFeedbackEffect_Base.h"

namespace OculusXRInput
{

	FOculusXRInput* GetOculusXRInput()
	{
		const TSharedPtr<FOculusXRInput> OculusXRInput = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
		if (!OculusXRInput)
		{
			UE_LOG(LogOcInput, Error, TEXT("Failed getting Oculus XR input."));
			return nullptr;
		}
		return OculusXRInput.Get();
	}

	float FOculusXRInputOVR::GetControllerSampleRateHz(EControllerHand Hand) const
	{
		float SampleRateHz = 0.f;
		const ovrpController OvrpController = (EControllerHand(Hand) == EControllerHand::Left) ? ovrpController_LTouch : ovrpController_RTouch;
		if (OVRP_FAILURE(FOculusXRHMDModule::GetPluginWrapper().GetControllerSampleRateHz(OvrpController, &SampleRateHz)))
		{
			UE_LOG(LogOcInput, Error, TEXT("GetControllerSampleRateHz failed."));
		}
		return SampleRateHz;
	}

	int FOculusXRInputOVR::GetMaxHapticDuration(EControllerHand Hand) const
	{
		const ovrpController OvrpController = (EControllerHand(Hand) == EControllerHand::Left) ? ovrpController_LTouch : ovrpController_RTouch;
		if (!GetOculusXRInput()->GetOvrpHapticsDesc((int32)Hand))
			return 0;

		return GetOculusXRInput()->OvrpHapticsDesc.MaximumBufferSamplesCount / GetOculusXRInput()->OvrpHapticsDesc.SampleRateHz;
	}

	void FOculusXRInputOVR::PlayHapticEffect(
		UHapticFeedbackEffect_Base* HapticEffect, EControllerHand Hand,
		EOculusXRHandHapticsLocation Location, bool bAppend, float Scale,
		bool bLoop)
	{
		if (HapticEffect)
		{
			switch (Hand)
			{
				case EControllerHand::Left:
					GetOculusXRInput()->ActiveHapticEffect_Left.Reset();
					GetOculusXRInput()->HapticsDesc_Left.Reset();
					GetOculusXRInput()->ActiveHapticEffect_Left = MakeShareable(new FActiveHapticFeedbackEffect(HapticEffect, Scale, bLoop));
					GetOculusXRInput()->HapticsDesc_Left = MakeShareable(new FOculusXRHapticsDesc(Location, bAppend));
					break;
				case EControllerHand::Right:
					GetOculusXRInput()->ActiveHapticEffect_Right.Reset();
					GetOculusXRInput()->HapticsDesc_Right.Reset();
					GetOculusXRInput()->ActiveHapticEffect_Right = MakeShareable(new FActiveHapticFeedbackEffect(HapticEffect, Scale, bLoop));
					GetOculusXRInput()->HapticsDesc_Right = MakeShareable(new FOculusXRHapticsDesc(Location, bAppend));
					break;
				default:
					UE_LOG(LogOcInput, Warning, TEXT("Invalid hand specified (%d) for haptic feedback effect %s"), (int32)Hand, *HapticEffect->GetName());
					break;
			}
		}
	}

	void FOculusXRInputOVR::PlayAmplitudeEnvelopeHapticEffect(EControllerHand Hand,
		int SamplesCount, void* Samples,
		int InSampleRate)
	{
		int TimeToSend = GetMaxHapticDuration(Hand);
		if (TimeToSend == 0)
		{
			return;
		}

		const ovrpController OvrpController = (EControllerHand(Hand) == EControllerHand::Left) ? ovrpController_LTouch : ovrpController_RTouch;
		int SampleRate = (InSampleRate > 0 ? InSampleRate : GetOculusXRInput()->OvrpHapticsDesc.SampleRateHz);
		int MaxSamplesCount = TimeToSend * SampleRate;
		if (SamplesCount > MaxSamplesCount || SamplesCount < GetOculusXRInput()->OvrpHapticsDesc.MinimumBufferSamplesCount)
		{
			UE_LOG(LogOcInput, Error, TEXT("Sample count should be between %d and %d which last %d time."),
				GetOculusXRInput()->OvrpHapticsDesc.MinimumBufferSamplesCount, MaxSamplesCount, TimeToSend);
		}
		int WantToSend = FMath::Min(SamplesCount, MaxSamplesCount);
		WantToSend = FMath::Max(WantToSend, GetOculusXRInput()->OvrpHapticsDesc.MinimumBufferSamplesCount);

		float* BufferToSend = (float*)FMemory::Malloc(WantToSend * sizeof(*BufferToSend));
		for (int i = 0; i < WantToSend; i++)
		{
			float Amplitude = ((uint8_t*)Samples)[i] / 255.0f;
			Amplitude = FMath::Min(1.0f, Amplitude);
			Amplitude = FMath::Max(0.0f, Amplitude);
			BufferToSend[i] = Amplitude;
			UE_CLOG(OVR_HAP_LOGGING, LogOcInput, Log, TEXT("amplitude, %.3f"), Amplitude);
		}

		ovrpHapticsAmplitudeEnvelopeVibration HapticsVibration;
		HapticsVibration.Duration = WantToSend / SampleRate;
		HapticsVibration.AmplitudeCount = WantToSend;
		HapticsVibration.Amplitudes = BufferToSend;

		FOculusXRHMDModule::GetPluginWrapper().SetControllerHapticsAmplitudeEnvelope(
			OvrpController,
			HapticsVibration);
		UE_CLOG(OVR_HAP_LOGGING, LogOcInput, Log, TEXT("HAEHaptics is finished: AmplitudeCount: %d, SampleRate: %d"),
			HapticsVibration.AmplitudeCount,
			SampleRate);

		if (BufferToSend)
		{
			FMemory::Free(BufferToSend);
		}
	}

	void FOculusXRInputOVR::SetHapticsByValue(float Frequency, float Amplitude, EControllerHand Hand, EOculusXRHandHapticsLocation Location)
	{
		const ovrpController OvrpController = (EControllerHand(Hand) == EControllerHand::Left) ? ovrpController_LTouch : ovrpController_RTouch;
		FOculusXRHMDModule::GetPluginWrapper().SetControllerLocalizedVibration(OvrpController, GetOculusXRInput()->GetOVRPHapticsLocation(Location), Frequency, Amplitude);
		UE_CLOG(OVR_HAP_LOGGING, LogOcInput, Log, TEXT("LocalizedVibration is finished: Location: %d, Frequency: %f, Amplitude: %f"), (int)(Location), Frequency, Amplitude);
	}

} // namespace OculusXRInput
