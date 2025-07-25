// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRHMD_Splash.h"

#if OCULUS_HMD_SUPPORTED_PLATFORMS
#include "OculusXRHMD.h"
#include "RenderingThread.h"
#include "Misc/ScopeLock.h"
#include "OculusXRHMDRuntimeSettings.h"
#include "TextureResource.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include "OculusXRHMDTypes.h"
#endif

namespace OculusXRHMD
{

	//-------------------------------------------------------------------------------------------------
	// FSplash
	//-------------------------------------------------------------------------------------------------

	FSplash::FSplash(FOculusXRHMD* InOculusXRHMD)
		: OculusXRHMD(InOculusXRHMD), CustomPresent(InOculusXRHMD->GetCustomPresent_Internal()), FramesOutstanding(0), NextLayerId(1), bInitialized(false), bIsShown(false), bNeedSplashUpdate(false), bShouldShowSplash(false), SystemDisplayInterval(1 / 90.0f)
	{
		// Create empty quad layer for UE layer
		{
			IStereoLayers::FLayerDesc LayerDesc;
			LayerDesc.QuadSize = FVector2D(0.01f, 0.01f);
			LayerDesc.Priority = 0;
			LayerDesc.PositionType = IStereoLayers::TrackerLocked;
			LayerDesc.TextureObj = nullptr;
			UELayer = MakeShareable(new FLayer(NextLayerId++));
			UELayer->SetDesc(LayerDesc);
		}
	}

	FSplash::~FSplash()
	{
		// Make sure RenTicker is freed in Shutdown
		check(!Ticker.IsValid())
	}

	void FSplash::Tick_RenderThread(float DeltaTime)
	{
		CheckInRenderThread();

		if (FramesOutstanding > 0)
		{
			UE_LOG(LogHMD, VeryVerbose, TEXT("Splash skipping frame; too many frames outstanding"));
			return;
		}

		const double TimeInSeconds = FPlatformTime::Seconds();
		const double DeltaTimeInSeconds = TimeInSeconds - LastTimeInSeconds;

		if (DeltaTimeInSeconds > 2.f * SystemDisplayInterval && Layers_RenderThread_DeltaRotation.Num() > 0)
		{
			FScopeLock ScopeLock(&RenderThreadLock);
			for (TTuple<FLayerPtr, FQuat>& Info : Layers_RenderThread_DeltaRotation)
			{
				FLayerPtr Layer = Info.Key;
				const FQuat& DeltaRotation = Info.Value;
				check(Layer.IsValid());
				check(!DeltaRotation.Equals(FQuat::Identity)); // Only layers with non-zero delta rotation should be in the DeltaRotation array.

				IStereoLayers::FLayerDesc LayerDesc = Layer->GetDesc();
				LayerDesc.Transform.SetRotation(LayerDesc.Transform.GetRotation() * DeltaRotation);
				LayerDesc.Transform.NormalizeRotation();
				Layer->SetDesc(LayerDesc);
			}
			LastTimeInSeconds = TimeInSeconds;
		}

		RenderFrame_RenderThread(FRHICommandListExecutor::GetImmediateCommandList());
	}

	void FSplash::LoadSettings()
	{
		UOculusXRHMDRuntimeSettings* HMDSettings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();
		check(HMDSettings);
		ClearSplashes();
		for (const FOculusXRSplashDesc& SplashDesc : HMDSettings->SplashDescs)
		{
			AddSplash(SplashDesc);
		}

		if (HMDSettings->bAutoEnabled)
		{
			if (!PreLoadLevelDelegate.IsValid())
			{
				PreLoadLevelDelegate = FCoreUObjectDelegates::PreLoadMap.AddSP(this, &FSplash::OnPreLoadMap);
			}
			if (!PostLoadLevelDelegate.IsValid())
			{
				PostLoadLevelDelegate = FCoreUObjectDelegates::PostLoadMapWithWorld.AddSP(this, &FSplash::OnPostLoadMap);
			}
		}
		else
		{
			if (PreLoadLevelDelegate.IsValid())
			{
				FCoreUObjectDelegates::PreLoadMap.Remove(PreLoadLevelDelegate);
				PreLoadLevelDelegate.Reset();
			}
			if (PostLoadLevelDelegate.IsValid())
			{
				FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadLevelDelegate);
				PostLoadLevelDelegate.Reset();
			}
		}
	}

	void FSplash::OnPreLoadMap(const FString&)
	{
		DoShow();
	}

	void FSplash::OnPostLoadMap(UWorld* LoadedWorld)
	{
		// Don't auto-hide splash if show loading screen is called explicitly
		if (!bShouldShowSplash)
		{
			UE_LOG(LogHMD, Log, TEXT("FSplash::OnPostLoadMap Hide Auto Splash"));
			HideLoadingScreen();
		}
	}

