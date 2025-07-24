// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRTouchProInputExtensionPlugin.h"
#include "OpenXRCore.h"

namespace OculusXRInput
{

	bool FTouchProInputExtensionPlugin::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_FB_TOUCH_CONTROLLER_PRO_EXTENSION_NAME);
		return true;
	}

	void FTouchProInputExtensionPlugin::PostCreateInstance(XrInstance InInstance)
	{
		XrResult Result = xrStringToPath(InInstance, "/interaction_profiles/facebook/touch_controller_pro", &InteractionProfile);
		check(XR_SUCCEEDED(Result));
	}

	bool FTouchProInputExtensionPlugin::GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics)
	{
		OutKeyPrefix = "OculusTouchPro";
		OutPath = InteractionProfile;
		OutHasHaptics = true;
		return true;
	}

} // namespace OculusXRInput
