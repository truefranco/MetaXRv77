// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "CoreMinimal.h"
#include "SceneViewExtension.h"
#include "XRSwapChain.h"
#include "OpenXR/IOculusXRExtensionPlugin.h"

#if defined(WITH_OCULUS_BRANCH) || defined(WITH_OPENXR_BRANCH)

DECLARE_LOG_CATEGORY_EXTERN(LogOculusSpaceWarpExtensionPlugin, Log, All);

struct FOpenXRSwapchainProperties;

namespace OculusXR
{
	class FSpaceWarpExtensionPlugin : public IOculusXRExtensionPlugin
	{
	public:
		FSpaceWarpExtensionPlugin();

		// IOpenXRExtensionPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual void* OnEnumerateViewConfigurationViews(XrInstance InInstance, XrSystemId InSystem, XrViewConfigurationType InViewConfigurationType, uint32_t InViewIndex, void* InNext) override;
		virtual const void* OnCreateInstance(IOpenXRHMDModule* InModule, const void* InNext) override;
		virtual void PostCreateInstance(XrInstance InInstance) override;
		virtual void PostCreateSession(XrSession InSession) override;
		virtual void OnBeginRendering_RenderThread(XrSession InSession) override;
		virtual const void* OnBeginProjectionView(XrSession InSession, int32 InLayerIndex, int32 InViewIndex, const void* InNext) override;
		virtual void PostBeginFrame_RHIThread(XrTime PredictedDisplayTime) override;
		virtual const void* OnEndFrame(XrSession InSession, XrTime DisplayTime, const void* InNext) override;
		virtual void PostEndFrame_RHIThread() override;
		virtual void AllocateRenderTargetTextures_RenderThread() override;
		virtual bool GetRecommendedVelocityTextureSize_RenderThread(FIntPoint& OutTextureSize) override;
		virtual FXRSwapChainPtr GetVelocitySwapchain_RenderThread() override;
		virtual FXRSwapChainPtr GetVelocityDepthSwapchain_RenderThread() override;

		bool IsSpaceWarpEnabled() const;

	private:
		class FSpaceWarpViewExtension : public FHMDSceneViewExtension
		{
		public:
			FSpaceWarpViewExtension(const FAutoRegister& AutoRegister, FSpaceWarpExtensionPlugin* InPlugin);

			// ISceneViewExtension
			virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
			virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
			virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;

		private:
			friend FSpaceWarpExtensionPlugin;

			FSpaceWarpExtensionPlugin* SpaceWarpExtensionPlugin = nullptr;
		};

		struct FPipelinedVelocityState
		{
			FXRSwapChainPtr VelocitySwapchain;
			FXRSwapChainPtr VelocityDepthSwapchain;

			bool bEnabled = false;
		};

		TArray<TSharedPtr<XrSwapchainSubImage>> VelocityImages_RHIThread;
		TArray<TSharedPtr<XrSwapchainSubImage>> VelocityDepthImages_RHIThread;
		TArray<TSharedPtr<XrCompositionLayerSpaceWarpInfoFB>> SpaceWarpLayerInfo_RHIThread;

		std::atomic<bool> bSpaceWarpExtensionEnabled = false;
		bool bIsMobileMultiViewEnabled = false;

		TSharedPtr<FSpaceWarpViewExtension> SpaceWarpViewExtension = nullptr;
		FPipelinedVelocityState PipelinedVelocityState_RenderThread = {};
		FPipelinedVelocityState PipelinedVelocityState_RHIThread = {};

		XrViewConfigurationType SelectedViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		XrSystemSpaceWarpPropertiesFB SpaceWarpSystemProperties = { XR_TYPE_SYSTEM_SPACE_WARP_PROPERTIES_FB };

		FTransform LastTrackingToWorld = FTransform::Identity;
	};

} // namespace OculusXR

#endif // defined(WITH_OCULUS_BRANCH)
