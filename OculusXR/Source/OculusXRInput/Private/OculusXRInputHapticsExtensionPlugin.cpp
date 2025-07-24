// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRInputHapticsExtensionPlugin.h"

#include "IOpenXRHMDModule.h"
#include "OculusXRInputExtensionPlugin.h"
#include "OculusXRInputXRFunctions.h"
#include "OpenXRCore.h"

namespace OculusXRInput
{

	XrInstance FInputHapticsExtensionPlugin::GetOpenXRInstance() const
	{
		return Instance;
	}

	XrSession FInputHapticsExtensionPlugin::GetOpenXRSession() const
	{
		return Session;
	}

	bool FInputHapticsExtensionPlugin::IsPCMExtensionAvailable() const
	{
		return bExtFBHapticsPcmAvailable;
	}

	bool FInputHapticsExtensionPlugin::IsAmplitudeEnvelopeExtensionAvailable() const
	{
		return bExtFBAmplitudeEnvelopeAvailable;
	}

	bool FInputHapticsExtensionPlugin::IsTouchControllerProExtensionAvailable() const
	{
		return bExtFBTouchControllerProAvailable;
	}

	XrAction FInputHapticsExtensionPlugin::GetXrHandHapticVibrationAction() const
	{
		return XrHandHapticVibrationAction;
	}

	XrAction FInputHapticsExtensionPlugin::GetXrThumbHapticVibrationAction() const
	{
		return XrThumbHapticVibrationAction;
	}

	XrAction FInputHapticsExtensionPlugin::GetXrIndexHapticVibrationAction() const
	{
		return XrIndexHapticVibrationAction;
	}

	XrPath* FInputHapticsExtensionPlugin::GetXrHandsSubactionPaths()
	{
		return XrPathBothHands;
	}

	XrPath* FInputHapticsExtensionPlugin::GetXrHandsHapticsSubactionPaths()
	{
		return XrPathBothHandsHaptics;
	}

	XrPath* FInputHapticsExtensionPlugin::GetXrThumbsHapticsSubactionPaths()
	{
		return XrPathBothThumbsHaptics;
	}

	XrPath* FInputHapticsExtensionPlugin::GetXrIndexesHapticsSubactionPaths()
	{
		return XrPathBothIndexesHaptics;
	}

