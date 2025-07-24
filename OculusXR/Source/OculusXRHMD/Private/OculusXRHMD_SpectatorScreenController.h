// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "OculusXRHMDPrivate.h"

#if OCULUS_HMD_SUPPORTED_PLATFORMS
#include "DefaultSpectatorScreenController.h"

class UTextureRenderTarget2D;

namespace OculusXRHMD
{

	// Oculus specific spectator screen modes that override the regular VR spectator screens
	enum class EMRSpectatorScreenMode : uint8
	{
		Default,
		ExternalComposition,
		DirectComposition
	};

	//-------------------------------------------------------------------------------------------------
	// FSpectatorScreenController
	//-------------------------------------------------------------------------------------------------

	class FSpectatorScreenController : public FDefaultSpectatorScreenController
	{
	public:
		FSpectatorScreenController(class FOculusXRHMD* InOculusXRHMD);

		void SetMRSpectatorScreenMode(EMRSpectatorScreenMode Mode) { SpectatorMode = Mode; }
		void SetMRForeground(UTextureRenderTarget2D* Texture) { ForegroundRenderTexture = Texture; }
		void SetMRBackground(UTextureRenderTarget2D* Texture) { BackgroundRenderTexture = Texture; }

		virtual void RenderSpectatorScreen_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* BackBuffer, FTextureRHIRef RenderTarget, FVector2D WindowSize) override;
		virtual void RenderSpectatorModeUndistorted(FRHICommandListImmediate& RHICmdList, FTextureRHIRef TargetTexture, FTextureRHIRef EyeTexture, FTextureRHIRef OtherTexture, FVector2D WindowSize) override;
		virtual void RenderSpectatorModeDistorted(FRHICommandListImmediate& RHICmdList, FTextureRHIRef TargetTexture, FTextureRHIRef EyeTexture, FTextureRHIRef OtherTexture, FVector2D WindowSize) override;
		virtual void RenderSpectatorModeSingleEye(FRHICommandListImmediate& RHICmdList, FTextureRHIRef TargetTexture, FTextureRHIRef EyeTexture, FTextureRHIRef OtherTexture, FVector2D WindowSize) override;

	private:
		FOculusXRHMD* OculusXRHMD;
		EMRSpectatorScreenMode SpectatorMode;
		UTextureRenderTarget2D* ForegroundRenderTexture;
		UTextureRenderTarget2D* BackgroundRenderTexture;

		void RenderSpectatorModeDirectComposition(FRHICommandListImmediate& RHICmdList, FTextureRHIRef TargetTexture, const FTextureRHIRef SrcTexture) const;
		void RenderSpectatorModeExternalComposition(FRHICommandListImmediate& RHICmdList, FTextureRHIRef TargetTexture, const FTextureRHIRef FrontTexture, const FTextureRHIRef BackTexture) const;
	};

} // namespace OculusXRHMD

#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
