// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRSceneModule.h"

#if OCULUS_SCENE_SUPPORTED_PLATFORMS
#include "OculusXRHMDModule.h"

#include "OculusXRHMD.h"
#include "OculusXRSceneEventHandling.h"
#include "OculusXRSceneComponents.h"

DEFINE_LOG_CATEGORY(LogOculusXRScene);

#define LOCTEXT_NAMESPACE "OculusXRScene"

//-------------------------------------------------------------------------------------------------
// FOculusXRSceneModule
//-------------------------------------------------------------------------------------------------
void FOculusXRSceneModule::StartupModule()
{
	SceneXR = MakeShareable(new XRScene::FSceneXR());
	SceneXR->RegisterAsOpenXRExtension();

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FOculusXRSceneModule::OnPostEngineInit);

	auto anchorsModule = FModuleManager::GetModulePtr<IOculusXRAnchorsModule>("OculusXRAnchors");
	anchorsModule->AddCreateAnchorComponentInterface(this);
}

void FOculusXRSceneModule::ShutdownModule()
{
	auto anchorsModule = FModuleManager::GetModulePtr<IOculusXRAnchorsModule>("OculusXRAnchors");
	anchorsModule->RemoveCreateAnchorComponentInterface(this);
}

void FOculusXRSceneModule::OnPostEngineInit()
{
	if (IsRunningCommandlet())
	{
		return;
	}

	if (!GEngine)
	{
		return;
	}

	OculusXRHMD::FOculusXRHMD* HMD = OculusXRHMD::FOculusXRHMD::GetOculusXRHMD();
	if (!HMD)
	{
		UE_LOG(LogOculusXRScene, Warning, TEXT("Unable to retrieve OculusXRHMD, cannot add event polling delegates."));
		return;
	}

	HMD->AddEventPollingDelegate(OculusXRHMD::FOculusXRHMDEventPollingDelegate::CreateStatic(&OculusXRScene::FOculusXRSceneEventHandling::OnPollEvent));
}

UOculusXRBaseAnchorComponent* FOculusXRSceneModule::TryCreateAnchorComponent(uint64 AnchorHandle, EOculusXRSpaceComponentType Type, UObject* Outer)
{
	switch (Type)
	{
		case EOculusXRSpaceComponentType::ScenePlane:
			return UOculusXRBaseAnchorComponent::FromSpace<UOculusXRPlaneAnchorComponent>(AnchorHandle, Outer);
		case EOculusXRSpaceComponentType::SceneVolume:
			return UOculusXRBaseAnchorComponent::FromSpace<UOculusXRVolumeAnchorComponent>(AnchorHandle, Outer);
		case EOculusXRSpaceComponentType::SemanticClassification:
			return UOculusXRBaseAnchorComponent::FromSpace<UOculusXRSemanticClassificationAnchorComponent>(AnchorHandle, Outer);
		case EOculusXRSpaceComponentType::RoomLayout:
			return UOculusXRBaseAnchorComponent::FromSpace<UOculusXRRoomLayoutAnchorComponent>(AnchorHandle, Outer);
		case EOculusXRSpaceComponentType::TriangleMesh:
			return UOculusXRBaseAnchorComponent::FromSpace<UOculusXRTriangleMeshAnchorComponent>(AnchorHandle, Outer);
		default:
			return nullptr;
	}
}

#endif // OCULUS_SCENE_SUPPORTED_PLATFORMS

IMPLEMENT_MODULE(FOculusXRSceneModule, OculusXRScene)

#undef LOCTEXT_NAMESPACE
