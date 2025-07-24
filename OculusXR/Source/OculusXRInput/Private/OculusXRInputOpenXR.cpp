// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRInputOpenXR.h"
#include "Haptics/HapticFeedbackEffect_Base.h"
#include "OculusXRInputExtensionPlugin.h"
#include "OculusXRInputModule.h"
#include "OculusXRInputXRFunctions.h"
#include "OpenXR/OculusXROpenXRUtilities.h"
#include "OpenXRCore.h"

#include <vector>

namespace OculusXRInput
{
	FOculusXRInputModule* GetInputModule()
	{
		FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
		if (!InputModule)
		{
			UE_LOG(LogOcInput, Error, TEXT("Failed getting Oculus XR input module."));
			return nullptr;
		}
		return InputModule;
	}

	bool InstanceAndSessionAreValid(
		const FOculusXRInputModule* InputModule)
	{
		if (!InputModule->GetHapticsOpenXRExtension()->GetOpenXRInstance() || !InputModule->GetHapticsOpenXRExtension()->GetOpenXRSession())
		{
			UE_LOG(LogOcInput, Error, TEXT("Failed getting OpenXR instance or session."));
			return false;
		}
		return true;
	}

	int ControllerHandToHandIndex(EControllerHand Hand)
	{
		int HandIndex = -1;
		switch (Hand)
		{
			case EControllerHand::Left:
				HandIndex = 0;
				break;
			case EControllerHand::Right:
				HandIndex = 1;
				break;
			default:
				UE_LOG(LogOcInput, Error, TEXT("No action defined for %s."), *UEnum::GetValueAsString(Hand));
		}
		return HandIndex;
	}

	XrAction LocationToXrAction(EOculusXRHandHapticsLocation Location)
	{
		const FOculusXRInputModule* InputModule = GetInputModule();

		if (Location != EOculusXRHandHapticsLocation::Hand && !InputModule->GetHapticsOpenXRExtension()->IsTouchControllerProExtensionAvailable())
		{
			UE_LOG(LogOcInput, Warning, TEXT("Touch Controller Pro extension is not available."));
			return XR_NULL_HANDLE;
		}

		switch (Location)
		{
			case EOculusXRHandHapticsLocation::Hand:
				return InputModule->GetHapticsOpenXRExtension()->GetXrHandHapticVibrationAction();
			case EOculusXRHandHapticsLocation::Thumb:
				return InputModule->GetHapticsOpenXRExtension()->GetXrThumbHapticVibrationAction();
			case EOculusXRHandHapticsLocation::Index:
				return InputModule->GetHapticsOpenXRExtension()->GetXrIndexHapticVibrationAction();
			default:
				UE_LOG(LogOcInput, Warning, TEXT("Invalid location specified (%d)"), (int32)Location);
		}
		return XR_NULL_HANDLE;
	}

	float FOculusXRInputOpenXR::GetControllerSampleRateHz(EControllerHand Hand) const
	{
		const FOculusXRInputModule* InputModule = GetInputModule();
		if (!InputModule || !InstanceAndSessionAreValid(InputModule))
		{
			return 0.f;
		}

		if (!InputModule->GetHapticsOpenXRExtension()->IsPCMExtensionAvailable())
		{
			UE_LOG(LogOcInput, Warning, TEXT("PCM extension is not available."));
			return 0.f;
		}

		const int HandIndex = ControllerHandToHandIndex(Hand);
		if (HandIndex == -1)
		{
			return 0.f;
		}

		XrHapticActionInfo HapticActionInfo = { XR_TYPE_HAPTIC_ACTION_INFO };
		HapticActionInfo.action = InputModule->GetHapticsOpenXRExtension()->GetXrHandHapticVibrationAction();
		HapticActionInfo.subactionPath = InputModule->GetHapticsOpenXRExtension()->GetXrHandsSubactionPaths()[HandIndex];
		HapticActionInfo.next = nullptr;

		XrDevicePcmSampleRateGetInfoFB DeviceSampleRate = { XR_TYPE_DEVICE_PCM_SAMPLE_RATE_GET_INFO_FB };

		const XrResult result = xrGetDeviceSampleRateFB(InputModule->GetHapticsOpenXRExtension()->GetOpenXRSession(), &HapticActionInfo, &DeviceSampleRate);

		if (XR_FAILED(result))
		{
			UE_LOG(LogOcInput, Error, TEXT("xrGetDeviceSampleRateFB failed."));
			return 0.f;
		}

		return DeviceSampleRate.sampleRate;
	}

