// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRFunctionLibraryOpenXR.h"

#include "OpenXR\OculusXRXRFunctions.h"
#include "OculusXRHMD.h"
#include "IOpenXRHMD.h"
#include "OpenXR\OculusXROpenXRUtilities.h"
#include "IOpenXRHMD.h"
#include "IOpenXRHMDModule.h"
#include "OpenXRBlueprintFunctionLibrary.h"

namespace
{
	struct HeadModelParms
	{
		float HeadModelDepth;
		float HeadModelHeight;
	};

	// CompositorOpenXR Constants
	const HeadModelParms DefaultHeadModel = {
		0.0805f,
		0.0750f,
	};

	FVector2D GetUserNeckEyeDistance()
	{
		return FVector2D(DefaultHeadModel.HeadModelDepth, DefaultHeadModel.HeadModelHeight);
	}
} // namespace

#define NOT_IMPLEMENTED() \
	UE_LOG(LogHMD, Log, TEXT("%s Not Implemented In OpenXR"), StringCast<TCHAR>(__FUNCTION__).Get());

namespace OculusXRHMD
{
	void FOculusXRFunctionLibraryOpenXR::GetPose(FRotator& DeviceRotation, FVector& DevicePosition, FVector& NeckPosition)
	{
		FQuat HeadOrientation = FQuat::Identity;
		FVector HeadPosition = FVector::ZeroVector;

		GEngine->XRSystem->GetCurrentPose(IXRTrackingSystem::HMDDeviceId, HeadOrientation, HeadPosition);

		FVector LocalNeckPosition = HeadOrientation.Inverse().RotateVector(HeadPosition);
		const FVector2D NeckEyeDistance = GetUserNeckEyeDistance();

		const float WorldToMetersScale = GEngine->XRSystem->GetWorldToMetersScale();
		LocalNeckPosition.X -= NeckEyeDistance.X * WorldToMetersScale;
		LocalNeckPosition.Z -= NeckEyeDistance.Y * WorldToMetersScale;

		DeviceRotation = HeadOrientation.Rotator();
		DevicePosition = HeadPosition;
		NeckPosition = LocalNeckPosition;
	}

	void FOculusXRFunctionLibraryOpenXR::SetBaseRotationAndBaseOffsetInMeters(FRotator Rotation, FVector BaseOffsetInMeters, EOrientPositionSelector::Type Options)
	{
		if (GEngine->XRSystem.IsValid())
		{
			if ((Options == EOrientPositionSelector::Orientation) || (Options == EOrientPositionSelector::OrientationAndPosition))
			{
				GEngine->XRSystem->SetBaseRotation(Rotation);
			}
			if ((Options == EOrientPositionSelector::Position) || (Options == EOrientPositionSelector::OrientationAndPosition))
			{
				GEngine->XRSystem->SetBasePosition(BaseOffsetInMeters);
			}
		}
	}

	void FOculusXRFunctionLibraryOpenXR::GetBaseRotationAndBaseOffsetInMeters(FRotator& OutRotation, FVector& OutBaseOffsetInMeters)
	{
		if (GEngine->XRSystem.IsValid())
		{
			OutRotation = GEngine->XRSystem->GetBaseRotation();
			OutBaseOffsetInMeters = GEngine->XRSystem->GetBasePosition();
		}
		else
		{
			OutRotation = FRotator::ZeroRotator;
			OutBaseOffsetInMeters = FVector::ZeroVector;
		}
	}

	void FOculusXRFunctionLibraryOpenXR::GetRawSensorData(FVector& AngularAcceleration, FVector& LinearAcceleration, FVector& AngularVelocity, FVector& LinearVelocity, float& TimeInSeconds, EOculusXRTrackedDeviceType DeviceType)
	{
		NOT_IMPLEMENTED();
	}

	static IMotionController* GetMotionController()
	{
		TArray<IMotionController*> MotionControllers;
		MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
		for (auto MotionController : MotionControllers)
		{
			FName MotionControllerName("OpenXR");
			if (MotionController != nullptr && MotionController->GetMotionControllerDeviceTypeName() == MotionControllerName)
			{
				return MotionController;
			}
		}
		return nullptr;
	}

