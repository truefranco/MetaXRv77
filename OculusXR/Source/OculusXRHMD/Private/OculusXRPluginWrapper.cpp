// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRPluginWrapper.h"
#include "OculusXRHMDModule.h"

#if PLATFORM_ANDROID
#include <dlfcn.h>
#define MIN_SDK_VERSION 29
#endif

DEFINE_LOG_CATEGORY(LogOculusPluginWrapper);

static void* LoadEntryPoint(void* handle, const char* EntryPointName);

bool OculusPluginWrapper::InitializeOculusPluginWrapper(OculusPluginWrapper* wrapper)
{
	if (wrapper->IsInitialized())
	{
		UE_LOG(LogOculusPluginWrapper, Warning, TEXT("wrapper already initialized"));
		return true;
	}

#if OCULUS_HMD_SUPPORTED_PLATFORMS
	void* LibraryHandle = nullptr;

#if PLATFORM_ANDROID
	const bool VersionValid = FAndroidMisc::GetAndroidBuildVersion() >= MIN_SDK_VERSION;
#else
	const bool VersionValid = true;
#endif

	if (VersionValid)
	{
		LibraryHandle = FOculusXRHMDModule::GetOVRPluginHandle();
		if (LibraryHandle == nullptr)
		{
			UE_LOG(LogOculusPluginWrapper, Warning, TEXT("GetOVRPluginHandle() returned NULL"));
			return false;
		}
	}
	else
	{
		return false;
	}
#else
	return false;
#endif

	struct OculusEntryPoint
	{
		const char* EntryPointName;
		void** EntryPointPtr;
	};

#define OCULUS_BIND_ENTRY_POINT(Func)         \
	{                                         \
		"ovrp_" #Func, (void**)&wrapper->Func \
	}

	OculusEntryPoint entryPointArray[] = {
		// OVR_Plugin.h

		OCULUS_BIND_ENTRY_POINT(PreInitialize5),
		OCULUS_BIND_ENTRY_POINT(GetInitialized),
		OCULUS_BIND_ENTRY_POINT(Initialize7),
		OCULUS_BIND_ENTRY_POINT(Shutdown2),
		OCULUS_BIND_ENTRY_POINT(SetLogCallback2),
		OCULUS_BIND_ENTRY_POINT(GetVersion2),
		OCULUS_BIND_ENTRY_POINT(GetNativeSDKVersion2),
		OCULUS_BIND_ENTRY_POINT(GetNativeSDKPointer2),
		OCULUS_BIND_ENTRY_POINT(GetNativeOpenXRHandles),
		OCULUS_BIND_ENTRY_POINT(GetDisplayAdapterId2),
		OCULUS_BIND_ENTRY_POINT(GetAudioOutId2),
		OCULUS_BIND_ENTRY_POINT(GetAudioOutDeviceId2),
		OCULUS_BIND_ENTRY_POINT(GetAudioInId2),
		OCULUS_BIND_ENTRY_POINT(GetAudioInDeviceId2),
		OCULUS_BIND_ENTRY_POINT(GetInstanceExtensionsVk),
		OCULUS_BIND_ENTRY_POINT(GetDeviceExtensionsVk),
		OCULUS_BIND_ENTRY_POINT(SetupDistortionWindow3),
		OCULUS_BIND_ENTRY_POINT(DestroyDistortionWindow2),
		OCULUS_BIND_ENTRY_POINT(GetDominantHand),
		OCULUS_BIND_ENTRY_POINT(SetRemoteHandedness),
		OCULUS_BIND_ENTRY_POINT(SetColorScaleAndOffset),
		OCULUS_BIND_ENTRY_POINT(SetupLayer),
		OCULUS_BIND_ENTRY_POINT(SetupLayerDepth),
		OCULUS_BIND_ENTRY_POINT(SetEyeFovPremultipliedAlphaMode),
		OCULUS_BIND_ENTRY_POINT(GetEyeFovLayerId),
		OCULUS_BIND_ENTRY_POINT(GetLayerTextureStageCount),
		OCULUS_BIND_ENTRY_POINT(GetLayerTexture2),
		OCULUS_BIND_ENTRY_POINT(GetLayerTextureFoveation),
		OCULUS_BIND_ENTRY_POINT(GetLayerOcclusionMesh),
		OCULUS_BIND_ENTRY_POINT(GetLayerAndroidSurfaceObject),
		OCULUS_BIND_ENTRY_POINT(GetLayerTextureSpaceWarp),
		OCULUS_BIND_ENTRY_POINT(CalculateEyeLayerDesc3),
		OCULUS_BIND_ENTRY_POINT(DestroyLayer),
		OCULUS_BIND_ENTRY_POINT(CalculateLayerDesc),
		OCULUS_BIND_ENTRY_POINT(CalculateEyeLayerDesc2),
		OCULUS_BIND_ENTRY_POINT(CalculateEyePreviewRect),
		OCULUS_BIND_ENTRY_POINT(SetupMirrorTexture2),
		OCULUS_BIND_ENTRY_POINT(DestroyMirrorTexture2),
		OCULUS_BIND_ENTRY_POINT(GetAdaptiveGpuPerformanceScale2),
		OCULUS_BIND_ENTRY_POINT(GetAppCpuStartToGpuEndTime2),
		OCULUS_BIND_ENTRY_POINT(GetEyePixelsPerTanAngleAtCenter2),
		OCULUS_BIND_ENTRY_POINT(GetHmdToEyeOffset2),
		OCULUS_BIND_ENTRY_POINT(Update3),
		OCULUS_BIND_ENTRY_POINT(WaitToBeginFrame),
		OCULUS_BIND_ENTRY_POINT(BeginFrame4),
		OCULUS_BIND_ENTRY_POINT(UpdateFoveation),
		OCULUS_BIND_ENTRY_POINT(EndFrame4),
		OCULUS_BIND_ENTRY_POINT(GetTrackingOrientationSupported2),
		OCULUS_BIND_ENTRY_POINT(GetTrackingOrientationEnabled2),
		OCULUS_BIND_ENTRY_POINT(SetTrackingOrientationEnabled2),
		OCULUS_BIND_ENTRY_POINT(GetTrackingPositionSupported2),
		OCULUS_BIND_ENTRY_POINT(GetTrackingPositionEnabled2),
		OCULUS_BIND_ENTRY_POINT(SetTrackingPositionEnabled2),
		OCULUS_BIND_ENTRY_POINT(GetTrackingPoseEnabledForInvisibleSession),
		OCULUS_BIND_ENTRY_POINT(SetTrackingPoseEnabledForInvisibleSession),
		OCULUS_BIND_ENTRY_POINT(GetTrackingIPDEnabled2),
		OCULUS_BIND_ENTRY_POINT(SetTrackingIPDEnabled2),
		OCULUS_BIND_ENTRY_POINT(GetTrackingCalibratedOrigin2),
		OCULUS_BIND_ENTRY_POINT(SetTrackingCalibratedOrigin2),
		OCULUS_BIND_ENTRY_POINT(GetTrackingOriginType2),
		OCULUS_BIND_ENTRY_POINT(SetTrackingOriginType2),
		OCULUS_BIND_ENTRY_POINT(RecenterTrackingOrigin2),
		OCULUS_BIND_ENTRY_POINT(GetNodePresent2),
		OCULUS_BIND_ENTRY_POINT(GetNodeOrientationTracked2),
		OCULUS_BIND_ENTRY_POINT(GetNodeOrientationValid),
		OCULUS_BIND_ENTRY_POINT(GetNodePositionTracked2),
		OCULUS_BIND_ENTRY_POINT(GetNodePositionValid),
		OCULUS_BIND_ENTRY_POINT(SetNodePositionTracked2),
		OCULUS_BIND_ENTRY_POINT(GetNodePoseState3),
		OCULUS_BIND_ENTRY_POINT(GetNodePoseStateRaw),
		OCULUS_BIND_ENTRY_POINT(GetNodeFrustum2),
		OCULUS_BIND_ENTRY_POINT(SetHeadPoseModifier),
		OCULUS_BIND_ENTRY_POINT(GetHeadPoseModifier),
		OCULUS_BIND_ENTRY_POINT(GetControllerState4),
		OCULUS_BIND_ENTRY_POINT(GetControllerState5),
		OCULUS_BIND_ENTRY_POINT(GetControllerState6),
		OCULUS_BIND_ENTRY_POINT(GetActiveController2),
		OCULUS_BIND_ENTRY_POINT(GetConnectedControllers2),
		OCULUS_BIND_ENTRY_POINT(SetControllerVibration2),
		OCULUS_BIND_ENTRY_POINT(SetControllerLocalizedVibration),
		OCULUS_BIND_ENTRY_POINT(SetControllerHapticsAmplitudeEnvelope),
		OCULUS_BIND_ENTRY_POINT(SetControllerHapticsPcm),
		OCULUS_BIND_ENTRY_POINT(GetControllerHapticsDesc2),
		OCULUS_BIND_ENTRY_POINT(GetControllerHapticsState2),
		OCULUS_BIND_ENTRY_POINT(GetControllerSampleRateHz),
		OCULUS_BIND_ENTRY_POINT(SetControllerHaptics2),
		OCULUS_BIND_ENTRY_POINT(SetSuggestedCpuPerformanceLevel),
		OCULUS_BIND_ENTRY_POINT(GetSuggestedCpuPerformanceLevel),
		OCULUS_BIND_ENTRY_POINT(SetSuggestedGpuPerformanceLevel),
		OCULUS_BIND_ENTRY_POINT(GetSuggestedGpuPerformanceLevel),
		OCULUS_BIND_ENTRY_POINT(GetAppCPUPriority2),
		OCULUS_BIND_ENTRY_POINT(SetAppCPUPriority2),
		OCULUS_BIND_ENTRY_POINT(GetSystemPowerSavingMode2),
		OCULUS_BIND_ENTRY_POINT(GetSystemDisplayFrequency2),
		OCULUS_BIND_ENTRY_POINT(GetSystemDisplayAvailableFrequencies),
		OCULUS_BIND_ENTRY_POINT(SetSystemDisplayFrequency),
		OCULUS_BIND_ENTRY_POINT(GetSystemVSyncCount2),
		OCULUS_BIND_ENTRY_POINT(SetSystemVSyncCount2),
		OCULUS_BIND_ENTRY_POINT(GetSystemProductName2),
		OCULUS_BIND_ENTRY_POINT(GetSystemRegion2),
		OCULUS_BIND_ENTRY_POINT(ShowSystemUI2),
		OCULUS_BIND_ENTRY_POINT(GetAppHasVrFocus2),
		OCULUS_BIND_ENTRY_POINT(GetAppHasInputFocus),
		OCULUS_BIND_ENTRY_POINT(GetAppHasSystemOverlayPresent),
		OCULUS_BIND_ENTRY_POINT(GetAppShouldQuit2),
		OCULUS_BIND_ENTRY_POINT(GetAppShouldRecenter2),
		OCULUS_BIND_ENTRY_POINT(GetAppShouldRecreateDistortionWindow2),
		OCULUS_BIND_ENTRY_POINT(GetAppSpace),
		OCULUS_BIND_ENTRY_POINT(GetAppLatencyTimings2),
		OCULUS_BIND_ENTRY_POINT(SetAppEngineInfo2),
		OCULUS_BIND_ENTRY_POINT(GetUserPresent2),
		OCULUS_BIND_ENTRY_POINT(GetUserIPD2),
		OCULUS_BIND_ENTRY_POINT(SetUserIPD2),
		OCULUS_BIND_ENTRY_POINT(GetUserEyeHeight2),
		OCULUS_BIND_ENTRY_POINT(SetUserEyeHeight2),
		OCULUS_BIND_ENTRY_POINT(GetUserNeckEyeDistance2),
		OCULUS_BIND_ENTRY_POINT(SetUserNeckEyeDistance2),
		OCULUS_BIND_ENTRY_POINT(SetupDisplayObjects2),
		OCULUS_BIND_ENTRY_POINT(GetSystemMultiViewSupported2),
		OCULUS_BIND_ENTRY_POINT(GetEyeTextureArraySupported2),
		OCULUS_BIND_ENTRY_POINT(GetBoundaryConfigured2),
		OCULUS_BIND_ENTRY_POINT(GetDepthCompositingSupported),
		OCULUS_BIND_ENTRY_POINT(TestBoundaryNode2),
		OCULUS_BIND_ENTRY_POINT(TestBoundaryPoint2),
		OCULUS_BIND_ENTRY_POINT(GetBoundaryGeometry3),
		OCULUS_BIND_ENTRY_POINT(GetBoundaryDimensions2),
		OCULUS_BIND_ENTRY_POINT(GetBoundaryVisible2),
		OCULUS_BIND_ENTRY_POINT(SetBoundaryVisible2),
		OCULUS_BIND_ENTRY_POINT(GetSystemHeadsetType2),
		OCULUS_BIND_ENTRY_POINT(GetAppPerfStats2),
		OCULUS_BIND_ENTRY_POINT(ResetAppPerfStats2),
		OCULUS_BIND_ENTRY_POINT(GetAppFramerate2),
		OCULUS_BIND_ENTRY_POINT(IsPerfMetricsSupported),
		OCULUS_BIND_ENTRY_POINT(GetPerfMetricsFloat),
		OCULUS_BIND_ENTRY_POINT(GetPerfMetricsInt),
		OCULUS_BIND_ENTRY_POINT(SetHandNodePoseStateLatency),
		OCULUS_BIND_ENTRY_POINT(GetHandNodePoseStateLatency),
		OCULUS_BIND_ENTRY_POINT(GetSystemRecommendedMSAALevel2),
		OCULUS_BIND_ENTRY_POINT(SetInhibitSystemUX2),
		OCULUS_BIND_ENTRY_POINT(GetTiledMultiResSupported),
		OCULUS_BIND_ENTRY_POINT(GetTiledMultiResLevel),
		OCULUS_BIND_ENTRY_POINT(SetTiledMultiResLevel),
		OCULUS_BIND_ENTRY_POINT(GetTiledMultiResDynamic),
		OCULUS_BIND_ENTRY_POINT(SetTiledMultiResDynamic),
		OCULUS_BIND_ENTRY_POINT(GetFoveationEyeTrackedSupported),
		OCULUS_BIND_ENTRY_POINT(GetFoveationEyeTracked),
		OCULUS_BIND_ENTRY_POINT(SetFoveationEyeTracked),
		OCULUS_BIND_ENTRY_POINT(GetFoveationEyeTrackedCenter),
		OCULUS_BIND_ENTRY_POINT(GetGPUUtilSupported),
		OCULUS_BIND_ENTRY_POINT(GetGPUUtilLevel),
		OCULUS_BIND_ENTRY_POINT(SetThreadPerformance),
		OCULUS_BIND_ENTRY_POINT(AutoThreadScheduling),
		OCULUS_BIND_ENTRY_POINT(GetGPUFrameTime),
		OCULUS_BIND_ENTRY_POINT(GetViewportStencil),
		OCULUS_BIND_ENTRY_POINT(SetDeveloperTelemetryConsent),
		OCULUS_BIND_ENTRY_POINT(SendEvent),
		OCULUS_BIND_ENTRY_POINT(SendEvent2),
		OCULUS_BIND_ENTRY_POINT(AddCustomMetadata),
		OCULUS_BIND_ENTRY_POINT(SetDeveloperMode),
		OCULUS_BIND_ENTRY_POINT(GetCurrentTrackingTransformPose),
		OCULUS_BIND_ENTRY_POINT(GetTrackingTransformRawPose),
		OCULUS_BIND_ENTRY_POINT(GetTrackingTransformRelativePose),
		OCULUS_BIND_ENTRY_POINT(GetTimeInSeconds),
		// OCULUS_BIND_ENTRY_POINT(GetPTWNear),
		OCULUS_BIND_ENTRY_POINT(GetASWVelocityScale),
		OCULUS_BIND_ENTRY_POINT(GetASWDepthScale),
		OCULUS_BIND_ENTRY_POINT(GetASWAdaptiveMode),
		OCULUS_BIND_ENTRY_POINT(SetASWAdaptiveMode),
		OCULUS_BIND_ENTRY_POINT(IsRequestingASWData),
		OCULUS_BIND_ENTRY_POINT(GetPredictedDisplayTime),
		OCULUS_BIND_ENTRY_POINT(GetHandTrackingEnabled),
		OCULUS_BIND_ENTRY_POINT(GetHandState),
		OCULUS_BIND_ENTRY_POINT(GetHandState2),
		OCULUS_BIND_ENTRY_POINT(GetHandTrackingState),
		OCULUS_BIND_ENTRY_POINT(GetSkeleton2),
		OCULUS_BIND_ENTRY_POINT(GetSkeleton3),
		OCULUS_BIND_ENTRY_POINT(GetMesh),
		OCULUS_BIND_ENTRY_POINT(GetLocalTrackingSpaceRecenterCount),
		OCULUS_BIND_ENTRY_POINT(GetSystemHmd3DofModeEnabled),
		OCULUS_BIND_ENTRY_POINT(SetClientColorDesc),
		OCULUS_BIND_ENTRY_POINT(GetHmdColorDesc),
		OCULUS_BIND_ENTRY_POINT(PollEvent),
		OCULUS_BIND_ENTRY_POINT(RegisterOpenXREventHandler),
		OCULUS_BIND_ENTRY_POINT(UnregisterOpenXREventHandler),
		OCULUS_BIND_ENTRY_POINT(GetNativeXrApiType),
		OCULUS_BIND_ENTRY_POINT(GetLocalDimmingSupported),
		OCULUS_BIND_ENTRY_POINT(SetLocalDimming),
		OCULUS_BIND_ENTRY_POINT(GetCurrentInteractionProfile),
		OCULUS_BIND_ENTRY_POINT(GetLayerRecommendedResolution),
		OCULUS_BIND_ENTRY_POINT(IsLayerShapeSupported),
		OCULUS_BIND_ENTRY_POINT(SetEyeBufferSharpenType),
		OCULUS_BIND_ENTRY_POINT(GetOpenXRInstanceProcAddrFunc),

		OCULUS_BIND_ENTRY_POINT(SaveUnifiedConsent),
		OCULUS_BIND_ENTRY_POINT(SaveUnifiedConsentWithOlderVersion),
		OCULUS_BIND_ENTRY_POINT(GetUnifiedConsent),
		OCULUS_BIND_ENTRY_POINT(GetConsentTitle),
		OCULUS_BIND_ENTRY_POINT(GetConsentMarkdownText),
		OCULUS_BIND_ENTRY_POINT(GetConsentNotificationMarkdownText),
		OCULUS_BIND_ENTRY_POINT(ShouldShowTelemetryConsentWindow),
		OCULUS_BIND_ENTRY_POINT(ShouldShowTelemetryNotification),
		OCULUS_BIND_ENTRY_POINT(SetNotificationShown),
		OCULUS_BIND_ENTRY_POINT(GetConsentSettingsChangeText),
		OCULUS_BIND_ENTRY_POINT(IsConsentSettingsChangeEnabled),

		OCULUS_BIND_ENTRY_POINT(InitializeEnvironmentDepth),
		OCULUS_BIND_ENTRY_POINT(DestroyEnvironmentDepth),
		OCULUS_BIND_ENTRY_POINT(GetEnvironmentDepthTextureDesc),
		OCULUS_BIND_ENTRY_POINT(GetEnvironmentDepthTextureStageCount),
		OCULUS_BIND_ENTRY_POINT(GetEnvironmentDepthTexture),
		OCULUS_BIND_ENTRY_POINT(SetEnvironmentDepthHandRemoval),
		OCULUS_BIND_ENTRY_POINT(StartEnvironmentDepth),
		OCULUS_BIND_ENTRY_POINT(StopEnvironmentDepth),
		OCULUS_BIND_ENTRY_POINT(GetEnvironmentDepthFrameDesc),


#ifndef OVRPLUGIN_JNI_LIB_EXCLUDED
		OCULUS_BIND_ENTRY_POINT(GetSystemVolume2),
		OCULUS_BIND_ENTRY_POINT(GetSystemHeadphonesPresent2),
#endif

		// Anchors
		OCULUS_BIND_ENTRY_POINT(LocateSpace),
		OCULUS_BIND_ENTRY_POINT(LocateSpace2),
		OCULUS_BIND_ENTRY_POINT(CreateSpatialAnchor),
		OCULUS_BIND_ENTRY_POINT(DestroySpace),
		OCULUS_BIND_ENTRY_POINT(SetSpaceComponentStatus),
		OCULUS_BIND_ENTRY_POINT(GetSpaceComponentStatus),
		OCULUS_BIND_ENTRY_POINT(EnumerateSpaceSupportedComponents),
		OCULUS_BIND_ENTRY_POINT(QuerySpaces),
		OCULUS_BIND_ENTRY_POINT(QuerySpaces2),
		OCULUS_BIND_ENTRY_POINT(RetrieveSpaceQueryResults),
		OCULUS_BIND_ENTRY_POINT(SaveSpace),
		OCULUS_BIND_ENTRY_POINT(EraseSpace),
		OCULUS_BIND_ENTRY_POINT(GetSpaceUuid),
		OCULUS_BIND_ENTRY_POINT(SaveSpaceList),
		OCULUS_BIND_ENTRY_POINT(ShareSpaces),
		OCULUS_BIND_ENTRY_POINT(ShareSpaces2),
		OCULUS_BIND_ENTRY_POINT(CreateSpaceUser),
		OCULUS_BIND_ENTRY_POINT(DestroySpaceUser),

		// Anchors 2.0 (APD)
		OCULUS_BIND_ENTRY_POINT(DiscoverSpaces),
		OCULUS_BIND_ENTRY_POINT(RetrieveSpaceDiscoveryResults),
		OCULUS_BIND_ENTRY_POINT(SaveSpaces),
		OCULUS_BIND_ENTRY_POINT(EraseSpaces),

		// Scene
		OCULUS_BIND_ENTRY_POINT(GetSpaceContainer),
		OCULUS_BIND_ENTRY_POINT(GetSpaceBoundingBox2D),
		OCULUS_BIND_ENTRY_POINT(GetSpaceBoundingBox3D),
		OCULUS_BIND_ENTRY_POINT(GetSpaceSemanticLabels),
		OCULUS_BIND_ENTRY_POINT(GetSpaceRoomLayout),
		OCULUS_BIND_ENTRY_POINT(GetSpaceBoundary2D),
		OCULUS_BIND_ENTRY_POINT(RequestSceneCapture),
		OCULUS_BIND_ENTRY_POINT(GetSpaceTriangleMesh),


		// Boundary Visibility
		OCULUS_BIND_ENTRY_POINT(RequestBoundaryVisibility),
		OCULUS_BIND_ENTRY_POINT(GetBoundaryVisibility),

		// Colocation Session
		OCULUS_BIND_ENTRY_POINT(StartColocationDiscovery),
		OCULUS_BIND_ENTRY_POINT(StopColocationDiscovery),
		OCULUS_BIND_ENTRY_POINT(StartColocationAdvertisement),
		OCULUS_BIND_ENTRY_POINT(StopColocationAdvertisement),

		// MovementSDK
		OCULUS_BIND_ENTRY_POINT(GetBodyTrackingEnabled),
		OCULUS_BIND_ENTRY_POINT(GetBodyTrackingSupported),
		OCULUS_BIND_ENTRY_POINT(StopBodyTracking),
		OCULUS_BIND_ENTRY_POINT(GetBodyState4),
		OCULUS_BIND_ENTRY_POINT(GetFullBodyTrackingEnabled),
		OCULUS_BIND_ENTRY_POINT(StartBodyTracking2),
		OCULUS_BIND_ENTRY_POINT(RequestBodyTrackingFidelity),
		OCULUS_BIND_ENTRY_POINT(ResetBodyTrackingCalibration),
		OCULUS_BIND_ENTRY_POINT(SuggestBodyTrackingCalibrationOverride),

		OCULUS_BIND_ENTRY_POINT(GetFaceTracking2Enabled),
		OCULUS_BIND_ENTRY_POINT(GetFaceTracking2Supported),
		OCULUS_BIND_ENTRY_POINT(GetFaceState2),
		OCULUS_BIND_ENTRY_POINT(StartFaceTracking2),
		OCULUS_BIND_ENTRY_POINT(StopFaceTracking2),
		OCULUS_BIND_ENTRY_POINT(GetEyeTrackingEnabled),
		OCULUS_BIND_ENTRY_POINT(GetEyeTrackingSupported),
		OCULUS_BIND_ENTRY_POINT(GetEyeGazesState),
		OCULUS_BIND_ENTRY_POINT(StartEyeTracking),
		OCULUS_BIND_ENTRY_POINT(StopEyeTracking),
		OCULUS_BIND_ENTRY_POINT(GetFaceTrackingVisemesSupported),
		OCULUS_BIND_ENTRY_POINT(GetFaceTrackingVisemesEnabled),
		OCULUS_BIND_ENTRY_POINT(GetFaceVisemesState),
		OCULUS_BIND_ENTRY_POINT(SetFaceTrackingVisemesEnabled),

		// QPL
		OCULUS_BIND_ENTRY_POINT(QplMarkerStart),
		OCULUS_BIND_ENTRY_POINT(QplMarkerEnd),
		OCULUS_BIND_ENTRY_POINT(QplMarkerPoint),
		OCULUS_BIND_ENTRY_POINT(QplMarkerPointCached),
		OCULUS_BIND_ENTRY_POINT(QplMarkerAnnotation),
		OCULUS_BIND_ENTRY_POINT(QplCreateMarkerHandle),
		OCULUS_BIND_ENTRY_POINT(QplDestroyMarkerHandle),
		OCULUS_BIND_ENTRY_POINT(OnEditorShutdown),
		OCULUS_BIND_ENTRY_POINT(QplSetConsent),

		// OVR_Plugin_Insight.h
		OCULUS_BIND_ENTRY_POINT(InitializeInsightPassthrough),
		OCULUS_BIND_ENTRY_POINT(ShutdownInsightPassthrough),
		OCULUS_BIND_ENTRY_POINT(GetInsightPassthroughInitialized),
		OCULUS_BIND_ENTRY_POINT(GetInsightPassthroughInitializationState),
		OCULUS_BIND_ENTRY_POINT(CreateInsightTriangleMesh),
		OCULUS_BIND_ENTRY_POINT(DestroyInsightTriangleMesh),
		OCULUS_BIND_ENTRY_POINT(AddInsightPassthroughSurfaceGeometry),
		OCULUS_BIND_ENTRY_POINT(DestroyInsightPassthroughGeometryInstance),
		OCULUS_BIND_ENTRY_POINT(UpdateInsightPassthroughGeometryTransform),
		OCULUS_BIND_ENTRY_POINT(SetInsightPassthroughStyle),
		OCULUS_BIND_ENTRY_POINT(SetInsightPassthroughStyle2),
		OCULUS_BIND_ENTRY_POINT(GetPassthroughCapabilityFlags),
		OCULUS_BIND_ENTRY_POINT(CreatePassthroughColorLut),
		OCULUS_BIND_ENTRY_POINT(DestroyPassthroughColorLut),
		OCULUS_BIND_ENTRY_POINT(UpdatePassthroughColorLut),
		OCULUS_BIND_ENTRY_POINT(GetPassthroughCapabilities),
		OCULUS_BIND_ENTRY_POINT(GetPassthroughPreferences),

		// OVR_Plugin_MixedReality.h

		OCULUS_BIND_ENTRY_POINT(InitializeMixedReality),
		OCULUS_BIND_ENTRY_POINT(ShutdownMixedReality),
		OCULUS_BIND_ENTRY_POINT(GetMixedRealityInitialized),
		OCULUS_BIND_ENTRY_POINT(UpdateExternalCamera),
		OCULUS_BIND_ENTRY_POINT(GetExternalCameraCount),
		OCULUS_BIND_ENTRY_POINT(GetExternalCameraName),
		OCULUS_BIND_ENTRY_POINT(GetExternalCameraIntrinsics),
		OCULUS_BIND_ENTRY_POINT(GetExternalCameraExtrinsics),

		// OVR_Plugin_Media.h

		OCULUS_BIND_ENTRY_POINT(Media_Initialize),
		OCULUS_BIND_ENTRY_POINT(Media_Shutdown),
		OCULUS_BIND_ENTRY_POINT(Media_GetInitialized),
		OCULUS_BIND_ENTRY_POINT(Media_Update),
		OCULUS_BIND_ENTRY_POINT(Media_GetMrcActivationMode),
		OCULUS_BIND_ENTRY_POINT(Media_SetMrcActivationMode),
		OCULUS_BIND_ENTRY_POINT(Media_IsMrcEnabled),
		OCULUS_BIND_ENTRY_POINT(Media_IsMrcActivated),
		OCULUS_BIND_ENTRY_POINT(Media_UseMrcDebugCamera),
		OCULUS_BIND_ENTRY_POINT(Media_SetMrcInputVideoBufferType),
		OCULUS_BIND_ENTRY_POINT(Media_GetMrcInputVideoBufferType),
		OCULUS_BIND_ENTRY_POINT(Media_SetMrcFrameSize),
		OCULUS_BIND_ENTRY_POINT(Media_GetMrcFrameSize),
		OCULUS_BIND_ENTRY_POINT(Media_SetMrcAudioSampleRate),
		OCULUS_BIND_ENTRY_POINT(Media_GetMrcAudioSampleRate),
		OCULUS_BIND_ENTRY_POINT(Media_SetMrcFrameImageFlipped),
		OCULUS_BIND_ENTRY_POINT(Media_GetMrcFrameImageFlipped),
		OCULUS_BIND_ENTRY_POINT(Media_SetMrcFrameInverseAlpha),
		OCULUS_BIND_ENTRY_POINT(Media_GetMrcFrameInverseAlpha),
		OCULUS_BIND_ENTRY_POINT(Media_SetAvailableQueueIndexVulkan),
		OCULUS_BIND_ENTRY_POINT(Media_EncodeMrcFrame),
		OCULUS_BIND_ENTRY_POINT(Media_EncodeMrcFrameWithDualTextures),
		OCULUS_BIND_ENTRY_POINT(Media_SyncMrcFrame),
		OCULUS_BIND_ENTRY_POINT(Media_EncodeMrcFrameWithPoseTime),
		OCULUS_BIND_ENTRY_POINT(Media_EncodeMrcFrameDualTexturesWithPoseTime),
		OCULUS_BIND_ENTRY_POINT(Media_SetHeadsetControllerPose),
		OCULUS_BIND_ENTRY_POINT(Media_EnumerateCameraAnchorHandles),
		OCULUS_BIND_ENTRY_POINT(Media_GetCurrentCameraAnchorHandle),
		OCULUS_BIND_ENTRY_POINT(Media_GetCameraAnchorName),
		OCULUS_BIND_ENTRY_POINT(Media_GetCameraAnchorHandle),
		OCULUS_BIND_ENTRY_POINT(Media_GetCameraAnchorType),
		OCULUS_BIND_ENTRY_POINT(Media_CreateCustomCameraAnchor),
		OCULUS_BIND_ENTRY_POINT(Media_DestroyCustomCameraAnchor),
		OCULUS_BIND_ENTRY_POINT(Media_GetCustomCameraAnchorPose),
		OCULUS_BIND_ENTRY_POINT(Media_SetCustomCameraAnchorPose),
		OCULUS_BIND_ENTRY_POINT(Media_GetCameraMinMaxDistance),
		OCULUS_BIND_ENTRY_POINT(Media_SetCameraMinMaxDistance),

		OCULUS_BIND_ENTRY_POINT(SetControllerDrivenHandPoses),
		OCULUS_BIND_ENTRY_POINT(SetControllerDrivenHandPosesAreNatural),
	};

#undef OCULUS_BIND_ENTRY_POINT

	bool result = true;
	for (int i = 0; i < UE_ARRAY_COUNT(entryPointArray); ++i)
	{
		*(entryPointArray[i].EntryPointPtr) = LoadEntryPoint(LibraryHandle, entryPointArray[i].EntryPointName);

		if (*entryPointArray[i].EntryPointPtr == nullptr)
		{
			UE_LOG(LogOculusPluginWrapper, Error, TEXT("OculusPlugin EntryPoint could not be loaded: %s"), ANSI_TO_TCHAR(entryPointArray[i].EntryPointName));
			result = false;
		}
	}

	wrapper->Initialized = true;

	if (result)
	{
		UE_LOG(LogOculusPluginWrapper, Log, TEXT("OculusPlugin initialized successfully"));
	}
	else
	{
		DestroyOculusPluginWrapper(wrapper);
	}

	return result;
}

