// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRInputModule.h"

#if OCULUS_INPUT_SUPPORTED_PLATFORMS
#include "OculusXRInput.h"
#include "OculusXRHMDModule.h"

#define LOCTEXT_NAMESPACE "OculusXRInput"

//-------------------------------------------------------------------------------------------------
// FOculusXRInputModule
//-------------------------------------------------------------------------------------------------

OculusXRInput::FInputExtensionPlugin* FOculusXRInputModule::GetInputOpenXRExtension() const
{
	return InputExtensionPlugin.Get();
}

OculusXRInput::FHandTrackingExtensionPlugin* FOculusXRInputModule::GetHandTrackingOpenXRExtension() const
{
	return HandTrackingExtensionPlugin.Get();
}

OculusXRInput::FInputHapticsExtensionPlugin* FOculusXRInputModule::GetHapticsOpenXRExtension() const
{
	return HapticsExtensionPlugin.Get();
}

void FOculusXRInputModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
	OculusXRInput::FOculusXRInput::PreInit();

	HapticsExtensionPlugin = MakeShareable(new OculusXRInput::FInputHapticsExtensionPlugin());
	HapticsExtensionPlugin->RegisterOpenXRExtensionPlugin();
	HandTrackingExtensionPlugin = MakeShareable(new OculusXRInput::FHandTrackingExtensionPlugin());
	HandTrackingExtensionPlugin->RegisterOpenXRExtensionPlugin();
	InputExtensionPlugin = MakeShareable(new OculusXRInput::FInputExtensionPlugin());
	InputExtensionPlugin->RegisterOpenXRExtensionPlugin();
	TouchProInputExtensionPlugin = MakeShareable(new OculusXRInput::FTouchProInputExtensionPlugin());
	TouchProInputExtensionPlugin->RegisterOpenXRExtensionPlugin();
	TouchPlusInputExtensionPlugin = MakeShareable(new OculusXRInput::FTouchPlusInputExtensionPlugin());
	TouchPlusInputExtensionPlugin->RegisterOpenXRExtensionPlugin();
}

void FOculusXRInputModule::ShutdownModule()
{
	OculusXRInput::FOculusXRInput::ShutdownXRFunctionLibrary();
}

TSharedPtr<class IInputDevice> FOculusXRInputModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	InputExtensionPlugin->SetMessageHandler(InMessageHandler);
	HandTrackingExtensionPlugin->SetMessageHandler(InMessageHandler);
	if (IOculusXRHMDModule::IsAvailable())
	{
		if (FOculusXRHMDModule::Get().PreInit())
		{
			TSharedPtr<OculusXRInput::FOculusXRInput> InputDevice(new OculusXRInput::FOculusXRInput(InMessageHandler));
			OculusXRInputDevice = InputDevice;
			return InputDevice;
		}
		// else, they may just not have a oculus headset plugged in (which we have to account for - no need for a warning)
	}
	else
	{
		UE_LOG(LogOcInput, Warning, TEXT("OculusXRInput plugin enabled, but OculusXRHMD plugin is not available."));
	}
	return nullptr;
}

uint32 FOculusXRInputModule::GetNumberOfTouchControllers() const
{
	if (OculusXRInputDevice.IsValid())
	{
		return OculusXRInputDevice.Pin()->GetNumberOfTouchControllers();
	}
	return 0;
}

uint32 FOculusXRInputModule::GetNumberOfHandControllers() const
{
	if (OculusXRInputDevice.IsValid())
	{
		return OculusXRInputDevice.Pin()->GetNumberOfHandControllers();
	}
	return 0;
}

TSharedPtr<IInputDevice> FOculusXRInputModule::GetInputDevice() const
{
	if (OculusXRInputDevice.IsValid())
	{
		return OculusXRInputDevice.Pin();
	}
	return nullptr;
}

#endif // OCULUS_INPUT_SUPPORTED_PLATFORMS

IMPLEMENT_MODULE(FOculusXRInputModule, OculusXRInput)

#undef LOCTEXT_NAMESPACE