	static bool IsControllerTracked(FName MotionSource)
	{
		IMotionController* MotionController = GetMotionController();
		if (MotionController != nullptr)
		{
			const int32_t ControllerIndex = 0;
			return MotionController->GetControllerTrackingStatus(ControllerIndex, MotionSource) == ETrackingStatus::Tracked;
		}
		return false;
	}

	bool FOculusXRFunctionLibraryOpenXR::IsDeviceTracked(EOculusXRTrackedDeviceType DeviceType)
	{
		bool bIsDeviceTracked = false;
		if (GEngine->XRSystem.IsValid())
		{
			switch (DeviceType)
			{
				case EOculusXRTrackedDeviceType::None:
					break;
				case EOculusXRTrackedDeviceType::HMD:
					if (GEngine->XRSystem.IsValid())
					{
						bIsDeviceTracked = GEngine->XRSystem->IsTracking(IXRTrackingSystem::HMDDeviceId);
					}
					break;
				case EOculusXRTrackedDeviceType::LTouch:
				{
					bIsDeviceTracked = IsControllerTracked(FName("LeftAim"));
				}
				break;
				case EOculusXRTrackedDeviceType::RTouch:
				{
					bIsDeviceTracked = IsControllerTracked(FName("RightAim"));
				}
				break;
				default:
					break;
			}
		}
		return bIsDeviceTracked;
	}

	void FOculusXRFunctionLibraryOpenXR::GetSuggestedCpuAndGpuPerformanceLevels(EOculusXRProcessorPerformanceLevel& CpuPerfLevel, EOculusXRProcessorPerformanceLevel& GpuPerfLevel)
	{
		OculusXR::FPerformanceExtensionPlugin& PerfPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetPerformanceExtensionPlugin();
		PerfPlugin.GetSuggestedCpuAndGpuPerformanceLevels(CpuPerfLevel, GpuPerfLevel);
	}

	void FOculusXRFunctionLibraryOpenXR::SetSuggestedCpuAndGpuPerformanceLevels(EOculusXRProcessorPerformanceLevel CpuPerfLevel, EOculusXRProcessorPerformanceLevel GpuPerfLevel)
	{
		OculusXR::FPerformanceExtensionPlugin& PerfPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetPerformanceExtensionPlugin();
		PerfPlugin.SetSuggestedCpuAndGpuPerformanceLevels(CpuPerfLevel, GpuPerfLevel);
	}

	bool FOculusXRFunctionLibraryOpenXR::GetUserProfile(FOculusXRHmdUserProfile& Profile)
	{
		IXRTrackingSystem* TrackingSystem = OculusXR::GetOpenXRTrackingSystem();
		if (TrackingSystem != nullptr)
		{
			const IHeadMountedDisplay* Hmd = TrackingSystem->GetHMDDevice();
			if (Hmd != nullptr)
			{
				const float IPD = Hmd->GetInterpupillaryDistance();
				Profile.Name = "";
				Profile.Gender = "Unknown";
				Profile.PlayerHeight = 0.0f;
				Profile.EyeHeight = DefaultHeadModel.HeadModelHeight;
				Profile.IPD = IPD;
				Profile.NeckToEyeDistance = FVector2D(DefaultHeadModel.HeadModelDepth, 0.0f);
			}
			return true;
		}
		return false;
	}

	void FOculusXRFunctionLibraryOpenXR::SetBaseRotationAndPositionOffset(FRotator BaseRot, FVector PosOffset, EOrientPositionSelector::Type Options)
	{
		NOT_IMPLEMENTED();
	}

	void FOculusXRFunctionLibraryOpenXR::GetBaseRotationAndPositionOffset(FRotator& OutRot, FVector& OutPosOffset)
	{
		NOT_IMPLEMENTED();
	}

	void FOculusXRFunctionLibraryOpenXR::AddLoadingSplashScreen(class UTexture2D* Texture, FVector TranslationInMeters, FRotator Rotation, FVector2D SizeInMeters, FRotator DeltaRotation, bool bClearBeforeAdd)
	{
		NOT_IMPLEMENTED();
	}

