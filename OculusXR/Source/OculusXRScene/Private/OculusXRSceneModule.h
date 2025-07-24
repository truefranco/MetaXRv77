// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "IOculusXRSceneModule.h"
#include "OculusXRAnchorsModule.h"
#include "openxr/OculusXRSceneXR.h"

#define LOCTEXT_NAMESPACE "OculusXRScene"

//-------------------------------------------------------------------------------------------------
// FOculusXRSceneModule
//-------------------------------------------------------------------------------------------------

DECLARE_LOG_CATEGORY_EXTERN(LogOculusXRScene, Log, All);

typedef TSharedPtr<XRScene::FSceneXR, ESPMode::ThreadSafe> FSceneXRPtr;

#if OCULUS_SCENE_SUPPORTED_PLATFORMS

class FOculusXRSceneModule : public IOculusXRSceneModule, public IOculusXRCreateAnchorComponent
{
public:
	static inline FOculusXRSceneModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOculusXRSceneModule>("OculusXRScene");
	}

	virtual ~FOculusXRSceneModule() = default;

	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnPostEngineInit();

	// IOculusXRCreateAnchorComponent
	virtual UOculusXRBaseAnchorComponent* TryCreateAnchorComponent(uint64 AnchorHandle, EOculusXRSpaceComponentType Type, UObject* Outer) override;

	FSceneXRPtr GetXrScene() { return SceneXR; }

private:
	FSceneXRPtr SceneXR;
};

#else // OCULUS_SCENE_SUPPORTED_PLATFORMS

class FOculusXRSceneModule : public FDefaultModuleImpl
{
};

#endif // OCULUS_SCENE_SUPPORTED_PLATFORMS

#undef LOCTEXT_NAMESPACE
