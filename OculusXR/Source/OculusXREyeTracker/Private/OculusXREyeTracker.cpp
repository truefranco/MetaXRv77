// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "IEyeTrackerModule.h"
#include "EyeTrackerTypes.h"
#include "IEyeTracker.h"
#include "Modules/ModuleManager.h"

#include "GameFramework/WorldSettings.h"
#include "Engine/World.h"
#include "IXRTrackingSystem.h"
#include "Engine/Engine.h"

#include "IOculusXRHMDModule.h"
#include "OculusXRMovement.h"
#include "OculusXRTelemetryEyeTrackerEvents.h"

#if OCULUS_HMD_SUPPORTED_PLATFORMS

namespace OculusXRHMD
{
	class FOculusXREyeTracker : public IEyeTracker
	{
	public:
		FOculusXREyeTracker()
		{
			if (IsValid(GWorld))
			{
				const auto* WorldSettings = GWorld->GetWorldSettings();
				if (IsValid(WorldSettings))
				{
					WorldToMeters = WorldSettings->WorldToMeters;
				}
			}

			if (GEngine != nullptr)
			{
				TrackingSystem = GEngine->XRSystem.Get();
			}

			OculusXRTelemetry::TScopedMarker<OculusXRTelemetry::Events::FMovementSDKEyeTrackerCreated>();
		}

		virtual ~FOculusXREyeTracker()
		{
			if (bIsTrackerStarted)
			{
				ensureMsgf(OculusXRMovement::StopEyeTracking(), TEXT("Cannot stop eye tracker."));
			}
		}

	private:
		// IEyeTracker
		virtual void SetEyeTrackedPlayer(APlayerController*) override
		{
			unimplemented();
		}

		virtual bool GetEyeTrackerGazeData(FEyeTrackerGazeData& OutGazeData) const override
		{
			return ReactOnEyeTrackerState([this, &OutGazeData](const FOculusXREyeGazesState& EyeGazeState, const FTransform& TrackingToWorld) {
				OutGazeData.FixationPoint = GetFixationPoint(EyeGazeState);
				OutGazeData.ConfidenceValue = MergeConfidence(EyeGazeState);

				OutGazeData.GazeDirection = TrackingToWorld.TransformVector(MergeOrientation(EyeGazeState).GetForwardVector());
				OutGazeData.GazeOrigin = TrackingToWorld.TransformPosition(MergePosition(EyeGazeState) * WorldToMeters);
			});
		}

		virtual bool GetEyeTrackerStereoGazeData(FEyeTrackerStereoGazeData& OutGazeData) const override
		{
			return ReactOnEyeTrackerState([this, &OutGazeData](const FOculusXREyeGazesState& EyeGazeState, const FTransform& TrackingToWorld) {
				OutGazeData.FixationPoint = GetFixationPoint(EyeGazeState);
				OutGazeData.ConfidenceValue = MergeConfidence(EyeGazeState);

				const FOculusXREyeGazeState& LeftGaze = EyeGazeState.EyeGazes[static_cast<int32>(EOculusXREye::Left)];
				const FOculusXREyeGazeState& RightGaze = EyeGazeState.EyeGazes[static_cast<int32>(EOculusXREye::Right)];
				OutGazeData.LeftEyeDirection = TrackingToWorld.TransformVector(LeftGaze.Orientation.Vector()); // Equivalent to .Quaternion().GetForwardVector()
				OutGazeData.RightEyeDirection = TrackingToWorld.TransformVector(RightGaze.Orientation.Vector());
				OutGazeData.LeftEyeOrigin = TrackingToWorld.TransformPosition(LeftGaze.Position * WorldToMeters);
				OutGazeData.RightEyeOrigin = TrackingToWorld.TransformPosition(RightGaze.Position * WorldToMeters);
			});
		}

		virtual EEyeTrackerStatus GetEyeTrackerStatus() const override
		{
			bool supported = OculusXRMovement::IsEyeTrackingSupported();
			bool enabled = OculusXRMovement::IsEyeTrackingEnabled();

			if (supported && enabled)
			{
				return EEyeTrackerStatus::Tracking;
			}
			else if (supported)
			{
				return EEyeTrackerStatus::NotTracking;
			}

			return EEyeTrackerStatus::NotConnected;
		}

