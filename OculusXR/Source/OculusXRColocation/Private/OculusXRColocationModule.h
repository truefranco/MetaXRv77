// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IOculusXRHMDModule.h"
#include "IOculusXRColocationModule.h"
#include "openxr/OculusXRColocationXR.h"

#define LOCTEXT_NAMESPACE "OculusColocation"

//-------------------------------------------------------------------------------------------------
// FOculusXRColocationModule
//-------------------------------------------------------------------------------------------------

#if OCULUS_HMD_SUPPORTED_PLATFORMS

DECLARE_LOG_CATEGORY_EXTERN(LogOculusXRColocation, Log, All);

typedef TSharedPtr<XRColocation::FColocationXR, ESPMode::ThreadSafe> FColocationXRPtr;

class FOculusXRColocationModule : public IOculusXRColocationModule
{
public:
	static inline FOculusXRColocationModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOculusXRColocationModule>("OculusXRColocation");
	}

	virtual ~FOculusXRColocationModule() = default;

	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FColocationXRPtr GetXrColocation() { return ColocationXR; }

private:
	void OnPostEngineInit();

	FColocationXRPtr ColocationXR;
};

#else // OCULUS_HMD_SUPPORTED_PLATFORMS

class FOculusXRColocationModule : public FDefaultModuleImpl
{
};

#endif // OCULUS_HMD_SUPPORTED_PLATFORMS

#undef LOCTEXT_NAMESPACE