#if WITH_EDITOR
	void FSplash::OnPieBegin(bool bIsSimulating)
	{
		LoadSettings();
	}
#endif

	void FSplash::Startup()
	{
		CheckInGameThread();

		if (!bInitialized)
		{
			Settings = OculusXRHMD->CreateNewSettings();
			Frame = OculusXRHMD->CreateNewGameFrame();
			// keep units in meters rather than UU (because UU make not much sense).
			Frame->WorldToMetersScale = 1.0f;

			float SystemDisplayFrequency;
			if (OVRP_SUCCESS(FOculusXRHMDModule::GetPluginWrapper().GetSystemDisplayFrequency2(&SystemDisplayFrequency)))
			{
				SystemDisplayInterval = 1.0f / SystemDisplayFrequency;
			}

			LoadSettings();

			OculusXRHMD->InitDevice();

#if WITH_EDITOR
			PieBeginDelegateHandle = FEditorDelegates::BeginPIE.AddRaw(this, &FSplash::OnPieBegin);
#else
			UOculusXRHMDRuntimeSettings* HMDSettings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();
			check(HMDSettings);
			if (HMDSettings->bAutoEnabled)
			{
				UE_LOG(LogHMD, Log, TEXT("FSplash::Startup Show Splash on Startup"));
				DoShow();
			}
#endif

			OculusXRHMD->Settings_RenderThread = OculusXRHMD->Settings->Clone();

			bInitialized = true;
		}
	}

	void FSplash::StopTicker()
	{
		CheckInGameThread();

		if (!bIsShown)
		{
			ExecuteOnRenderThread([this]() {
				if (Ticker.IsValid())
				{
					Ticker->Unregister();
					Ticker = nullptr;
				}
			});
			UnloadTextures();
		}
	}

	void FSplash::StartTicker()
	{
		CheckInGameThread();

		if (!Ticker.IsValid())
		{
			Ticker = MakeShareable(new FTicker(this));

			ExecuteOnRenderThread([this]() {
				LastTimeInSeconds = FPlatformTime::Seconds();
				Ticker->Register();
			});
		}
	}

	void FSplash::RenderFrame_RenderThread(FRHICommandListImmediate& RHICmdList)
	{
		CheckInRenderThread();

		FScopeLock ScopeLock(&RenderThreadLock);

		// RenderFrame
		FSettingsPtr XSettings = Settings->Clone();
		FGameFramePtr XFrame = Frame->Clone();
		XFrame->FrameNumber = OculusXRHMD->NextFrameNumber;
		XFrame->ShowFlags.Rendering = true;
		TArray<FLayerPtr> XLayers = Layers_RenderThread_Input;

		ensure(XLayers.Num() != 0);

		ovrpResult Result;
		if (FOculusXRHMDModule::GetPluginWrapper().GetInitialized() && OculusXRHMD->WaitFrameNumber != XFrame->FrameNumber)
		{
			UE_LOG(LogHMD, Verbose, TEXT("Splash FOculusXRHMDModule::GetPluginWrapper().WaitToBeginFrame %u"), XFrame->FrameNumber);
			if (OVRP_FAILURE(Result = FOculusXRHMDModule::GetPluginWrapper().WaitToBeginFrame(XFrame->FrameNumber)))
			{
				UE_LOG(LogHMD, Error, TEXT("Splash FOculusXRHMDModule::GetPluginWrapper().WaitToBeginFrame %u failed (%d)"), XFrame->FrameNumber, Result);
				XFrame->ShowFlags.Rendering = false;
			}
			else
			{
				OculusXRHMD->WaitFrameNumber = XFrame->FrameNumber;
				OculusXRHMD->NextFrameNumber = XFrame->FrameNumber + 1;
				FPlatformAtomics::InterlockedIncrement(&FramesOutstanding);
			}
		}
		else
		{
			XFrame->ShowFlags.Rendering = false;
		}

		if (XFrame->ShowFlags.Rendering)
		{
			if (OVRP_FAILURE(Result = FOculusXRHMDModule::GetPluginWrapper().Update3(ovrpStep_Render, XFrame->FrameNumber, 0.0)))
			{
				UE_LOG(LogHMD, Error, TEXT("Splash FOculusXRHMDModule::GetPluginWrapper().Update3 %u failed (%d)"), XFrame->FrameNumber, Result);
			}
		}

		{
			int32 LayerIndex = 0;
			int32 LayerIndex_RenderThread = 0;

			while (LayerIndex < XLayers.Num() && LayerIndex_RenderThread < Layers_RenderThread.Num())
			{
				uint32 LayerIdA = XLayers[LayerIndex]->GetId();
				uint32 LayerIdB = Layers_RenderThread[LayerIndex_RenderThread]->GetId();

				if (LayerIdA < LayerIdB)
				{
					XLayers[LayerIndex++]->Initialize_RenderThread(XSettings.Get(), CustomPresent, &OculusXRHMD->DeferredDeletion, RHICmdList);
				}
				else if (LayerIdA > LayerIdB)
				{
					OculusXRHMD->DeferredDeletion.AddLayerToDeferredDeletionQueue(Layers_RenderThread[LayerIndex_RenderThread++]);
				}
				else
				{
					XLayers[LayerIndex++]->Initialize_RenderThread(XSettings.Get(), CustomPresent, &OculusXRHMD->DeferredDeletion, RHICmdList, Layers_RenderThread[LayerIndex_RenderThread++].Get());
				}
			}

			while (LayerIndex < XLayers.Num())
			{
				XLayers[LayerIndex++]->Initialize_RenderThread(XSettings.Get(), CustomPresent, &OculusXRHMD->DeferredDeletion, RHICmdList);
			}

			while (LayerIndex_RenderThread < Layers_RenderThread.Num())
			{
				OculusXRHMD->DeferredDeletion.AddLayerToDeferredDeletionQueue(Layers_RenderThread[LayerIndex_RenderThread++]);
			}
		}

		Layers_RenderThread = XLayers;

		for (int32 LayerIndex = 0; LayerIndex < Layers_RenderThread.Num(); LayerIndex++)
		{
			Layers_RenderThread[LayerIndex]->UpdateTexture_RenderThread(XSettings.Get(), CustomPresent, RHICmdList);
		}

		// This submit is required since splash happens before the game is rendering, so layers won't be submitted with game render commands
		CustomPresent->SubmitGPUCommands_RenderThread(RHICmdList);

		// RHIFrame
		for (int32 LayerIndex = 0; LayerIndex < XLayers.Num(); LayerIndex++)
		{
			XLayers[LayerIndex] = XLayers[LayerIndex]->Clone();
		}

		ExecuteOnRHIThread_DoNotWait([this, XSettings, XFrame, XLayers]() {
			ovrpResult ResultT;

			if (XFrame->ShowFlags.Rendering)
			{
				UE_LOG(LogHMD, Verbose, TEXT("Splash FOculusXRHMDModule::GetPluginWrapper().BeginFrame4 %u"), XFrame->FrameNumber);
				if (OVRP_FAILURE(ResultT = FOculusXRHMDModule::GetPluginWrapper().BeginFrame4(XFrame->FrameNumber, CustomPresent->GetOvrpCommandQueue())))
				{
					UE_LOG(LogHMD, Error, TEXT("Splash FOculusXRHMDModule::GetPluginWrapper().BeginFrame4 %u failed (%d)"), XFrame->FrameNumber, ResultT);
					XFrame->ShowFlags.Rendering = false;
				}
			}

			FPlatformAtomics::InterlockedDecrement(&FramesOutstanding);

			Layers_RHIThread = XLayers;
			Layers_RHIThread.Sort(FLayerPtr_ComparePriority());

			if (XFrame->ShowFlags.Rendering)
			{
				TArray<const ovrpLayerSubmit*> LayerSubmitPtr;
				LayerSubmitPtr.SetNum(Layers_RHIThread.Num());

				for (int32 LayerIndex = 0; LayerIndex < Layers_RHIThread.Num(); LayerIndex++)
				{
					LayerSubmitPtr[LayerIndex] = Layers_RHIThread[LayerIndex]->UpdateLayer_RHIThread(XSettings.Get(), XFrame.Get(), LayerIndex);
				}

				UE_LOG(LogHMD, Verbose, TEXT("Splash FOculusXRHMDModule::GetPluginWrapper().EndFrame4 %u"), XFrame->FrameNumber);
				if (OVRP_FAILURE(ResultT = FOculusXRHMDModule::GetPluginWrapper().EndFrame4(XFrame->FrameNumber, LayerSubmitPtr.GetData(), LayerSubmitPtr.Num(), CustomPresent->GetOvrpCommandQueue())))
				{
					UE_LOG(LogHMD, Error, TEXT("Splash FOculusXRHMDModule::GetPluginWrapper().EndFrame4 %u failed (%d)"), XFrame->FrameNumber, ResultT);
				}
				else
				{
					for (int32 LayerIndex = 0; LayerIndex < Layers_RHIThread.Num(); LayerIndex++)
					{
						Layers_RHIThread[LayerIndex]->IncrementSwapChainIndex_RHIThread(CustomPresent);
					}
				}
			}
		});

		// Splash screen render loop sometimes doesn't trigger RHICmdList flush
		CustomPresent->SubmitGPUCommands_RenderThread(FRHICommandListExecutor::GetImmediateCommandList());
	}

	void FSplash::ReleaseResources_RHIThread()
	{
		for (int32 LayerIndex = 0; LayerIndex < Layers_RenderThread.Num(); LayerIndex++)
		{
			Layers_RenderThread[LayerIndex]->ReleaseResources_RHIThread();
		}

		for (int32 LayerIndex = 0; LayerIndex < Layers_RHIThread.Num(); LayerIndex++)
		{
			Layers_RHIThread[LayerIndex]->ReleaseResources_RHIThread();
		}

		Layers_RenderThread.Reset();
		Layers_RHIThread.Reset();
	}

	void FSplash::PreShutdown()
	{
		CheckInGameThread();
	}

	void FSplash::Shutdown()
	{
		CheckInGameThread();

#if WITH_EDITOR
		if (PieBeginDelegateHandle.IsValid())
		{
			FEditorDelegates::BeginPIE.Remove(PieBeginDelegateHandle);
			PieBeginDelegateHandle.Reset();
		}
#endif

		if (PreLoadLevelDelegate.IsValid())
		{
			FCoreUObjectDelegates::PreLoadMap.Remove(PreLoadLevelDelegate);
			PreLoadLevelDelegate.Reset();
		}
		if (PostLoadLevelDelegate.IsValid())
		{
			FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadLevelDelegate);
			PostLoadLevelDelegate.Reset();
		}

		if (bInitialized)
		{
			ExecuteOnRenderThread([this]() {
				if (Ticker)
				{
					Ticker->Unregister();
					Ticker = nullptr;
				}

				ExecuteOnRHIThread([this]() {
					SplashLayers.Reset();
					Layers_RenderThread.Reset();
					Layers_RenderThread_Input.Reset();
					Layers_RHIThread.Reset();
				});
			});

			bInitialized = false;
		}
	}

	int FSplash::AddSplash(const FOculusXRSplashDesc& Desc)
	{
		CheckInGameThread();

		FScopeLock ScopeLock(&RenderThreadLock);
		return SplashLayers.Add(FSplashLayer(Desc));
	}

	void FSplash::AddSplash(const FSplashDesc& Splash)
	{
		FOculusXRSplashDesc OculusDesc;
		OculusDesc.TransformInMeters = Splash.Transform;
		OculusDesc.QuadSizeInMeters = Splash.QuadSize;
		OculusDesc.DeltaRotation = Splash.DeltaRotation;
		OculusDesc.bNoAlphaChannel = Splash.bIgnoreAlpha;
		OculusDesc.bIsDynamic = Splash.bIsDynamic || Splash.bIsExternal;
		OculusDesc.TextureOffset = Splash.UVRect.Min;
		OculusDesc.TextureScale = Splash.UVRect.Max;
		OculusDesc.LoadedTexture = Splash.TextureObj->GetResource()->GetTextureReference();

		AddSplash(OculusDesc);
	}

	void FSplash::ClearSplashes()
	{
		CheckInGameThread();

		FScopeLock ScopeLock(&RenderThreadLock);
		SplashLayers.Reset();
	}

	bool FSplash::GetSplash(unsigned InSplashLayerIndex, FOculusXRSplashDesc& OutDesc)
	{
		CheckInGameThread();

		FScopeLock ScopeLock(&RenderThreadLock);
		if (InSplashLayerIndex < unsigned(SplashLayers.Num()))
		{
			OutDesc = SplashLayers[int32(InSplashLayerIndex)].Desc;
			return true;
		}
		return false;
	}

	IStereoLayers::FLayerDesc FSplash::StereoLayerDescFromOculusSplashDesc(FOculusXRSplashDesc OculusDesc)
	{
		IStereoLayers::FLayerDesc LayerDesc;
		if (OculusDesc.LoadedTexture->GetTextureCube() != nullptr)
		{
			LayerDesc.SetShape<FCubemapLayer>();
		}
		// else LayerDesc.Shape defaults to FQuadLayer

		LayerDesc.Transform = OculusDesc.TransformInMeters * FTransform(OculusXRHMD->GetSplashRotation().Quaternion());
		LayerDesc.QuadSize = OculusDesc.QuadSizeInMeters;
		LayerDesc.UVRect = FBox2D(OculusDesc.TextureOffset, OculusDesc.TextureOffset + OculusDesc.TextureScale);
		LayerDesc.Priority = INT32_MAX - (int32)(OculusDesc.TransformInMeters.GetTranslation().X * 1000.f);
		LayerDesc.PositionType = IStereoLayers::TrackerLocked;
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		LayerDesc.Texture = OculusDesc.LoadedTexture;
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
		LayerDesc.Flags = IStereoLayers::LAYER_FLAG_QUAD_PRESERVE_TEX_RATIO | (OculusDesc.bNoAlphaChannel ? IStereoLayers::LAYER_FLAG_TEX_NO_ALPHA_CHANNEL : 0) | (OculusDesc.bIsDynamic ? IStereoLayers::LAYER_FLAG_TEX_CONTINUOUS_UPDATE : 0);

		return LayerDesc;
	}

	void FSplash::DoShow()
	{
		CheckInGameThread();

		OculusXRHMD->SetSplashRotationToForward();

		// Create new textures
		UnloadTextures();

		// Make sure all UTextures are loaded and contain Resource->TextureRHI
		bool bWaitForRT = false;

		for (int32 SplashLayerIndex = 0; SplashLayerIndex < SplashLayers.Num(); ++SplashLayerIndex)
		{
			FSplashLayer& SplashLayer = SplashLayers[SplashLayerIndex];

			if (SplashLayer.Desc.TexturePath.IsValid())
			{
				// load temporary texture (if TexturePath was specified)
				LoadTexture(SplashLayer);
			}
			if (SplashLayer.Desc.LoadingTexture && SplashLayer.Desc.LoadingTexture->IsValidLowLevel())
			{
				SplashLayer.Desc.LoadingTexture->UpdateResource();
				bWaitForRT = true;
			}
		}

		FlushRenderingCommands();

		for (int32 SplashLayerIndex = 0; SplashLayerIndex < SplashLayers.Num(); ++SplashLayerIndex)
		{
			FSplashLayer& SplashLayer = SplashLayers[SplashLayerIndex];

			//@DBG BEGIN
			if (SplashLayer.Desc.LoadingTexture && SplashLayer.Desc.LoadingTexture->IsValidLowLevel())
			{
				if (SplashLayer.Desc.LoadingTexture->GetResource() && SplashLayer.Desc.LoadingTexture->GetResource()->TextureRHI)
				{
					SplashLayer.Desc.LoadedTexture = SplashLayer.Desc.LoadingTexture->GetResource()->TextureRHI;
				}
				else
				{
					UE_LOG(LogHMD, Warning, TEXT("Splash, %s - no Resource"), *SplashLayer.Desc.LoadingTexture->GetDesc());
				}
			}
			//@DBG END

			if (SplashLayer.Desc.LoadedTexture.IsValid())
			{
				SplashLayer.Layer = MakeShareable(new FLayer(NextLayerId++));
				SplashLayer.Layer->SetDesc(StereoLayerDescFromOculusSplashDesc(SplashLayer.Desc));
			}
		}

		{
			// add oculus-generated layers through the OculusVR settings area
			FScopeLock ScopeLock(&RenderThreadLock);
			Layers_RenderThread_DeltaRotation.Reset();
			Layers_RenderThread_Input.Reset();
			for (int32 SplashLayerIndex = 0; SplashLayerIndex < SplashLayers.Num(); SplashLayerIndex++)
			{
				const FSplashLayer& SplashLayer = SplashLayers[SplashLayerIndex];

				if (SplashLayer.Layer.IsValid())
				{
					FLayerPtr ClonedLayer = SplashLayer.Layer->Clone();
					Layers_RenderThread_Input.Add(ClonedLayer);

					// Register layers that need to be rotated every n ticks
					if (!SplashLayer.Desc.DeltaRotation.Equals(FQuat::Identity))
					{
						Layers_RenderThread_DeltaRotation.Emplace(ClonedLayer, SplashLayer.Desc.DeltaRotation);
					}
				}
			}

			// add UE VR splash screen
			FOculusXRSplashDesc UESplashDesc = OculusXRHMD->GetUESplashScreenDesc();
			if (UESplashDesc.LoadedTexture != nullptr)
			{
				UELayer.Reset();
				UELayer = MakeShareable(new FLayer(NextLayerId++));
				UELayer->SetDesc(StereoLayerDescFromOculusSplashDesc(UESplashDesc));
				Layers_RenderThread_Input.Add(UELayer->Clone());
			}

			Layers_RenderThread_Input.Sort(FLayerPtr_CompareId());
		}

		if (Layers_RenderThread_Input.Num() > 0)
		{
			// If no textures are loaded, this will push black frame
			StartTicker();
			bIsShown = true;
			UE_LOG(LogHMD, Log, TEXT("FSplash::DoShow"));
		}
		else
		{
			UE_LOG(LogHMD, Log, TEXT("No splash layers in FSplash::DoShow"));
		}
	}

	void FSplash::DoHide()
	{
		CheckInGameThread();

		UE_LOG(LogHMD, Log, TEXT("FSplash::DoHide"));
		bIsShown = false;

		StopTicker();
	}

	void FSplash::UpdateLoadingScreen_GameThread()
	{
		if (bNeedSplashUpdate)
		{
			if (bShouldShowSplash)
			{
				DoShow();
			}
			else
			{
				DoHide();
			}

			bNeedSplashUpdate = false;
		}
	}

	void FSplash::ShowLoadingScreen()
	{
		bShouldShowSplash = true;

		// DoShow will be called from UpdateSplashScreen_Gamethread().
		// This can can happen if the splashes are already being shown, as it will reset the relative positions and delta rotations of the layers.
		bNeedSplashUpdate = true;
	}

	void FSplash::HideLoadingScreen()
	{
		bShouldShowSplash = false;
		bNeedSplashUpdate = bIsShown; // no need to call DoHide when the splash is already hidden
	}

	void FSplash::UnloadTextures()
	{
		CheckInGameThread();

		// unload temporary loaded textures
		FScopeLock ScopeLock(&RenderThreadLock);
		for (int32 SplashLayerIndex = 0; SplashLayerIndex < SplashLayers.Num(); ++SplashLayerIndex)
		{
			if (SplashLayers[SplashLayerIndex].Desc.TexturePath.IsValid())
			{
				UnloadTexture(SplashLayers[SplashLayerIndex]);
			}
		}
	}

	void FSplash::LoadTexture(FSplashLayer& InSplashLayer)
	{
		CheckInGameThread();

		UnloadTexture(InSplashLayer);

		UE_LOG(LogLoadingSplash, Log, TEXT("Loading texture for splash %s..."), *InSplashLayer.Desc.TexturePath.GetAssetName());
		InSplashLayer.Desc.LoadingTexture = Cast<UTexture>(InSplashLayer.Desc.TexturePath.TryLoad());
		if (InSplashLayer.Desc.LoadingTexture != nullptr)
		{
			UE_LOG(LogLoadingSplash, Log, TEXT("...Success. "));
		}
		InSplashLayer.Desc.LoadedTexture = nullptr;
		InSplashLayer.Layer.Reset();
	}

	void FSplash::UnloadTexture(FSplashLayer& InSplashLayer)
	{
		CheckInGameThread();

		InSplashLayer.Desc.LoadingTexture = nullptr;
		InSplashLayer.Desc.LoadedTexture = nullptr;
		InSplashLayer.Layer.Reset();
	}

} // namespace OculusXRHMD

#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