		virtual bool IsStereoGazeDataAvailable() const override
		{
			return true;
		}

	private:
		// FOculusXREyeTracker
		template <typename ReactOnState>
		bool ReactOnEyeTrackerState(ReactOnState&& React) const
		{
			if (!bIsTrackerStarted)
			{
				bIsTrackerStarted = OculusXRMovement::StartEyeTracking();
			}

			if (bIsTrackerStarted)
			{
				FOculusXREyeGazesState eyeGazes;
				bool getStateResult = OculusXRMovement::GetEyeGazesState(eyeGazes, WorldToMeters);
				if (getStateResult && IsStateValidForBothEyes(eyeGazes))
				{
					FTransform TrackingToWorld = TrackingSystem ? TrackingSystem->GetTrackingToWorldTransform() : FTransform::Identity;
					React(eyeGazes, TrackingToWorld);

					return true;
				}
			}

			return false;
		}

		static float IsStateValidForBothEyes(const FOculusXREyeGazesState& EyeGazes)
		{
			return EyeGazes.EyeGazes[static_cast<int32>(EOculusXREye::Left)].bIsValid && EyeGazes.EyeGazes[static_cast<int32>(EOculusXREye::Right)].bIsValid;
		}

		static float MergeConfidence(const FOculusXREyeGazesState& EyeGazes)
		{
			const auto& LeftEyeConfidence = EyeGazes.EyeGazes[static_cast<int32>(EOculusXREye::Left)].Confidence;
			const auto& RightEyeConfidence = EyeGazes.EyeGazes[static_cast<int32>(EOculusXREye::Right)].Confidence;
			return FMath::Min(LeftEyeConfidence, RightEyeConfidence);
		}

		/// Warn: The result of MergedOrientation is not normalized.
		static FQuat MergeOrientation(const FOculusXREyeGazesState& EyeGazes)
		{
			const auto& LeftEyeOrientation = EyeGazes.EyeGazes[static_cast<int32>(EOculusXREye::Left)].Orientation;
			const auto& RightEyeOrientation = EyeGazes.EyeGazes[static_cast<int32>(EOculusXREye::Right)].Orientation;
			return FQuat::FastLerp(LeftEyeOrientation.Quaternion(), RightEyeOrientation.Quaternion(), 0.5f);
		}

		static FVector MergePosition(const FOculusXREyeGazesState& EyeGazes)
		{
			const auto& LeftEyePosition = EyeGazes.EyeGazes[static_cast<int32>(EOculusXREye::Left)].Position;
			const auto& RightEyePosition = EyeGazes.EyeGazes[static_cast<int32>(EOculusXREye::Right)].Position;
			return (LeftEyePosition + RightEyePosition) / 2.f;
		}

		static FVector GetFixationPoint(const FOculusXREyeGazesState& EyeGazes)
		{
			return FVector::ZeroVector; // Not supported
		}

		float WorldToMeters = 100.f;
		IXRTrackingSystem* TrackingSystem = nullptr;
		mutable bool bIsTrackerStarted = false;
	};
} // namespace OculusXRHMD
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS

class FOculusXREyeTrackerModule : public IEyeTrackerModule
{
public:
	static inline FOculusXREyeTrackerModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOculusXREyeTrackerModule>("OculusXREyeTracker");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("OculusXREyeTracker");
	}

	virtual FString GetModuleKeyName() const override
	{
		return TEXT("OculusXREyeTracker");
	}

	virtual bool IsEyeTrackerConnected() const override
	{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
		return GEngine->XRSystem.IsValid() && OculusXRMovement::IsEyeTrackingSupported();
#else
		return false;
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
	}

	virtual TSharedPtr<class IEyeTracker, ESPMode::ThreadSafe> CreateEyeTracker() override
	{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
		return MakeShared<OculusXRHMD::FOculusXREyeTracker>();
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
		return TSharedPtr<class IEyeTracker, ESPMode::ThreadSafe>();
	}
};

IMPLEMENT_MODULE(FOculusXREyeTrackerModule, OculusXREyeTracker)
