// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRHMD_SpectatorScreenController.h"

#if OCULUS_HMD_SUPPORTED_PLATFORMS
#include "OculusXRHMD.h"
#include "TextureResource.h"
#include "Engine/TextureRenderTarget2D.h"

namespace OculusXRHMD
{

	//-------------------------------------------------------------------------------------------------
	// FSpectatorScreenController
	//-------------------------------------------------------------------------------------------------

	FSpectatorScreenController::FSpectatorScreenController(FOculusXRHMD* InOculusXRHMD)
		: FDefaultSpectatorScreenController(InOculusXRHMD)
		, OculusXRHMD(InOculusXRHMD)
		, SpectatorMode(EMRSpectatorScreenMode::Default)
		, ForegroundRenderTexture(nullptr)
		, BackgroundRenderTexture(nullptr)
	{
	}

	void FSpectatorScreenController::RenderSpectatorScreen_RenderThread(FRDGBuilder& GraphBuilder, FRDGTextureRef BackBuffer, FRDGTextureRef SrcTexture, FRDGTextureRef LayersTexture, FVector2f WindowSize)
	{
		CheckInRenderThread();
		if (OculusXRHMD->GetCustomPresent_Internal())
		{
			if (SpectatorMode == EMRSpectatorScreenMode::ExternalComposition)
			{
				auto ForegroundResource = ForegroundRenderTexture->GetRenderTargetResource();
				auto BackgroundResource = BackgroundRenderTexture->GetRenderTargetResource();
				if (ForegroundResource && BackgroundResource)
				{
					FRDGTextureRef RDGForegroundResource = RegisterExternalTexture(GraphBuilder, ForegroundResource->GetRenderTargetTexture(), TEXT("OculusXRForegroundDebugLayerTexture"));
					FRDGTextureRef RDGBackgroundResource = RegisterExternalTexture(GraphBuilder, BackgroundResource->GetRenderTargetTexture(), TEXT("OculusXRBackgroundDebugLayerTexture"));

					RenderSpectatorModeExternalComposition(
						GraphBuilder.RHICmdList,
						BackBuffer,
						RDGForegroundResource,
						RDGBackgroundResource);
					return;
				}
			}
			else if (SpectatorMode == EMRSpectatorScreenMode::DirectComposition)
			{
				auto BackgroundResource = BackgroundRenderTexture->GetRenderTargetResource();
				if (BackgroundResource)
				{
					FRDGTextureRef RDGBackgroundResource = RegisterExternalTexture(GraphBuilder, BackgroundResource->GetRenderTargetTexture(), TEXT("OculusXRBackgroundDebugLayerTexture"));
					RenderSpectatorModeDirectComposition(
						GraphBuilder.RHICmdList,
						BackBuffer,
						RDGBackgroundResource);
					return;
				}
			}
			FDefaultSpectatorScreenController::RenderSpectatorScreen_RenderThread(GraphBuilder, BackBuffer, SrcTexture, LayersTexture, WindowSize);
		}
	}

	void FSpectatorScreenController::RenderSpectatorModeDirectComposition(FRHICommandListImmediate& RHICmdList, FRDGTextureRef TargetTexture, const FRDGTextureRef SrcTexture) const
	{
		CheckInRenderThread();
		const FIntRect SrcRect(0, 0, SrcTexture->Desc.GetSize().X, SrcTexture->Desc.GetSize().Y);
		const FIntRect DstRect(0, 0, TargetTexture->Desc.GetSize().X, TargetTexture->Desc.GetSize().Y);

		OculusXRHMD->CopyTexture_RenderThread(RHICmdList, SrcTexture->GetRHI(), SrcRect, TargetTexture->GetRHI(), DstRect, false, true);
	}

	void FSpectatorScreenController::RenderSpectatorModeExternalComposition(FRHICommandListImmediate& RHICmdList, FRDGTextureRef TargetTexture, const FRDGTextureRef FrontTexture, const FRDGTextureRef BackTexture) const
	{
		CheckInRenderThread();
		const FIntRect FrontSrcRect(0, 0, FrontTexture->Desc.GetSize().X, FrontTexture->Desc.GetSize().Y);
		const FIntRect FrontDstRect(0, 0, TargetTexture->Desc.GetSize().X / 2, TargetTexture->Desc.GetSize().Y);
		const FIntRect BackSrcRect(0, 0, BackTexture->Desc.GetSize().X, BackTexture->Desc.GetSize().Y);
		const FIntRect BackDstRect(TargetTexture->Desc.GetSize().X / 2, 0, TargetTexture->Desc.GetSize().X, TargetTexture->Desc.GetSize().Y);

		OculusXRHMD->CopyTexture_RenderThread(RHICmdList, FrontTexture->GetRHI(), FrontSrcRect, TargetTexture->GetRHI(), FrontDstRect, false, true);
		OculusXRHMD->CopyTexture_RenderThread(RHICmdList, BackTexture->GetRHI(), BackSrcRect, TargetTexture->GetRHI(), BackDstRect, false, true);
	}

} // namespace OculusXRHMD

#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