void OculusPluginWrapper::DestroyOculusPluginWrapper(OculusPluginWrapper* wrapper)
{
	if (!wrapper->Initialized)
		return;

	wrapper->Reset();

	UE_LOG(LogOculusPluginWrapper, Log, TEXT("OculusPlugin destroyed successfully"));
}

static void* LoadEntryPoint(void* Handle, const char* EntryPointName)
{
	if (Handle == nullptr)
		return nullptr;

#if PLATFORM_WINDOWS
	void* ptr = GetProcAddress((HMODULE)Handle, EntryPointName);
	if (ptr == nullptr)
	{
		UE_LOG(LogOculusPluginWrapper, Error, TEXT("Unable to load entry point: %s"), ANSI_TO_TCHAR(EntryPointName));
	}
	return ptr;
#elif PLATFORM_ANDROID
	void* ptr = dlsym(Handle, EntryPointName);
	if (ptr == nullptr)
	{
		UE_LOG(LogOculusPluginWrapper, Error, TEXT("Unable to load entry point: %s, error %s"), ANSI_TO_TCHAR(EntryPointName), ANSI_TO_TCHAR(dlerror()));
	}
	return ptr;
#else
	UE_LOG(LogOculusPluginWrapper, Error, TEXT("LoadEntryPoint: Unsupported platform"));
	return nullptr;
#endif
}