	int FOculusXRInputOpenXR::GetMaxHapticDuration(EControllerHand Hand) const
	{
		const float SampleRate = GetControllerSampleRateHz(Hand);
		if (SampleRate == 0.f)
		{
			UE_LOG(LogOcInput, Warning, TEXT("Sample rate equals 0"));
			return 0;
		}
		return XR_MAX_HAPTIC_PCM_BUFFER_SIZE_FB / SampleRate;
	}

	void FOculusXRInputOpenXR::Tick(float DeltaTime)
	{
		if (ActiveHapticEffect_Left.IsValid())
		{
			FHapticFeedbackValues LeftHaptics;
			const bool bPlaying = ActiveHapticEffect_Left->Update(DeltaTime, LeftHaptics);
			if (!bPlaying)
			{
				ActiveHapticEffect_Left->bLoop ? HapticsDesc_Left->Restart() : HapticsDesc_Left.Reset();
				ActiveHapticEffect_Left->bLoop ? ActiveHapticEffect_Left->Restart() : ActiveHapticEffect_Left.Reset();
			}

			SetHapticFeedbackValues(EControllerHand::Left, LeftHaptics, HapticsDesc_Left.Get());
		}

		if (ActiveHapticEffect_Right.IsValid())
		{
			FHapticFeedbackValues RightHaptics;
			const bool bPlaying = ActiveHapticEffect_Right->Update(DeltaTime, RightHaptics);
			if (!bPlaying)
			{
				ActiveHapticEffect_Right->bLoop ? HapticsDesc_Right->Restart() : HapticsDesc_Right.Reset();
				ActiveHapticEffect_Right->bLoop ? ActiveHapticEffect_Right->Restart() : ActiveHapticEffect_Right.Reset();
			}

			SetHapticFeedbackValues(EControllerHand::Right, RightHaptics, HapticsDesc_Right.Get());
		}
	}

	// Tick will only get called if the object is created in FOculusXRInput::GetOculusXRInputBaseImpl(), so we do not need ETickableTickType::Conditional
	ETickableTickType FOculusXRInputOpenXR::GetTickableTickType() const
	{
		return ETickableTickType::Always;
	}