	bool FInputHapticsExtensionPlugin::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_FB_HAPTIC_PCM_EXTENSION_NAME);
		OutExtensions.Add(XR_FB_HAPTIC_AMPLITUDE_ENVELOPE_EXTENSION_NAME);
		OutExtensions.Add(XR_FB_TOUCH_CONTROLLER_PRO_EXTENSION_NAME);
		return true;
	}

	const void* FInputHapticsExtensionPlugin::OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext)
	{
		bExtFBHapticsPcmAvailable = InModule->IsExtensionEnabled(XR_FB_HAPTIC_PCM_EXTENSION_NAME);
		bExtFBAmplitudeEnvelopeAvailable = InModule->IsExtensionEnabled(XR_FB_HAPTIC_AMPLITUDE_ENVELOPE_EXTENSION_NAME);
		bExtFBTouchControllerProAvailable = InModule->IsExtensionEnabled(XR_FB_TOUCH_CONTROLLER_PRO_EXTENSION_NAME);
		return InNext;
	}

	void FInputHapticsExtensionPlugin::PostCreateInstance(XrInstance InInstance)
	{
		Instance = InInstance;
		InitOpenXRFunctions(Instance);
	}

	void FInputHapticsExtensionPlugin::PostCreateSession(XrSession InSession)
	{
		Session = InSession;
	}

	void FInputHapticsExtensionPlugin::CreateHapticActions()
	{
		// Create action set
		HapticsActionSet = XR_NULL_HANDLE;
		XrActionSetCreateInfo ActionSetInfo{ XR_TYPE_ACTION_SET_CREATE_INFO };
		ActionSetInfo.next = nullptr;
		FCStringAnsi::Strcpy(ActionSetInfo.actionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "oculushapticsactionset");
		FCStringAnsi::Strcpy(ActionSetInfo.localizedActionSetName, XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE, "OculusHapticsActionSet");
		XR_ENSURE(xrCreateActionSet(Instance, &ActionSetInfo, &HapticsActionSet));

		// Create hand haptics paths
		XR_ENSURE(xrStringToPath(Instance, "/user/hand/left", &XrPathLeftHand));
		XR_ENSURE(xrStringToPath(Instance, "/user/hand/left/output/haptic", &XrPathLeftHandHaptics));
		XR_ENSURE(xrStringToPath(Instance, "/user/hand/right", &XrPathRightHand));
		XR_ENSURE(xrStringToPath(Instance, "/user/hand/right/output/haptic", &XrPathRightHandHaptics));
		XrPathBothHands[0] = XrPathLeftHand;
		XrPathBothHands[1] = XrPathRightHand;
		XrPathBothHandsHaptics[0] = XrPathLeftHandHaptics;
		XrPathBothHandsHaptics[1] = XrPathRightHandHaptics;

		// Create localized haptics paths
		XR_ENSURE(xrStringToPath(Instance, "/interaction_profiles/facebook/touch_controller_pro", &XrQuestProInteractionProfile));
		XR_ENSURE(xrStringToPath(Instance, "/user/hand/left/output/thumb_haptic_fb", &XrPathLeftThumbHaptics));
		XR_ENSURE(xrStringToPath(Instance, "/user/hand/left/output/trigger_haptic_fb", &XrPathLeftIndexHaptics));
		XR_ENSURE(xrStringToPath(Instance, "/user/hand/right/output/thumb_haptic_fb", &XrPathRightThumbHaptics));
		XR_ENSURE(xrStringToPath(Instance, "/user/hand/right/output/trigger_haptic_fb", &XrPathRightIndexHaptics));

		XrPathBothThumbsHaptics[0] = XrPathLeftThumbHaptics;
		XrPathBothThumbsHaptics[1] = XrPathRightThumbHaptics;
		XrPathBothIndexesHaptics[0] = XrPathLeftIndexHaptics;
		XrPathBothIndexesHaptics[1] = XrPathRightIndexHaptics;

		// Create actions
		const auto CreateVibrationOutputAction = [this](const char* ActionName) {
			XrActionCreateInfo ActionCreateInfo{ XR_TYPE_ACTION_CREATE_INFO };
			ActionCreateInfo.next = nullptr;
			ActionCreateInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;

			FCStringAnsi::Strcpy(ActionCreateInfo.actionName, XR_MAX_ACTION_NAME_SIZE, ActionName);
			FCStringAnsi::Strcpy(ActionCreateInfo.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, ActionName);

			ActionCreateInfo.countSubactionPaths = sizeof(XrPathBothHands) / sizeof(XrPath);
			ActionCreateInfo.subactionPaths = XrPathBothHands;
			XrAction Action = XR_NULL_HANDLE;
			XR_ENSURE(xrCreateAction(HapticsActionSet, &ActionCreateInfo, &Action));
			return Action;
		};

		XrHandHapticVibrationAction = CreateVibrationOutputAction("hand_haptic_vibration");
		XrThumbHapticVibrationAction = CreateVibrationOutputAction("hand_thumb_haptic_vibration");
		XrIndexHapticVibrationAction = CreateVibrationOutputAction("hand_index_haptic_vibration");
	}

	bool FInputHapticsExtensionPlugin::GetSuggestedBindings(XrPath InInteractionProfile, TArray<XrActionSuggestedBinding>& OutBindings)
	{
		if (HapticsActionSet == XR_NULL_HANDLE)
		{
			return false;
		}

		OutBindings.Add({ XrHandHapticVibrationAction, XrPathLeftHandHaptics });
		OutBindings.Add({ XrHandHapticVibrationAction, XrPathRightHandHaptics });
		if (InInteractionProfile == XrQuestProInteractionProfile)
		{
			OutBindings.Add({ XrIndexHapticVibrationAction, XrPathLeftIndexHaptics });
			OutBindings.Add({ XrIndexHapticVibrationAction, XrPathRightIndexHaptics });
			OutBindings.Add({ XrThumbHapticVibrationAction, XrPathLeftThumbHaptics });
			OutBindings.Add({ XrThumbHapticVibrationAction, XrPathRightThumbHaptics });
		}

		return true;
	}

	void FInputHapticsExtensionPlugin::AttachActionSets(TSet<XrActionSet>& OutActionSets)
	{
		if (HapticsActionSet != XR_NULL_PATH)
		{
			OutActionSets.Add(HapticsActionSet);
		}
	}

	void FInputHapticsExtensionPlugin::GetActiveActionSetsForSync(
		TArray<XrActiveActionSet>& OutActiveSets)
	{
		if (HapticsActionSet != XR_NULL_PATH)
		{
			OutActiveSets.Add({ HapticsActionSet, XR_NULL_PATH });
		}
	}

	bool FInputHapticsExtensionPlugin::GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath,
		bool& OutHasHaptics)
	{
		// Called at the start of Epic's input action creation
		if (HapticsActionSet != XR_NULL_HANDLE)
		{
			DestroyHapticActions();
		}
		CreateHapticActions();
		return true;
	}

	void FInputHapticsExtensionPlugin::DestroyHapticActions()
	{
		if (HapticsActionSet != XR_NULL_HANDLE)
		{
			xrDestroyActionSet(HapticsActionSet);
			XrHandHapticVibrationAction = XR_NULL_HANDLE;
			XrThumbHapticVibrationAction = XR_NULL_HANDLE;
			XrIndexHapticVibrationAction = XR_NULL_HANDLE;
			HapticsActionSet = XR_NULL_HANDLE;
		}
	}

} // namespace OculusXRInput
