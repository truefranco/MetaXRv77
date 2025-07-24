// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "IOculusXRAnchorsModule.h"
#include "OculusXRAnchors.h"
#include "openxr/OculusXRAnchorsXR.h"

#define LOCTEXT_NAMESPACE "OculusAnchors"

//-------------------------------------------------------------------------------------------------
// FOculusXRAnchorsModule
//-------------------------------------------------------------------------------------------------

#if OCULUS_ANCHORS_SUPPORTED_PLATFORMS

DECLARE_LOG_CATEGORY_EXTERN(LogOculusXRAnchors, Log, All);

typedef TSharedPtr<XRAnchors::FAnchorsXR, ESPMode::ThreadSafe> FAnchorsXRPtr;

class FOculusXRAnchorsModule : public IOculusXRAnchorsModule, IOculusXRCreateAnchorComponent
{
public:
	static inline FOculusXRAnchorsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOculusXRAnchorsModule>("OculusXRAnchors");
	}

	virtual ~FOculusXRAnchorsModule() = default;

	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnPostEngineInit();

	// IOculusXRAnchorsModule
	virtual void AddCreateAnchorComponentInterface(IOculusXRCreateAnchorComponent* CastInterface) override;
	virtual void RemoveCreateAnchorComponentInterface(IOculusXRCreateAnchorComponent* CastInterface) override;
	virtual UOculusXRBaseAnchorComponent* CreateAnchorComponent(uint64 AnchorHandle, EOculusXRSpaceComponentType Type, UObject* Outer) override;

	// IOculusXRAnchorComponentCaster
	virtual UOculusXRBaseAnchorComponent* TryCreateAnchorComponent(uint64 AnchorHandle, EOculusXRSpaceComponentType Type, UObject* Outer) override;

	static OculusXRAnchors::FOculusXRAnchors* GetOculusAnchors();

	FAnchorsXRPtr GetXrAnchors() { return AnchorsXR; }

private:
	TArray<IOculusXRCreateAnchorComponent*> CreateComponentInterfaces;
	OculusXRAnchors::FOculusXRAnchors Anchors;
	FAnchorsXRPtr AnchorsXR;
};

#else // OCULUS_ANCHORS_SUPPORTED_PLATFORMS

class FOculusXRAnchorsModule : public FDefaultModuleImpl
{
};

#endif // OCULUS_ANCHORS_SUPPORTED_PLATFORMS

#undef LOCTEXT_NAMESPACE