	TStatId FOculusXRInputOpenXR::GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FOculusXRInputOpenXR, STATGROUP_Tickables);
	}

	void FOculusXRInputOpenXR::SetHapticFeedbackValues(EControllerHand Hand, const FHapticFeedbackValues& Values, FOculusXRHapticsDesc* HapticsDesc)
	{
		FHapticFeedbackBuffer* const HapticBuffer = Values.HapticBuffer;
		const bool bHasBuffer = HapticBuffer && HapticBuffer->BufferLength > 0;

		// UHapticFeedbackEffect_SoundWave
		if (bHasBuffer)
		{
			const FOculusXRInputModule* InputModule = GetInputModule();
			if (!InputModule || !InstanceAndSessionAreValid(InputModule))
			{
				return;
			}
			if (!InputModule->GetHapticsOpenXRExtension()->IsPCMExtensionAvailable())
			{
				UE_LOG(LogOcInput, Warning, TEXT("PCM extension is not available."));
				return;
			}

			int SamplesToSend = 0.036f * HapticBuffer->SamplingRate; // Related to the duration that each PCM haptic batch lasts (36ms)
			if (SamplesToSend == 0 || HapticBuffer->SamplesSent == HapticBuffer->BufferLength)
			{
				return;
			}

			// Makes sure we are not overloading it
			SamplesToSend = FMath::Min(SamplesToSend, XR_MAX_HAPTIC_PCM_BUFFER_SIZE_FB);
			SamplesToSend = FMath::Min(SamplesToSend, (HapticBuffer->BufferLength - HapticBuffer->SamplesSent) / 2);

			uint32_t SamplesConsumed = 0;
			std::vector<float> PCMBuffer(SamplesToSend);
			for (int i = 0; i < SamplesToSend; i++)
			{
				const uint32 DataIndex = HapticBuffer->CurrentPtr + (i * 2);
				const int16* const RawData = reinterpret_cast<const int16*>(&HapticBuffer->RawData[DataIndex]);
				float SampleValue = (*RawData * HapticBuffer->ScaleFactor) / INT16_MAX;
				SampleValue = FMath::Min(1.0f, SampleValue);
				SampleValue = FMath::Max(-1.0f, SampleValue);
				PCMBuffer[i] = SampleValue;
			}

			XrHapticActionInfo HapticActionInfo = { XR_TYPE_HAPTIC_ACTION_INFO };
			HapticActionInfo.action = LocationToXrAction(HapticsDesc->Location);
			HapticActionInfo.subactionPath = InputModule->GetHapticsOpenXRExtension()->GetXrHandsSubactionPaths()[ControllerHandToHandIndex(Hand)];

			XrHapticPcmVibrationFB HapticPcmVibration = { XR_TYPE_HAPTIC_PCM_VIBRATION_FB };
			HapticPcmVibration.buffer = PCMBuffer.data();
			HapticPcmVibration.bufferSize = SamplesToSend;
			HapticPcmVibration.sampleRate = HapticBuffer->SamplingRate;
			HapticPcmVibration.samplesConsumed = &SamplesConsumed;
			HapticPcmVibration.append = HapticsDesc->bIsFirstCall ? HapticsDesc->bAppend : true;

			const XrResult result = xrApplyHapticFeedback(InputModule->GetHapticsOpenXRExtension()->GetOpenXRSession(), &HapticActionInfo, reinterpret_cast<XrHapticBaseHeader*>(&HapticPcmVibration));

			if (XR_FAILED(result))
			{
				UE_LOG(LogOcInput, Error, TEXT("xrApplyHapticFeedback failed for PCM haptics with result %s"), OpenXRResultToString(result));
			}

			HapticsDesc->bIsFirstCall = false;
			HapticBuffer->CurrentPtr = FMath::Min(HapticBuffer->CurrentPtr + SamplesConsumed * 2, static_cast<uint32>(HapticBuffer->BufferLength));
			HapticBuffer->SamplesSent = FMath::Min(HapticBuffer->SamplesSent + SamplesConsumed * 2, static_cast<uint32>(HapticBuffer->BufferLength));
		}
		// UHapticFeedbackEffect_Curve and UHapticFeedbackEffect_Buffer
		else
		{
			SetHapticsByValue(Values.Frequency, Values.Amplitude, Hand, HapticsDesc ? HapticsDesc->Location : EOculusXRHandHapticsLocation::Hand);
		}
	}

	void FOculusXRInputOpenXR::PlayHapticEffect(
		UHapticFeedbackEffect_Base* HapticEffect, EControllerHand Hand,
		EOculusXRHandHapticsLocation Location, bool bAppend, float Scale,
		bool bLoop)
	{
		if (!HapticEffect)
		{
			return;
		}
		switch (Hand)
		{
			case EControllerHand::Left:
				ActiveHapticEffect_Left.Reset();
				ActiveHapticEffect_Left = MakeShareable(new FActiveHapticFeedbackEffect(HapticEffect, Scale, bLoop));
				HapticsDesc_Left.Reset();
				HapticsDesc_Left = MakeShareable(new FOculusXRHapticsDesc(Location, bAppend));
				break;
			case EControllerHand::Right:
				ActiveHapticEffect_Right.Reset();
				ActiveHapticEffect_Right = MakeShareable(new FActiveHapticFeedbackEffect(HapticEffect, Scale, bLoop));
				HapticsDesc_Right.Reset();
				HapticsDesc_Right = MakeShareable(new FOculusXRHapticsDesc(Location, bAppend));
				break;
			default:
				UE_LOG(LogOcInput, Warning, TEXT("Invalid hand specified (%d) for haptic feedback effect %s"), (int32)Hand, *HapticEffect->GetName());
				break;
		}
	}

	void FOculusXRInputOpenXR::PlayAmplitudeEnvelopeHapticEffect(EControllerHand Hand,
		int SamplesCount,
		void* Samples,
		int InSampleRate)
	{
		const FOculusXRInputModule* InputModule = GetInputModule();
		if (!InputModule || !InstanceAndSessionAreValid(InputModule))
		{
			return;
		}

		if (!InputModule->GetHapticsOpenXRExtension()->IsAmplitudeEnvelopeExtensionAvailable())
		{
			UE_LOG(LogOcInput, Warning, TEXT("Amplitude Envelope extension is not available."));
			return;
		}

		const int MaxTimeToSend = GetMaxHapticDuration(Hand);
		if (MaxTimeToSend == 0)
		{
			return;
		}

		const int SampleRate = InSampleRate > 0 ? InSampleRate : GetControllerSampleRateHz(Hand);
		if (SamplesCount > XR_MAX_HAPTIC_AMPLITUDE_ENVELOPE_SAMPLES_FB || SamplesCount < 1)
		{
			UE_LOG(LogOcInput, Warning, TEXT("Sample count should be between 1 and %d which last %d seconds."), XR_MAX_HAPTIC_PCM_BUFFER_SIZE_FB, MaxTimeToSend);
		}

		const int AmplitudesCount = FMath::Min(SamplesCount, static_cast<int>(XR_MAX_HAPTIC_AMPLITUDE_ENVELOPE_SAMPLES_FB));

		std::vector<float> AmplitudesToSend(AmplitudesCount);
		for (int i = 0; i < AmplitudesCount; i++)
		{
			float Amplitude = static_cast<uint8_t*>(Samples)[i] / 255.0f;
			Amplitude = FMath::Min(1.0f, Amplitude);
			Amplitude = FMath::Max(0.0f, Amplitude);
			AmplitudesToSend[i] = Amplitude;
		}

		XrHapticActionInfo HapticActionInfo = { XR_TYPE_HAPTIC_ACTION_INFO };
		HapticActionInfo.action = InputModule->GetHapticsOpenXRExtension()->GetXrHandHapticVibrationAction();
		HapticActionInfo.subactionPath = InputModule->GetHapticsOpenXRExtension()->GetXrHandsSubactionPaths()[ControllerHandToHandIndex(Hand)];

		XrHapticAmplitudeEnvelopeVibrationFB HapticAmplitudeEnvelopeVibration = { XR_TYPE_HAPTIC_AMPLITUDE_ENVELOPE_VIBRATION_FB };
		HapticAmplitudeEnvelopeVibration.duration = OculusXR::ToXrDuration(static_cast<float>(AmplitudesCount) / SampleRate);
		HapticAmplitudeEnvelopeVibration.amplitudeCount = static_cast<uint32_t>(AmplitudesCount);
		HapticAmplitudeEnvelopeVibration.amplitudes = AmplitudesToSend.data();

		const XrResult result = xrApplyHapticFeedback(InputModule->GetHapticsOpenXRExtension()->GetOpenXRSession(), &HapticActionInfo, reinterpret_cast<XrHapticBaseHeader*>(&HapticAmplitudeEnvelopeVibration));

		if (XR_FAILED(result))
		{
			UE_LOG(LogOcInput, Error, TEXT("xrApplyHapticFeedback failed for amplitude envelope haptics with result %s"), OpenXRResultToString(result));
		}
	}

	void FOculusXRInputOpenXR::SetHapticsByValue(float Frequency, float Amplitude, EControllerHand Hand, EOculusXRHandHapticsLocation Location)
	{
		const FOculusXRInputModule* InputModule = GetInputModule();
		if (!InputModule || !InstanceAndSessionAreValid(InputModule))
		{
			return;
		}

		const int HandIndex = ControllerHandToHandIndex(Hand);
		if (HandIndex == -1)
		{
			return;
		}

		XrHapticActionInfo HapticActionInfo = { XR_TYPE_HAPTIC_ACTION_INFO };
		HapticActionInfo.action = LocationToXrAction(Location);
		HapticActionInfo.subactionPath = InputModule->GetHapticsOpenXRExtension()->GetXrHandsSubactionPaths()[ControllerHandToHandIndex(Hand)];

		XrHapticVibration Vibration = { XR_TYPE_HAPTIC_VIBRATION };
		Vibration.amplitude = Amplitude;
		Vibration.frequency = Frequency;
		Vibration.duration = 2000000000; // 2 second duration, this is to give enough
										 // time for a new signal to be received without
										 // stopping the previous vibration

		const XrResult Result = xrApplyHapticFeedback(InputModule->GetHapticsOpenXRExtension()->GetOpenXRSession(), &HapticActionInfo, reinterpret_cast<XrHapticBaseHeader*>(&Vibration));

		if (XR_FAILED(Result))
		{
			UE_LOG(LogOcInput, Error, TEXT("xrApplyHapticFeedback failed with result %s"), OpenXRResultToString(Result));
		}
	}

} // namespace OculusXRInput