	void FOculusXRFunctionLibraryOpenXR::ClearLoadingSplashScreens()
	{
		NOT_IMPLEMENTED();
	}

	bool FOculusXRFunctionLibraryOpenXR::HasInputFocus()
	{
		IXRTrackingSystem* TrackingSystem = OculusXR::GetOpenXRTrackingSystem();
		if (TrackingSystem != nullptr)
		{
			IOpenXRHMD* OpenXRHMD = TrackingSystem->GetIOpenXRHMD();
			if (OpenXRHMD != nullptr)
			{
				return OpenXRHMD->IsFocused();
			}
		}
		return false;
	}

	bool FOculusXRFunctionLibraryOpenXR::HasSystemOverlayPresent()
	{
		IXRTrackingSystem* TrackingSystem = OculusXR::GetOpenXRTrackingSystem();
		if (TrackingSystem != nullptr)
		{
			IOpenXRHMD* OpenXRHMD = TrackingSystem->GetIOpenXRHMD();
			if (OpenXRHMD != nullptr)
			{
				return !OpenXRHMD->IsFocused();
			}
		}
		return false;
	}

	void FOculusXRFunctionLibraryOpenXR::GetGPUUtilization(bool& IsGPUAvailable, float& GPUUtilization)
	{
		OculusXR::FPerformanceExtensionPlugin& PerfPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetPerformanceExtensionPlugin();
		const FOculusXRPerformanceMetrics& PerformanceMetrics = PerfPlugin.GetPerformanceMetrics();
		if (PerfPlugin.IsPerformanceMetricsSupported(OculusXR::GPUUtilizationFloat))
		{
			GPUUtilization = PerformanceMetrics.GpuUtil;
			IsGPUAvailable = true;
		}
		IsGPUAvailable = false;
	}

	float FOculusXRFunctionLibraryOpenXR::GetGPUFrameTime()
	{
		OculusXR::FPerformanceExtensionPlugin& PerfPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetPerformanceExtensionPlugin();
		const FOculusXRPerformanceMetrics& PerformanceMetrics = PerfPlugin.GetPerformanceMetrics();
		return PerformanceMetrics.AppGpuTime;
	}

	void FOculusXRFunctionLibraryOpenXR::GetPerformanceMetrics(FOculusXRPerformanceMetrics& PerformanceMetrics)
	{
		OculusXR::FPerformanceExtensionPlugin& PerfPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetPerformanceExtensionPlugin();
		PerformanceMetrics = PerfPlugin.GetPerformanceMetrics();
	}


	EOculusXRFoveatedRenderingMethod FOculusXRFunctionLibraryOpenXR::GetFoveatedRenderingMethod()
	{
		NOT_IMPLEMENTED();
		return EOculusXRFoveatedRenderingMethod::FixedFoveatedRendering;
	}

	void FOculusXRFunctionLibraryOpenXR::SetFoveatedRenderingMethod(EOculusXRFoveatedRenderingMethod Method)
	{
		NOT_IMPLEMENTED();
	}

