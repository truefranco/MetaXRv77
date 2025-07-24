// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "IInputDevice.h"
#include "IOculusXRInputModule.h"
#include "OculusXRInputExtensionPlugin.h"
#include "OculusXRInputHapticsExtensionPlugin.h"
#include "OculusXRInputHandTrackingExtensionPlugin.h"
#include "OculusXRTouchPlusInputExtensionPlugin.h"
#include "OculusXRTouchProInputExtensionPlugin.h"
#include "Templates/SharedPointer.h"

#define LOCTEXT_NAMESPACE "OculusXRInput"

//-------------------------------------------------------------------------------------------------
// FOculusXRInputModule
//-------------------------------------------------------------------------------------------------

#if OCULUS_INPUT_SUPPORTED_PLATFORMS

namespace OculusXRInput
{
	class FOculusXRInput;
}

class FOculusXRInputModule : public IOculusXRInputModule
{
public:
	OculusXRInput::FInputExtensionPlugin* GetInputOpenXRExtension() const;
	OculusXRInput::FHandTrackingExtensionPlugin* GetHandTrackingOpenXRExtension() const;
	OculusXRInput::FInputHapticsExtensionPlugin* GetHapticsOpenXRExtension() const;

	TWeakPtr<OculusXRInput::FOculusXRInput> OculusXRInputDevice;

	// IInputDeviceModule overrides
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

	// IOculusXRInputModule overrides
	virtual uint32 GetNumberOfTouchControllers() const override;
	virtual uint32 GetNumberOfHandControllers() const override;
	virtual TSharedPtr<IInputDevice> GetInputDevice() const override;

private:
	typedef TSharedPtr<OculusXRInput::FInputHapticsExtensionPlugin, ESPMode::ThreadSafe> FHapticsExtensionPluginPtr;
	FHapticsExtensionPluginPtr HapticsExtensionPlugin;
	typedef TSharedPtr<OculusXRInput::FHandTrackingExtensionPlugin, ESPMode::ThreadSafe> FHandTrackingExtensionPluginPtr;
	FHandTrackingExtensionPluginPtr HandTrackingExtensionPlugin;
	typedef TSharedPtr<OculusXRInput::FInputExtensionPlugin, ESPMode::ThreadSafe> FInputExtensionPluginPtr;
	FInputExtensionPluginPtr InputExtensionPlugin;
	typedef TSharedPtr<OculusXRInput::FTouchProInputExtensionPlugin, ESPMode::ThreadSafe> FTouchProInputExtensionPluginPtr;
	FTouchProInputExtensionPluginPtr TouchProInputExtensionPlugin;
	typedef TSharedPtr<OculusXRInput::FTouchPlusInputExtensionPlugin, ESPMode::ThreadSafe> FTouchPlusInputExtensionPluginPtr;
	FTouchPlusInputExtensionPluginPtr TouchPlusInputExtensionPlugin;
};

#else //	OCULUS_INPUT_SUPPORTED_PLATFORMS

class FOculusXRInputModule : public FDefaultModuleImpl
{
	virtual uint32 GetNumberOfTouchControllers() const
	{
		return 0;
	};

	virtual uint32 GetNumberOfHandControllers() const
	{
		return 0;
	};
};

#endif // OCULUS_INPUT_SUPPORTED_PLATFORMS

#undef LOCTEXT_NAMESPACE
