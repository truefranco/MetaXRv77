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

		// Check for Mixed Reality Composition modes first.
		// This logic is ported from the old RenderSpectatorScreen_RenderThread function.
		if (OculusXRHMD->GetCustomPresent_Internal())
		{

			if (SpectatorMode == EMRSpectatorScreenMode::ExternalComposition)
			{
				// This mode composites a foreground and background texture.
				auto ForegroundTex = ForegroundRenderTexture->GetRenderTargetResource();
				auto BackgroundTex = BackgroundRenderTexture->GetRenderTargetResource();

				if (ForegroundTex && BackgroundTex && ForegroundTex->GetRenderTargetTexture() && BackgroundTex->GetRenderTargetTexture())
				{
					// Get the RHI textures for the MRC layers.
					FTextureRHIRef FrontTextureRHI = ForegroundTex->GetRenderTargetTexture();
					FTextureRHIRef BackTextureRHI = BackgroundTex->GetRenderTargetTexture();

					// Register the external RHI textures with RDG to use them in a pass.
					FRDGTextureRef FrontRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(FrontTextureRHI, TEXT("OculusMRCFront")));
					FRDGTextureRef BackRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(BackTextureRHI, TEXT("OculusMRCBack")));

					RenderSpectatorModeExternalComposition(
						GraphBuilder.RHICmdList,
						BackBuffer,
						FrontRDG,
						BackRDG);
					return; // We've handled the rendering, so we can exit.
				}
			}
			else if (SpectatorMode == EMRSpectatorScreenMode::DirectComposition)
			{
				// This mode composites just a background texture.
				auto BackgroundTex = BackgroundRenderTexture->GetRenderTargetResource();
				if (BackgroundTex && BackgroundTex->GetRenderTargetTexture())
				{
					FTextureRHIRef BackTextureRHI = BackgroundTex->GetRenderTargetTexture();
					FRDGTextureRef BackRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(BackTextureRHI, TEXT("OculusMRCBack")));

					RenderSpectatorModeDirectComposition(
						GraphBuilder.RHICmdList,
						BackBuffer,
						BackRDG);
					return; // We've handled the rendering, so we can exit.
				}
			}
		}

		// If not in a special MRC mode, fall back to the default spectator screen rendering.
		// This will call our AddSpectatorModePass override for standard modes.
		FDefaultSpectatorScreenController::RenderSpectatorScreen_RenderThread(GraphBuilder, BackBuffer, SrcTexture, LayersTexture, WindowSize);
	}

	void FSpectatorScreenController::AddSpectatorModePass(ESpectatorScreenMode Spectator_Mode, FRDGBuilder& GraphBuilder, FRDGTextureRef TargetTexture, FRDGTextureRef EyeTexture, FRDGTextureRef OtherTexture, FVector2f WindowSize)
	{
		CheckInRenderThread();
		FSettings* Settings = OculusXRHMD->GetSettings_RenderThread();
		if (!Settings)
		{
			return;
		}

		switch (Spectator_Mode)
		{
		case ESpectatorScreenMode::Undistorted:
		{
			CheckInRenderThread();
			Settings = OculusXRHMD->GetSettings_RenderThread();
			FIntRect DestRect(0, 0, TargetTexture->Desc.GetSize().X / 2, TargetTexture->Desc.GetSize().Y);
			for (int i = 0; i < 2; ++i)
			{
				OculusXRHMD->CopyTexture_RenderThread(GraphBuilder.RHICmdList, EyeTexture->GetRHI(), Settings->EyeRenderViewport[i], TargetTexture->GetRHI(), DestRect, false, true);
				DestRect.Min.X += TargetTexture->Desc.GetSize().X / 2;
				DestRect.Max.X += TargetTexture->Desc.GetSize().X / 2;
			}
			break;
		}

		case ESpectatorScreenMode::Distorted:
		{
			CheckInRenderThread();
			FCustomPresent* CustomPresent = OculusXRHMD->GetCustomPresent_Internal();
			FTextureRHIRef MirrorTexture = CustomPresent->GetMirrorTexture();
			if (MirrorTexture)
			{
				FIntRect SrcRect(0, 0, MirrorTexture->GetSizeX(), MirrorTexture->GetSizeY());
				FIntRect DestRect(0, 0, TargetTexture->Desc.GetSize().X, TargetTexture->Desc.GetSize().Y);
				OculusXRHMD->CopyTexture_RenderThread(GraphBuilder.RHICmdList, MirrorTexture, SrcRect, TargetTexture->GetRHI(), DestRect, false, true);
			}
			break;
		}

		case ESpectatorScreenMode::SingleEye:
		{
			CheckInRenderThread();
			Settings = OculusXRHMD->GetSettings_RenderThread();
			const FIntRect SrcRect = Settings->EyeRenderViewport[0];
			const FIntRect DstRect(0, 0, TargetTexture->Desc.GetSize().X, TargetTexture->Desc.GetSize().Y);

			OculusXRHMD->CopyTexture_RenderThread(GraphBuilder.RHICmdList, EyeTexture->GetRHI(), SrcRect, TargetTexture->GetRHI(), DstRect, false, true);
			break;
		}

		default:
		{
			// For all other modes, use the default engine behavior.
			FDefaultSpectatorScreenController::AddSpectatorModePass(Spectator_Mode, GraphBuilder, TargetTexture, EyeTexture, OtherTexture, WindowSize);
			break;
		}
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
