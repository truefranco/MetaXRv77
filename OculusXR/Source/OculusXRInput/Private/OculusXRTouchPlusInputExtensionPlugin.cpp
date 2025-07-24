// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRTouchPlusInputExtensionPlugin.h"
#include "OpenXRCore.h"

namespace OculusXRInput
{

	bool FTouchPlusInputExtensionPlugin::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_META_TOUCH_CONTROLLER_PLUS_EXTENSION_NAME);
		return true;
	}

	void FTouchPlusInputExtensionPlugin::PostCreateInstance(XrInstance InInstance)
	{
		XrResult Result = xrStringToPath(InInstance, "/interaction_profiles/meta/touch_controller_plus", &InteractionProfile);
		check(XR_SUCCEEDED(Result));
	}

	bool FTouchPlusInputExtensionPlugin::GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics)
	{
		OutKeyPrefix = "OculusTouchPlus";
		OutPath = InteractionProfile;
		OutHasHaptics = true;
		return true;
	}

} // namespace OculusXRInput