	void FOculusXRFunctionLibraryOpenXR::SetFoveatedRenderingLevel(EOculusXRFoveatedRenderingLevel level, bool isDynamic)
	{
		if (IConsoleVariable* FoveationLevelCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("xr.OpenXRFBFoveationLevel")))
		{
			FoveationLevelCVar->Set(static_cast<int>(level));
		}
		if (IConsoleVariable* FoveationDynamicCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("xr.OpenXRFBFoveationDynamic")))
		{
			FoveationDynamicCVar->Set(isDynamic);
		}
	}

	EOculusXRFoveatedRenderingLevel FOculusXRFunctionLibraryOpenXR::GetFoveatedRenderingLevel()
	{
		const auto FoveationLevelCVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("xr.OpenXRFBFoveationLevel"));

		return FoveationLevelCVar ? static_cast<EOculusXRFoveatedRenderingLevel>(FoveationLevelCVar->GetValueOnAnyThread()) : EOculusXRFoveatedRenderingLevel::Off;
	}

	bool FOculusXRFunctionLibraryOpenXR::GetEyeTrackedFoveatedRenderingSupported()
	{
		return false;
	}

	FString FOculusXRFunctionLibraryOpenXR::GetDeviceName()
	{
		// deprecated
		return FString();
	}

	EOculusXRDeviceType FOculusXRFunctionLibraryOpenXR::GetDeviceType()
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.GetDeviceType();
	}

	EOculusXRControllerType FOculusXRFunctionLibraryOpenXR::GetControllerType(EControllerHand deviceHand)
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.GetControllerType(deviceHand);
	}

	TArray<float> FOculusXRFunctionLibraryOpenXR::GetAvailableDisplayFrequencies()
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.GetSystemDisplayAvailableFrequencies();
	}

	float FOculusXRFunctionLibraryOpenXR::GetCurrentDisplayFrequency()
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.GetSystemDisplayFrequency();
	}

	void FOculusXRFunctionLibraryOpenXR::SetDisplayFrequency(float RequestedFrequency)
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.SetSystemDisplayFrequency(RequestedFrequency);
	}

	void FOculusXRFunctionLibraryOpenXR::EnablePositionTracking(bool bPositionTracking)
	{
		NOT_IMPLEMENTED();
	}

	void FOculusXRFunctionLibraryOpenXR::EnableOrientationTracking(bool bOrientationTracking)
	{
		NOT_IMPLEMENTED();
	}

	void FOculusXRFunctionLibraryOpenXR::SetColorScaleAndOffset(FLinearColor ColorScale, FLinearColor ColorOffset, bool bApplyToAllLayers)
	{
		OculusXR::FLayerExtensionPlugin& Plugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetLayerExtensionPlugin();
		Plugin.SetColorScaleAndOffset(ColorScale, ColorOffset, bApplyToAllLayers);
	}

	class IStereoLayers* FOculusXRFunctionLibraryOpenXR::GetStereoLayers()
	{
		NOT_IMPLEMENTED();
		return nullptr;
	}

	bool FOculusXRFunctionLibraryOpenXR::IsGuardianConfigured()
	{
		OculusXR::FGuardianExtensionPlugin& Plugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetGuardianExtensionPlugin();
		return Plugin.IsGuardianConfigured();
	}

	bool FOculusXRFunctionLibraryOpenXR::IsGuardianDisplayed()
	{
		// deprecated
		return false;
	}

	TArray<FVector> FOculusXRFunctionLibraryOpenXR::GetGuardianPoints(EOculusXRBoundaryType BoundaryType, bool UsePawnSpace /* = false */)
	{
		if (BoundaryType != EOculusXRBoundaryType::Boundary_PlayArea)
		{
			UE_LOG(LogHMD, Log, TEXT("GetGuardianPoints: Only Boundary_PlayArea is applicable in OpenXR"));
			return TArray<FVector>();
		}

		TArray<FVector> BoundaryPoints;
		IXRTrackingSystem* TrackingSystem = OculusXR::GetOpenXRTrackingSystem();

		if (TrackingSystem != nullptr)
		{
			OculusXR::FGuardianExtensionPlugin& Plugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetGuardianExtensionPlugin();
			Plugin.GetGuardianPoints(BoundaryPoints);
		}

		return BoundaryPoints;
	}

	FVector FOculusXRFunctionLibraryOpenXR::GetGuardianDimensions(EOculusXRBoundaryType BoundaryType)
	{
		OculusXR::FGuardianExtensionPlugin& Plugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetGuardianExtensionPlugin();
		return Plugin.GetGuardianDimensions();
	}

	FTransform FOculusXRFunctionLibraryOpenXR::GetPlayAreaTransform()
	{
		IXRTrackingSystem* TrackingSystem = OculusXR::GetOpenXRTrackingSystem();
		if (TrackingSystem != nullptr)
		{
			TArray<FVector> BoundaryPoints;
			OculusXR::FGuardianExtensionPlugin& Plugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetGuardianExtensionPlugin();
			if (Plugin.GetGuardianPoints(BoundaryPoints))
			{
				check(BoundaryPoints.Num() == 4);
				const float WorldToMetersScale = TrackingSystem->GetWorldToMetersScale();

				const FVector Edge = BoundaryPoints[1] - BoundaryPoints[0];
				const float Angle = FMath::Acos((Edge).GetSafeNormal() | FVector::RightVector);
				const FQuat Rotation(FVector::UpVector, Edge.X < 0 ? Angle : -Angle);

				const FVector Position = (BoundaryPoints[0] + BoundaryPoints[1] + BoundaryPoints[2] + BoundaryPoints[3]) / 4;
				const FVector Scale(FVector::Distance(BoundaryPoints[3], BoundaryPoints[0]) / WorldToMetersScale, FVector::Distance(BoundaryPoints[1], BoundaryPoints[0]) / WorldToMetersScale, 1.0);

				return FTransform(Rotation, Position, Scale);
			}
		}
		return FTransform();
	}

	FOculusXRGuardianTestResult FOculusXRFunctionLibraryOpenXR::GetPointGuardianIntersection(const FVector Point, EOculusXRBoundaryType BoundaryType)
	{
		// deprecated
		FOculusXRGuardianTestResult InteractionInfo;
		memset(&InteractionInfo, 0, sizeof(FOculusXRGuardianTestResult));
		return InteractionInfo;
	}

	FOculusXRGuardianTestResult FOculusXRFunctionLibraryOpenXR::GetNodeGuardianIntersection(EOculusXRTrackedDeviceType DeviceType, EOculusXRBoundaryType BoundaryType)
	{
		// deprecated
		FOculusXRGuardianTestResult InteractionInfo;
		memset(&InteractionInfo, 0, sizeof(FOculusXRGuardianTestResult));
		return InteractionInfo;
	}

	void FOculusXRFunctionLibraryOpenXR::SetGuardianVisibility(bool GuardianVisible)
	{
		// deprecated
	}

	bool FOculusXRFunctionLibraryOpenXR::GetSystemHmd3DofModeEnabled()
	{
		NOT_IMPLEMENTED();
		return false;
	}

	EOculusXRColorSpace FOculusXRFunctionLibraryOpenXR::GetHmdColorDesc()
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.GetColorSpace();
	}

	void FOculusXRFunctionLibraryOpenXR::SetClientColorDesc(EOculusXRColorSpace ColorSpace)
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		SystemPlugin.SetColorSpace(ColorSpace);
	}

	void FOculusXRFunctionLibraryOpenXR::SetLocalDimmingOn(bool LocalDimmingOn)
	{
		OculusXR::FLayerExtensionPlugin& Plugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetLayerExtensionPlugin();
		return Plugin.SetEnableLocalDimming(LocalDimmingOn);
	}

	bool FOculusXRFunctionLibraryOpenXR::IsPassthroughSupported()
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.IsPassthroughSupported();
	}

	bool FOculusXRFunctionLibraryOpenXR::IsColorPassthroughSupported()
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.IsColorPassthroughSupported();
	}

	void FOculusXRFunctionLibraryOpenXR::StartEnvironmentDepth()
	{
#ifdef WITH_OCULUS_BRANCH
		auto& EnvDepthPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetEnvironmentDepthExtensionPlugin();
		EnvDepthPlugin.StartEnvironmentDepth();
#endif
	}

	void FOculusXRFunctionLibraryOpenXR::StopEnvironmentDepth()
	{
#ifdef WITH_OCULUS_BRANCH
		auto& EnvDepthPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetEnvironmentDepthExtensionPlugin();
		EnvDepthPlugin.StopEnvironmentDepth();
#endif
	}

	bool FOculusXRFunctionLibraryOpenXR::IsEnvironmentDepthStarted()
	{
#ifdef WITH_OCULUS_BRANCH
		const auto& EnvDepthPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetEnvironmentDepthExtensionPlugin();
		return !EnvDepthPlugin.IsEnvironmentDepthStarted();
#else
		return false;
#endif
	}

	void FOculusXRFunctionLibraryOpenXR::SetEnvironmentDepthHandRemoval(bool RemoveHands)
	{
#ifdef WITH_OCULUS_BRANCH
		auto& EnvDepthPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetEnvironmentDepthExtensionPlugin();
		EnvDepthPlugin.SetEnvironmentDepthHandRemoval_RenderThread(RemoveHands);
#endif
	}

	void FOculusXRFunctionLibraryOpenXR::SetXROcclusionsMode(UObject* WorldContextObject, EOculusXROcclusionsMode Mode)
	{
#ifdef WITH_OCULUS_BRANCH
		auto& EnvDepthPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetEnvironmentDepthExtensionPlugin();
		EnvDepthPlugin.SetXROcclusionsMode(WorldContextObject, Mode);
#endif
	}

	void FOculusXRFunctionLibraryOpenXR::SetEyeBufferSharpenType(EOculusXREyeBufferSharpenType EyeBufferSharpenType)
	{
		OculusXR::FLayerExtensionPlugin& Plugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetLayerExtensionPlugin();
		Plugin.SetEyeBufferSharpenType(EyeBufferSharpenType);
	}

	bool FOculusXRFunctionLibraryOpenXR::IsPassthroughRecommended()
	{
		OculusXR::FSystemInfoExtensionPlugin& SystemPlugin = FOculusXRHMDModule::Get().GetExtensionPluginManager().GetSystemInfoExtensionPlugin();
		return SystemPlugin.IsPassthroughRecommended();
	}

	void FOculusXRFunctionLibraryOpenXR::GetOpenXRInstanceProcAddrFunc(void** func)
	{
		*func = (void*)OpenXRDynamicAPI::xrGetInstanceProcAddr;
	}

	void FOculusXRFunctionLibraryOpenXR::GetNativeOpenXRHandles(uint64_t* OutXrInstance, uint64_t* OutXrSession)
	{
		*OutXrInstance = 0;
		*OutXrSession = 0;

		IXRTrackingSystem* TrackingSystem = OculusXR::GetOpenXRTrackingSystem();
		if (TrackingSystem)
		{
			IOpenXRHMD* OpenXRHMD = TrackingSystem->GetIOpenXRHMD();
			if (OpenXRHMD)
			{
				if (OutXrInstance)
				{
					*OutXrInstance = (uint64_t)OpenXRHMD->GetInstance();
				}
				if (OutXrSession)
				{
					*OutXrSession = (uint64_t)OpenXRHMD->GetSession();
				}
			}
		}
	}

	void FOculusXRFunctionLibraryOpenXR::GetAppSpace(uint64_t* OutAppSpace)
	{
		IXRTrackingSystem* TrackingSystem = OculusXR::GetOpenXRTrackingSystem();
		if (TrackingSystem)
		{
			IOpenXRHMD* OpenXRHMD = TrackingSystem->GetIOpenXRHMD();
			if (OpenXRHMD)
			{
				*OutAppSpace = (uint64_t)OpenXRHMD->GetTrackingSpace();
			}
		}
	}

	void FOculusXRFunctionLibraryOpenXR::GetNextPredictedDisplayTime(uint64_t* NextPredictedDisplayTime)
	{
		IXRTrackingSystem* TrackingSystem = OculusXR::GetOpenXRTrackingSystem();
		if (TrackingSystem)
		{
			IOpenXRHMD* OpenXRHMD = TrackingSystem->GetIOpenXRHMD();
			if (OpenXRHMD)
			{
				*NextPredictedDisplayTime = OpenXRHMD->GetDisplayTime();
			}
		}
	}

	void FOculusXRFunctionLibraryOpenXR::RegisterOpenXrEventHandler(void (*OpenXrEventHandler)(void* data, void* context), void* Context)
	{
		ensureMsgf(false, TEXT("RegisterOpenXrEventHandler is only available with OvrPlugin"));
	}

	void FOculusXRFunctionLibraryOpenXR::UnregisterOpenXrEventHandler(void (*OpenXrEventHandler)(void* data, void* context))
	{
		ensureMsgf(false, TEXT("UnregisterOpenXrEventHandler is only available with OvrPlugin"));
	}

} // namespace OculusXRHMD
