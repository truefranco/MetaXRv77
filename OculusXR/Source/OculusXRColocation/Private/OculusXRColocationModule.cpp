// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRColocationModule.h"
#include "IOculusXRHMDModule.h"

#if OCULUS_HMD_SUPPORTED_PLATFORMS
#include "OculusXRHMDModule.h"
#include "OculusXRHMD.h"
#include "OculusXRColocationEventPolling.h"

DEFINE_LOG_CATEGORY(LogOculusXRColocation);

#define LOCTEXT_NAMESPACE "OculusXRColocation"

//-------------------------------------------------------------------------------------------------
// FOculusXRColocationModule
//-------------------------------------------------------------------------------------------------
void FOculusXRColocationModule::StartupModule()
{
	ColocationXR = MakeShareable(new XRColocation::FColocationXR());
	ColocationXR->RegisterAsOpenXRExtension();

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FOculusXRColocationModule::OnPostEngineInit);
}

void FOculusXRColocationModule::ShutdownModule()
{
}

void FOculusXRColocationModule::OnPostEngineInit()
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
		UE_LOG(LogOculusXRColocation, Warning, TEXT("Unable to retrieve OculusXRHMD, cannot add event polling delegate."));
		return;
	}

	HMD->AddEventPollingDelegate(OculusXRHMD::FOculusXRHMDEventPollingDelegate::CreateStatic(&OculusXRColocation::FColocationEventPolling::OnPollEvent));
}

#endif // OCULUS_HMD_SUPPORTED_PLATFORMS

IMPLEMENT_MODULE(FOculusXRColocationModule, OculusXRColocation)

#undef LOCTEXT_NAMESPACE
