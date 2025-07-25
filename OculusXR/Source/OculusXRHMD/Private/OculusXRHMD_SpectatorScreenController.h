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

		// ADD these two new overrides
		virtual void RenderSpectatorScreen_RenderThread(class FRDGBuilder& GraphBuilder, FRDGTextureRef BackBuffer, FRDGTextureRef SrcTexture, FRDGTextureRef LayersTexture, FVector2f WindowSize) override;
		virtual void AddSpectatorModePass(ESpectatorScreenMode SpectatorMode, class FRDGBuilder& GraphBuilder, FRDGTextureRef TargetTexture, FRDGTextureRef EyeTexture, FRDGTextureRef OtherTexture, FVector2f WindowSize) override;

	private:
		FOculusXRHMD* OculusXRHMD;
		EMRSpectatorScreenMode SpectatorMode;
		UTextureRenderTarget2D* ForegroundRenderTexture;
		UTextureRenderTarget2D* BackgroundRenderTexture;

		void RenderSpectatorModeDirectComposition(FRHICommandListImmediate& RHICmdList, FRDGTextureRef TargetTexture, const FRDGTextureRef SrcTexture) const;
		void RenderSpectatorModeExternalComposition(FRHICommandListImmediate& RHICmdList, FRDGTextureRef TargetTexture, const FRDGTextureRef FrontTexture, const FRDGTextureRef BackTexture) const;
	};

} // namespace OculusXRHMD

#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
