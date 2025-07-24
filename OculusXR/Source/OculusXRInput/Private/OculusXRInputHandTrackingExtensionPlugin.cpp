// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRInputHandTrackingExtensionPlugin.h"
#include "IOpenXRHMDModule.h"
#include "OpenXRCore.h"
#include "xr_linear.h"
#include <khronos/openxr/meta_openxr_preview/meta_hand_tracking_microgestures.h>

#include "IOculusXRHMDModule.h"
#include "OculusXRHandTracking.h"
#include "OVR_Plugin_Types.h"

namespace OculusXRInput
{
	namespace
	{
		static FORCEINLINE XrPosef XrPosef_Identity()
		{
			XrPosef result;
			XrPosef_CreateIdentity(&result);
			return result;
		}

		static FORCEINLINE XrQuaternionf XrQuaternionf_Inverse(const XrQuaternionf q)
		{
			XrQuaternionf r;
			XrQuaternionf_Invert(&r, &q);
			return r;
		}

		static FORCEINLINE XrQuaternionf XrQuaternionf_Multiply(const XrQuaternionf a, const XrQuaternionf b)
		{
			XrQuaternionf result;
			XrQuaternionf_Multiply(&result, &b, &a);
			return result;
		}

		static FORCEINLINE XrQuaternionf XrFromToRotation(const XrVector3f from, const XrVector3f to)
		{
			XrQuaternionf result = XrQuaternionf{};

			const float cx = from.y * to.z - from.z * to.y;
			const float cy = from.z * to.x - from.x * to.z;
			const float cz = from.x * to.y - from.y * to.x;
			const float dot = from.x * to.x + from.y * to.y + from.z * to.z;
			const float crossLengthSq = cx * cx + cy * cy + cz * cz;
			const float magnitude = static_cast<float>(sqrt(crossLengthSq + dot * dot));
			const float cw = dot + magnitude;
			if (cw < SMALLEST_NON_DENORMAL)
			{
				const float sx = to.y * to.y + to.z * to.z;
				const float sz = to.x * to.x + to.y * to.y;
				if (sx > sz)
				{
					const float rcpLength = XrRcpSqrt(sx);
					result.x = float(0);
					result.y = to.z * rcpLength;
					result.z = -to.y * rcpLength;
					result.w = float(0);
				}
				else
				{
					const float rcpLength = XrRcpSqrt(sz);
					result.x = to.y * rcpLength;
					result.y = -to.x * rcpLength;
					result.z = float(0);
					result.w = float(0);
				}
				return result;
			}
			const float rcpLength = XrRcpSqrt(crossLengthSq + cw * cw);
			result.x = cx * rcpLength;
			result.y = cy * rcpLength;
			result.z = cz * rcpLength;
			result.w = cw * rcpLength;

			return result;
		}
	} // namespace

	void FHandTrackingExtensionPlugin::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	{
		MessageHandler = InMessageHandler;
	}

	bool FHandTrackingExtensionPlugin::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
	{
		return false;
	}

	void FHandTrackingExtensionPlugin::SendControllerButtonPressed(FName Key, bool IsPressed, FPlatformUserId UserId, FInputDeviceId DeviceId)
	{
		if (IsPressed)
		{
			MessageHandler->OnControllerButtonPressed(Key, UserId, DeviceId, false);
		}
		else
		{
			MessageHandler->OnControllerButtonReleased(Key, UserId, DeviceId, false);
		}
	}

	void FHandTrackingExtensionPlugin::CreateActions()
	{
		if (!bExtMicrogesturesAvailable)
		{
			return;
		}

		// Create action set
		MicrogestureActionSet = XR_NULL_HANDLE;
		XrActionSetCreateInfo ActionSetInfo{ XR_TYPE_ACTION_SET_CREATE_INFO };
		ActionSetInfo.next = nullptr;
		FCStringAnsi::Strncpy(ActionSetInfo.actionSetName, "oculusmicrogestureactionset", XR_MAX_ACTION_SET_NAME_SIZE);
		FCStringAnsi::Strncpy(ActionSetInfo.localizedActionSetName, "OculusMicrogestureActionSet", XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);
		XR_ENSURE(xrCreateActionSet(Instance, &ActionSetInfo, &MicrogestureActionSet));

		// Declare left hand microgesture actions
		MicrogestureActions.Add({ "OculusHand_Left_MicrogestureSwipeLeft", FOculusKey::OculusHand_Left_MicrogestureSwipeLeft, "/user/hand/left/input/swipe_left_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });
		MicrogestureActions.Add({ "OculusHand_Left_MicrogestureSwipeRight", FOculusKey::OculusHand_Left_MicrogestureSwipeRight, "/user/hand/left/input/swipe_right_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });
		MicrogestureActions.Add({ "OculusHand_Left_MicrogestureSwipeForward", FOculusKey::OculusHand_Left_MicrogestureSwipeForward, "/user/hand/left/input/swipe_forward_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });
		MicrogestureActions.Add({ "OculusHand_Left_MicrogestureSwipeBack", FOculusKey::OculusHand_Left_MicrogestureSwipeBack, "/user/hand/left/input/swipe_backward_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });
		MicrogestureActions.Add({ "OculusHand_Left_MicrogestureTapThumb", FOculusKey::OculusHand_Left_MicrogestureTapThumb, "/user/hand/left/input/tap_thumb_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });

		// Declare right hand microgesture actions
		MicrogestureActions.Add({ "OculusHand_Right_MicrogestureSwipeLeft", FOculusKey::OculusHand_Right_MicrogestureSwipeLeft, "/user/hand/right/input/swipe_left_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });
		MicrogestureActions.Add({ "OculusHand_Right_MicrogestureSwipeRight", FOculusKey::OculusHand_Right_MicrogestureSwipeRight, "/user/hand/right/input/swipe_right_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });
		MicrogestureActions.Add({ "OculusHand_Right_MicrogestureSwipeForward", FOculusKey::OculusHand_Right_MicrogestureSwipeForward, "/user/hand/right/input/swipe_forward_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });
		MicrogestureActions.Add({ "OculusHand_Right_MicrogestureSwipeBack", FOculusKey::OculusHand_Right_MicrogestureSwipeBack, "/user/hand/right/input/swipe_backward_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });
		MicrogestureActions.Add({ "OculusHand_Right_MicrogestureTapThumb", FOculusKey::OculusHand_Right_MicrogestureTapThumb, "/user/hand/right/input/tap_thumb_meta/click", XR_NULL_HANDLE, XR_NULL_PATH });

		// Create all microgesture actions
		TArray<XrActionSuggestedBinding> SuggestedBindings;
		for (auto& MicrogestureAction : MicrogestureActions)
		{
			// Initialize action create info
			XR_ENSURE(xrStringToPath(Instance, TCHAR_TO_ANSI(*MicrogestureAction.PathString), &MicrogestureAction.Path));
			XrActionCreateInfo ActionCreateInfo{ XR_TYPE_ACTION_CREATE_INFO };
			ActionCreateInfo.next = nullptr;
			ActionCreateInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
			ActionCreateInfo.countSubactionPaths = 0;
			FCStringAnsi::Strncpy(ActionCreateInfo.actionName, TCHAR_TO_ANSI(*MicrogestureAction.Name.ToLower()), XR_MAX_ACTION_NAME_SIZE);
			FCStringAnsi::Strncpy(ActionCreateInfo.localizedActionName, TCHAR_TO_ANSI(*MicrogestureAction.Name), XR_MAX_LOCALIZED_ACTION_NAME_SIZE);

			// Create the action
			XR_ENSURE(xrCreateAction(MicrogestureActionSet, &ActionCreateInfo, &MicrogestureAction.Action));

			// Add a suggested binding for this action
			SuggestedBindings.Add({ MicrogestureAction.Action, MicrogestureAction.Path });
		}

		// Suggest bindings for all microgesture actions.  Normally Epic's OpenXRInput code would handle this, but only
		// if the user creates a DefaultMappingContext that includes microgesture keys in the bindings.  Because we can't
		// guarantee this is the case (eg, they may be added at runtime), we'll handle the binding ourselves.
		XrInteractionProfileSuggestedBinding InteractionProfile;
		InteractionProfile.type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING;
		InteractionProfile.next = nullptr;
		InteractionProfile.interactionProfile = XrPathHandInteractionProfile;
		InteractionProfile.countSuggestedBindings = SuggestedBindings.Num();
		InteractionProfile.suggestedBindings = SuggestedBindings.GetData();

		XR_ENSURE(xrSuggestInteractionProfileBindings(Instance, &InteractionProfile));
	}

	void FHandTrackingExtensionPlugin::DestroyActions()
	{
		if (MicrogestureActionSet != XR_NULL_HANDLE)
		{
			xrDestroyActionSet(MicrogestureActionSet);
			MicrogestureActions.Empty();
			MicrogestureActionSet = XR_NULL_HANDLE;
		}
	}

	bool FHandTrackingExtensionPlugin::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_EXT_HAND_TRACKING_EXTENSION_NAME);
		OutExtensions.Add(XR_EXT_HAND_INTERACTION_EXTENSION_NAME);
		return true;
	}

	bool FHandTrackingExtensionPlugin::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_FB_HAND_TRACKING_MESH_EXTENSION_NAME);
		OutExtensions.Add(XR_FB_HAND_TRACKING_CAPSULES_EXTENSION_NAME);
		OutExtensions.Add(XR_FB_HAND_TRACKING_AIM_EXTENSION_NAME);
		OutExtensions.Add(XR_META_HAND_TRACKING_MICROGESTURES_EXTENSION_NAME);
		return true;
	}

	void FHandTrackingExtensionPlugin::AttachActionSets(TSet<XrActionSet>& OutActionSets)
	{
		if (MicrogestureActionSet != XR_NULL_HANDLE)
		{
			OutActionSets.Add(MicrogestureActionSet);
		}
	}

	void FHandTrackingExtensionPlugin::GetActiveActionSetsForSync(
		TArray<XrActiveActionSet>& OutActiveSets)
	{
		if (MicrogestureActionSet != XR_NULL_HANDLE)
		{
			OutActiveSets.Add({ MicrogestureActionSet, XR_NULL_PATH });
		}
	}

	const void* FHandTrackingExtensionPlugin::OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext)
	{
		bExtHandTrackingMeshAvailable = InModule->IsExtensionEnabled(XR_FB_HAND_TRACKING_MESH_EXTENSION_NAME);
		bExtMicrogesturesAvailable = InModule->IsExtensionEnabled(XR_META_HAND_TRACKING_MICROGESTURES_EXTENSION_NAME);

		return InNext;
	}

	const void* FHandTrackingExtensionPlugin::OnGetSystem(XrInstance InInstance, const void* InNext)
	{
		// Store extension open xr calls to member function pointers for convenient use.
		XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrGetHandMeshFB", (PFN_xrVoidFunction*)&xrGetHandMeshFB));
		XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrLocateHandJointsEXT", (PFN_xrVoidFunction*)&xrLocateHandJointsEXT));
		XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrCreateHandTrackerEXT", (PFN_xrVoidFunction*)&xrCreateHandTrackerEXT));
		XR_ENSURE(xrGetInstanceProcAddr(InInstance, "xrDestroyHandTrackerEXT", (PFN_xrVoidFunction*)&xrDestroyHandTrackerEXT));

		return InNext;
	}

	void FHandTrackingExtensionPlugin::PostCreateInstance(XrInstance InInstance)
	{
		Instance = InInstance;

		// Create hands interaction profile path
		XR_ENSURE(xrStringToPath(Instance, "/interaction_profiles/ext/hand_interaction_ext", &XrPathHandInteractionProfile));

		// Create Actions
		if (MicrogestureActionSet != XR_NULL_HANDLE)
		{
			DestroyActions();
		}
		CreateActions();
	}

	const void* FHandTrackingExtensionPlugin::OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext)
	{
		XrSystemHandTrackingPropertiesEXT HandTrackingSystemProperties{ XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT };
		XrSystemProperties systemProperties{ XR_TYPE_SYSTEM_PROPERTIES, &HandTrackingSystemProperties };
		XR_ENSURE(xrGetSystemProperties(InInstance, InSystem, &systemProperties));
		bHandTrackingAvailable = HandTrackingSystemProperties.supportsHandTracking == XR_TRUE;

		return InNext;
	}

	const void* FHandTrackingExtensionPlugin::OnBeginSession(XrSession InSession, const void* InNext)
	{
		if (!bHandTrackingAvailable)
		{
			return InNext;
		}

		XrHandTrackerEXT LeftHandTracker{};
		{
			XrHandTrackerCreateInfoEXT CreateInfo{ XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT };
			CreateInfo.hand = XR_HAND_LEFT_EXT;
			CreateInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;
			XR_ENSURE(xrCreateHandTrackerEXT(InSession, &CreateInfo, &LeftHandTracker));
		}
		XrHandTrackerEXT RightHandTracker{};
		{
			XrHandTrackerCreateInfoEXT CreateInfo{ XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT };
			CreateInfo.hand = XR_HAND_RIGHT_EXT;
			CreateInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;
			XR_ENSURE(xrCreateHandTrackerEXT(InSession, &CreateInfo, &RightHandTracker));
		}
		OculusHandTrackers.Add(EOculusXRHandType::HandLeft, LeftHandTracker);
		OculusHandTrackers.Add(EOculusXRHandType::HandRight, RightHandTracker);
		bIsInitialized = true;
		return InNext;
	}

	void FHandTrackingExtensionPlugin::OnDestroySession(XrSession InSession)
	{
		for (TPair<EOculusXRHandType, XrHandTrackerEXT> Pair : OculusHandTrackers)
		{
			XR_ENSURE(xrDestroyHandTrackerEXT(Pair.Value));
		}
		OculusHandTrackers.Reset();
		bIsInitialized = false;
	}

	bool FHandTrackingExtensionPlugin::IsHandTrackingAvailable()
	{
		return bHandTrackingAvailable && bIsInitialized;
	}

	bool FHandTrackingExtensionPlugin::IsHandTrackingActive()
	{
		return IsHandTrackingAvailable() && bHandTrackingActive;
	}

	void FHandTrackingExtensionPlugin::UpdateDeviceLocations(XrSession InSession, XrTime DisplayTime, XrSpace TrackingSpace)
	{
		if (!IsHandTrackingAvailable())
		{
			return;
		}

		InternalHandsState.PredictedDisplayTime = DisplayTime;
		bool bHandTrackingActiveCurrentTick = false;

		for (int HandIndex = 0; HandIndex < MaxHandCount; HandIndex++)
		{
			const bool bIsLeft = (HandIndex == 0);
			XrHandTrackingScaleFB HandScale = { XR_TYPE_HAND_TRACKING_SCALE_FB };

			XrHandTrackingAimStateFB AimState = { XR_TYPE_HAND_TRACKING_AIM_STATE_FB };
			AimState.next = &HandScale;

			XrHandJointVelocitiesEXT HandJointVelocities = { XR_TYPE_HAND_JOINT_VELOCITIES_EXT };
			HandJointVelocities.next = &AimState;
			HandJointVelocities.jointCount = XR_HAND_JOINT_COUNT_EXT;
			HandJointVelocities.jointVelocities = bIsLeft ? XrLeftHandJointVelocities : XrRightHandJointVelocities;

			XrHandJointLocationsEXT HandJointLocations = { XR_TYPE_HAND_JOINT_LOCATIONS_EXT };
			HandJointLocations.next = &HandJointVelocities;
			HandJointLocations.jointCount = XR_HAND_JOINT_COUNT_EXT;
			HandJointLocations.jointLocations = bIsLeft ? XrLeftHandJointLocations : XrRightHandJointLocations;

			XrHandJointsLocateInfoEXT HandLocateInfo = { XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT };
			HandLocateInfo.baseSpace = TrackingSpace;
			HandLocateInfo.time = InternalHandsState.PredictedDisplayTime;

			XrHandTrackingDataSourceStateEXT DataSourceState{ XR_TYPE_HAND_TRACKING_DATA_SOURCE_STATE_EXT };
			XrHandJointsMotionRangeInfoEXT MotionRangeInfo{ XR_TYPE_HAND_JOINTS_MOTION_RANGE_INFO_EXT };

			XrHandTrackerEXT& HandTracker = bIsLeft ? OculusHandTrackers[EOculusXRHandType::HandLeft] : OculusHandTrackers[EOculusXRHandType::HandRight];
			XR_ENSURE(xrLocateHandJointsEXT(HandTracker, &HandLocateInfo, &HandJointLocations));

			const bool bHandTrackingActivePerHand = (HandJointLocations.isActive != XR_FALSE);
			if (bHandTrackingActivePerHand)
			{
				InternalHandsState.HandState[HandIndex].PosesGeneratedByControllerData = false;
			}
			bHandTrackingActiveCurrentTick |= bHandTrackingActivePerHand;

			const XrSpaceLocationFlags IsValidMask =
				XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
			const XrSpaceLocationFlags IsTrackedMask =
				XR_SPACE_LOCATION_POSITION_TRACKED_BIT | XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
			const XrSpaceLocationFlags IsValidAndTrackedMask = IsValidMask | IsTrackedMask;

			const bool HandIsValid =
				(HandJointLocations.jointLocations[XR_HAND_JOINT_WRIST_EXT].locationFlags & IsValidMask) == IsValidMask;
			const bool HandIsValidAndTracked = (HandJointLocations.jointLocations[XR_HAND_JOINT_WRIST_EXT].locationFlags & IsValidAndTrackedMask) == IsValidAndTrackedMask;
			const bool ThumbIsValidAndTracked = (HandJointLocations.jointLocations[XR_HAND_JOINT_THUMB_TIP_EXT].locationFlags & IsValidAndTrackedMask) == IsValidAndTrackedMask;
			const bool IndexIsValidAndTracked = (HandJointLocations.jointLocations[XR_HAND_JOINT_INDEX_TIP_EXT].locationFlags & IsValidAndTrackedMask) == IsValidAndTrackedMask;
			const bool MiddleIsValidAndTracked = (HandJointLocations.jointLocations[XR_HAND_JOINT_MIDDLE_TIP_EXT].locationFlags & IsValidAndTrackedMask) == IsValidAndTrackedMask;
			const bool RingIsValidAndTracked = (HandJointLocations.jointLocations[XR_HAND_JOINT_RING_TIP_EXT].locationFlags & IsValidAndTrackedMask) == IsValidAndTrackedMask;
			const bool PinkyIsValidAndTracked = (HandJointLocations.jointLocations[XR_HAND_JOINT_LITTLE_TIP_EXT].locationFlags & IsValidAndTrackedMask) == IsValidAndTrackedMask;

			// seems like unreal only uses v1, so skipping v2 path
			InternalHandsState.HandState[HandIndex].Status = 0;

			if (bHandTrackingActivePerHand)
			{
				if (HandIsValid)
				{
					InternalHandsState.HandState[HandIndex].Status |= static_cast<int32>(EHandStatus::HandTracked);
				}
				if (AimState.status & XR_HAND_TRACKING_AIM_VALID_BIT_FB)
				{
					InternalHandsState.HandState[HandIndex].Status |= static_cast<int32>(EHandStatus::InputValid);
				}
				if (AimState.status & XR_HAND_TRACKING_AIM_SYSTEM_GESTURE_BIT_FB)
				{
					InternalHandsState.HandState[HandIndex].Status |= static_cast<int32>(EHandStatus::SystemGestureInProgress);
				}
				if (AimState.status & XR_HAND_TRACKING_AIM_MENU_PRESSED_BIT_FB)
				{
					InternalHandsState.HandState[HandIndex].Status |= static_cast<int32>(EHandStatus::MenuPressed);
				}
			}

			if (AimState.status & XR_HAND_TRACKING_AIM_DOMINANT_HAND_BIT_FB)
			{
				InternalHandsState.HandState[HandIndex].Status |= static_cast<int32>(EHandStatus::DominantHand);
			}

			InternalHandsState.HandState[HandIndex].HandConfidence =
				HandIsValidAndTracked ? EOculusXRTrackingConfidence::High : EOculusXRTrackingConfidence::Low;
			InternalHandsState.HandState[HandIndex].FingerConfidences[static_cast<int32>(EHandFinger::Thumb)] =
				ThumbIsValidAndTracked ? EOculusXRTrackingConfidence::High : EOculusXRTrackingConfidence::Low;
			InternalHandsState.HandState[HandIndex].FingerConfidences[static_cast<int32>(EHandFinger::Index)] =
				IndexIsValidAndTracked ? EOculusXRTrackingConfidence::High : EOculusXRTrackingConfidence::Low;
			InternalHandsState.HandState[HandIndex].FingerConfidences[static_cast<int32>(EHandFinger::Middle)] =
				MiddleIsValidAndTracked ? EOculusXRTrackingConfidence::High : EOculusXRTrackingConfidence::Low;
			InternalHandsState.HandState[HandIndex].FingerConfidences[static_cast<int32>(EHandFinger::Ring)] =
				RingIsValidAndTracked ? EOculusXRTrackingConfidence::High : EOculusXRTrackingConfidence::Low;
			InternalHandsState.HandState[HandIndex].FingerConfidences[static_cast<int32>(EHandFinger::Pinky)] =
				PinkyIsValidAndTracked ? EOculusXRTrackingConfidence::High : EOculusXRTrackingConfidence::Low;

			const FQuat4f RotTip = bIsLeft ? FQuat4f({ 1.0f, 0.0f, 0.0f }, FMath::DegreesToRadians(180.0f)) * FQuat4f({ 0.0f, 1.0f, 0.0f }, FMath::DegreesToRadians(-90.0f))
										   : FQuat4f({ 0.0f, 1.0f, 0.0f }, FMath::DegreesToRadians(90.0f));

			const FQuat4f RotThumb = bIsLeft ? RotTip * FQuat4f({ 0.0f, 0.0f, 1.0f }, FMath::DegreesToRadians(90.0f))
											 : RotTip * FQuat4f({ 0.0f, 0.0f, 1.0f }, FMath::DegreesToRadians(-90.0f));

			const XrQuaternionf InvRotTip = XrQuaternionf_Inverse((XrQuaternionf&)RotTip);
			XrQuaternionf InvRotThumb = XrQuaternionf_Inverse((XrQuaternionf&)RotThumb);

			if (XR_CURRENT_API_VERSION >= XR_MAKE_VERSION(1, 0, 23))
			{
				InvRotThumb = InvRotTip;
			}

			// back out last mile OpenXR spec pose adjustment and convert all bones from app space to wrist bone space
			XrQuaternionf RotWrist = HandJointLocations.jointLocations[XR_HAND_JOINT_WRIST_EXT].pose.orientation;
			RotWrist = XrQuaternionf_Multiply(RotWrist, InvRotTip);

			// vrapi wrist bone was always equal to the identity rot, so any remaining rotation present is the xr space
			// transform
			XrQuaternionf InvRotSpace = XrQuaternionf_Inverse(RotWrist);
			RotWrist = XrQuaternionf_Multiply(InvRotSpace, RotWrist);

			// thumb0 doesn't exist in OpenXR, so we have to do some work to deduce its rotation
			XrPosef WristPose = HandJointLocations.jointLocations[XR_HAND_JOINT_WRIST_EXT].pose;
			WristPose.orientation = XrQuaternionf_Multiply(WristPose.orientation, InvRotTip);

			// parent-space bind poses for thumb0/1
			XrPosef Thumb0BindPoseParentSpace = bIsLeft ? XrLeftHandLegacyBindPoseThumb0 : XrRightHandLegacyBindPoseThumb0;
			XrPosef Thumb1BindPoseParentSpace = bIsLeft ? XrLeftHandLegacyBindPoseThumb1 : XrRightHandLegacyBindPoseThumb1;

			// compute parent-space pose of thumb1
			XrPosef Thumb1PoseWorldSpace = HandJointLocations.jointLocations[XR_HAND_JOINT_THUMB_METACARPAL_EXT].pose;
			XrPosef WristPoseInv;
			XrPosef_Invert(&WristPoseInv, &WristPose);
			XrPosef Thumb1PoseWristSpace{};
			XrPosef_Multiply(&Thumb1PoseWristSpace, &WristPoseInv, &Thumb1PoseWorldSpace);
			XrPosef Thumb0BindPoseParentSpaceInv;
			XrPosef_Invert(&Thumb0BindPoseParentSpaceInv, &Thumb0BindPoseParentSpace);
			XrPosef Thumb1PoseParentSpace{};
			XrPosef_Multiply(&Thumb1PoseParentSpace, &Thumb0BindPoseParentSpaceInv, &Thumb1PoseWristSpace);

			// deduce thumb0 bind space rotation from the change in thumb1 parent-space position
			XrQuaternionf Thumb0BindSpaceRot =
				XrFromToRotation(Thumb1BindPoseParentSpace.position, Thumb1PoseParentSpace.position);

			// final parent-space rotation of thumb0 is its parent-space bind pose combined with its bind space rotation
			XrQuaternionf RotThumb0 = XrQuaternionf_Multiply(Thumb0BindPoseParentSpace.orientation, Thumb0BindSpaceRot);

			XrQuaternionf RotThumb1 = HandJointLocations.jointLocations[XR_HAND_JOINT_THUMB_METACARPAL_EXT].pose.orientation;
			RotThumb1 = XrQuaternionf_Multiply(InvRotSpace, RotThumb1);
			RotThumb1 = XrQuaternionf_Multiply(RotThumb1, InvRotThumb);

			XrQuaternionf RotThumb2 = HandJointLocations.jointLocations[XR_HAND_JOINT_THUMB_PROXIMAL_EXT].pose.orientation;
			RotThumb2 = XrQuaternionf_Multiply(InvRotSpace, RotThumb2);
			RotThumb2 = XrQuaternionf_Multiply(RotThumb2, InvRotThumb);

			XrQuaternionf RotThumb3 = HandJointLocations.jointLocations[XR_HAND_JOINT_THUMB_DISTAL_EXT].pose.orientation;
			RotThumb3 = XrQuaternionf_Multiply(InvRotSpace, RotThumb3);
			RotThumb3 = XrQuaternionf_Multiply(RotThumb3, InvRotThumb);

			XrQuaternionf RotIndex1 = HandJointLocations.jointLocations[XR_HAND_JOINT_INDEX_PROXIMAL_EXT].pose.orientation;
			RotIndex1 = XrQuaternionf_Multiply(InvRotSpace, RotIndex1);
			RotIndex1 = XrQuaternionf_Multiply(RotIndex1, InvRotTip);

			XrQuaternionf RotIndex2 =
				HandJointLocations.jointLocations[XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT].pose.orientation;
			RotIndex2 = XrQuaternionf_Multiply(InvRotSpace, RotIndex2);
			RotIndex2 = XrQuaternionf_Multiply(RotIndex2, InvRotTip);

			XrQuaternionf RotIndex3 = HandJointLocations.jointLocations[XR_HAND_JOINT_INDEX_DISTAL_EXT].pose.orientation;
			RotIndex3 = XrQuaternionf_Multiply(InvRotSpace, RotIndex3);
			RotIndex3 = XrQuaternionf_Multiply(RotIndex3, InvRotTip);

			XrQuaternionf RotMiddle1 = HandJointLocations.jointLocations[XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT].pose.orientation;
			RotMiddle1 = XrQuaternionf_Multiply(InvRotSpace, RotMiddle1);
			RotMiddle1 = XrQuaternionf_Multiply(RotMiddle1, InvRotTip);

			XrQuaternionf RotMiddle2 =
				HandJointLocations.jointLocations[XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT].pose.orientation;
			RotMiddle2 = XrQuaternionf_Multiply(InvRotSpace, RotMiddle2);
			RotMiddle2 = XrQuaternionf_Multiply(RotMiddle2, InvRotTip);

			XrQuaternionf RotMiddle3 = HandJointLocations.jointLocations[XR_HAND_JOINT_MIDDLE_DISTAL_EXT].pose.orientation;
			RotMiddle3 = XrQuaternionf_Multiply(InvRotSpace, RotMiddle3);
			RotMiddle3 = XrQuaternionf_Multiply(RotMiddle3, InvRotTip);

			XrQuaternionf RotRing1 = HandJointLocations.jointLocations[XR_HAND_JOINT_RING_PROXIMAL_EXT].pose.orientation;
			RotRing1 = XrQuaternionf_Multiply(InvRotSpace, RotRing1);
			RotRing1 = XrQuaternionf_Multiply(RotRing1, InvRotTip);

			XrQuaternionf RotRing2 = HandJointLocations.jointLocations[XR_HAND_JOINT_RING_INTERMEDIATE_EXT].pose.orientation;
			RotRing2 = XrQuaternionf_Multiply(InvRotSpace, RotRing2);
			RotRing2 = XrQuaternionf_Multiply(RotRing2, InvRotTip);

			XrQuaternionf RotRing3 = HandJointLocations.jointLocations[XR_HAND_JOINT_RING_DISTAL_EXT].pose.orientation;
			RotRing3 = XrQuaternionf_Multiply(InvRotSpace, RotRing3);
			RotRing3 = XrQuaternionf_Multiply(RotRing3, InvRotTip);

			XrQuaternionf RotPinky0 = HandJointLocations.jointLocations[XR_HAND_JOINT_LITTLE_METACARPAL_EXT].pose.orientation;
			RotPinky0 = XrQuaternionf_Multiply(InvRotSpace, RotPinky0);
			RotPinky0 = XrQuaternionf_Multiply(RotPinky0, InvRotTip);

			XrQuaternionf RotPinky1 = HandJointLocations.jointLocations[XR_HAND_JOINT_LITTLE_PROXIMAL_EXT].pose.orientation;
			RotPinky1 = XrQuaternionf_Multiply(InvRotSpace, RotPinky1);
			RotPinky1 = XrQuaternionf_Multiply(RotPinky1, InvRotTip);

			XrQuaternionf RotPinky2 =
				HandJointLocations.jointLocations[XR_HAND_JOINT_LITTLE_INTERMEDIATE_EXT].pose.orientation;
			RotPinky2 = XrQuaternionf_Multiply(InvRotSpace, RotPinky2);
			RotPinky2 = XrQuaternionf_Multiply(RotPinky2, InvRotTip);

			XrQuaternionf RotPinky3 = HandJointLocations.jointLocations[XR_HAND_JOINT_LITTLE_DISTAL_EXT].pose.orientation;
			RotPinky3 = XrQuaternionf_Multiply(InvRotSpace, RotPinky3);
			RotPinky3 = XrQuaternionf_Multiply(RotPinky3, InvRotTip);

			XrQuaternionf RotThumbTip = HandJointLocations.jointLocations[XR_HAND_JOINT_THUMB_TIP_EXT].pose.orientation;
			RotThumbTip = XrQuaternionf_Multiply(InvRotSpace, RotThumbTip);
			RotThumbTip = XrQuaternionf_Multiply(RotThumbTip, InvRotThumb);

			XrQuaternionf RotIndexTip = HandJointLocations.jointLocations[XR_HAND_JOINT_INDEX_TIP_EXT].pose.orientation;
			RotIndexTip = XrQuaternionf_Multiply(InvRotSpace, RotIndexTip);
			RotIndexTip = XrQuaternionf_Multiply(RotIndexTip, InvRotTip);

			XrQuaternionf RotMiddleTip = HandJointLocations.jointLocations[XR_HAND_JOINT_MIDDLE_TIP_EXT].pose.orientation;
			RotMiddleTip = XrQuaternionf_Multiply(InvRotSpace, RotMiddleTip);
			RotMiddleTip = XrQuaternionf_Multiply(RotMiddleTip, InvRotTip);

			XrQuaternionf RotRingTip = HandJointLocations.jointLocations[XR_HAND_JOINT_RING_TIP_EXT].pose.orientation;
			RotRingTip = XrQuaternionf_Multiply(InvRotSpace, RotRingTip);
			RotRingTip = XrQuaternionf_Multiply(RotRingTip, InvRotTip);

			XrQuaternionf RotPinkyTip = HandJointLocations.jointLocations[XR_HAND_JOINT_LITTLE_TIP_EXT].pose.orientation;
			RotPinkyTip = XrQuaternionf_Multiply(InvRotSpace, RotPinkyTip);
			RotPinkyTip = XrQuaternionf_Multiply(RotPinkyTip, InvRotTip);

			// convert all bones from wrist bone space to parent/child hierarchical bone space to match vrapi behavior
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::WristRoot)] = RotWrist;
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::ForearmStub)] = XrQuaternionf{ 0, 0, 0, 1 };

			// thumb
			RotThumbTip = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotThumb3), RotThumbTip);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::ThumbTip)] = RotThumbTip;
			RotThumb3 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotThumb2), RotThumb3);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Thumb3)] = RotThumb3;
			RotThumb2 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotThumb1), RotThumb2);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Thumb2)] = RotThumb2;
			RotThumb1 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotThumb0), RotThumb1);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Thumb1)] = RotThumb1;
			RotThumb0 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotWrist), RotThumb0);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Thumb0)] = RotThumb0;

			// index finger
			RotIndexTip = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotIndex3), RotIndexTip);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::IndexTip)] = RotIndexTip;
			RotIndex3 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotIndex2), RotIndex3);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Index3)] = RotIndex3;
			RotIndex2 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotIndex1), RotIndex2);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Index2)] = RotIndex2;
			RotIndex1 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotWrist), RotIndex1);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Index1)] = RotIndex1;

			// middle finger
			RotMiddleTip = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotMiddle3), RotMiddleTip);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::MiddleTip)] = RotMiddleTip;
			RotMiddle3 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotMiddle2), RotMiddle3);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Middle3)] = RotMiddle3;
			RotMiddle2 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotMiddle1), RotMiddle2);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Middle2)] = RotMiddle2;
			RotMiddle1 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotWrist), RotMiddle1);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Middle1)] = RotMiddle1;

			// ring finger
			RotRingTip = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotRing3), RotRingTip);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::RingTip)] = RotRingTip;
			RotRing3 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotRing2), RotRing3);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Ring3)] = RotRing3;
			RotRing2 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotRing1), RotRing2);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Ring2)] = RotRing2;
			RotRing1 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotWrist), RotRing1);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Ring1)] = RotRing1;

			// pinky finger
			RotPinkyTip = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotPinky3), RotPinkyTip);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::PinkyTip)] = RotPinkyTip;
			RotPinky3 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotPinky2), RotPinky3);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Pinky3)] = RotPinky3;
			RotPinky2 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotPinky1), RotPinky2);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Pinky2)] = RotPinky2;
			RotPinky1 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotPinky0), RotPinky1);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Pinky1)] = RotPinky1;
			RotPinky0 = XrQuaternionf_Multiply(XrQuaternionf_Inverse(RotWrist), RotPinky0);
			InternalHandsState.HandState[HandIndex].BoneRotations[static_cast<int32>(EHandBoneId::Pinky0)] = RotPinky0;

			// compute root pose by using the wrist bone pose with the last mile OpenXR pose adjustment backed out
			XrPosef RootPose = HandJointLocations.jointLocations[XR_HAND_JOINT_WRIST_EXT].pose;
			RootPose.orientation = XrQuaternionf_Multiply(RootPose.orientation, InvRotTip);
			InternalHandsState.HandState[HandIndex].RootPose = RootPose;

			InternalHandsState.HandState[HandIndex].HandScale =
				(HandScale.sensorOutput != 0.0f) ? HandScale.sensorOutput : 1.0f;
			InternalHandsState.HandState[HandIndex].RequestedTimeStamp = InternalHandsState.PredictedDisplayTime;
			InternalHandsState.HandState[HandIndex].SampleTimeStamp = InternalHandsState.PredictedDisplayTime;
			InternalHandsState.HandState[HandIndex].PointerPose.orientation = AimState.aimPose.orientation;
			InternalHandsState.HandState[HandIndex].PointerPose.position = AimState.aimPose.position;
			InternalHandsState.HandState[HandIndex].Pinches = 0;

			if (AimState.status & XR_HAND_TRACKING_AIM_INDEX_PINCHING_BIT_FB)
			{
				InternalHandsState.HandState[HandIndex].Pinches |= static_cast<int32>(EHandFingerPinch::Index);
			}
			if (AimState.status & XR_HAND_TRACKING_AIM_MIDDLE_PINCHING_BIT_FB)
			{
				InternalHandsState.HandState[HandIndex].Pinches |= static_cast<int32>(EHandFingerPinch::Middle);
			}
			if (AimState.status & XR_HAND_TRACKING_AIM_RING_PINCHING_BIT_FB)
			{
				InternalHandsState.HandState[HandIndex].Pinches |= static_cast<int32>(EHandFingerPinch::Ring);
			}
			if (AimState.status & XR_HAND_TRACKING_AIM_LITTLE_PINCHING_BIT_FB)
			{
				InternalHandsState.HandState[HandIndex].Pinches |= static_cast<int32>(EHandFingerPinch::Pinky);
			}
			if (InternalHandsState.HandState[HandIndex].Pinches != 0)
			{
				InternalHandsState.HandState[HandIndex].Pinches |= static_cast<int32>(EHandFingerPinch::Thumb);
			}

			InternalHandsState.HandState[HandIndex].PinchStrength[static_cast<int32>(EHandFinger::Index)] = AimState.pinchStrengthIndex;
			InternalHandsState.HandState[HandIndex].PinchStrength[static_cast<int32>(EHandFinger::Middle)] =
				AimState.pinchStrengthMiddle;
			InternalHandsState.HandState[HandIndex].PinchStrength[static_cast<int32>(EHandFinger::Ring)] = AimState.pinchStrengthRing;
			InternalHandsState.HandState[HandIndex].PinchStrength[static_cast<int32>(EHandFinger::Pinky)] = AimState.pinchStrengthLittle;

			// find max pinch strength
			float MaxPinchStrength = 0.0f;
			for (int i = static_cast<int32>(EHandFinger::Index); i < static_cast<int32>(EHandFinger::Count); i++)
			{
				if (InternalHandsState.HandState[HandIndex].PinchStrength[i] > MaxPinchStrength)
				{
					MaxPinchStrength = InternalHandsState.HandState[HandIndex].PinchStrength[i];
				}
			}

			// set thumb to max pinch strength
			InternalHandsState.HandState[HandIndex].PinchStrength[static_cast<int32>(EHandFinger::Thumb)] = MaxPinchStrength;

			// Update the public HandControllerState with the resulting values
			EOculusXRHandType HandType = bIsLeft ? EOculusXRHandType::HandLeft : EOculusXRHandType::HandRight;
			FOculusHandControllerState& ExternalState = HandControllerStates[HandType];
			FInternalHandState InternalHandState = InternalHandsState.HandState[HandIndex];
			ExternalState.bIsConnected = true;

			// Update various data about hands
			ExternalState.HandScale = InternalHandState.HandScale;

			// Update Bone Rotations
			for (uint32 BoneIndex = 0; BoneIndex < UE_ARRAY_COUNT(ExternalState.BoneRotations); BoneIndex++)
			{
				const XrQuaternionf RawRotation = InternalHandState.BoneRotations[BoneIndex];
				FQuat BoneRotation = XrBoneQuatToFQuat(RawRotation);
				BoneRotation.Normalize();
				ExternalState.BoneRotations[BoneIndex] = BoneRotation;
			}

			// Update Pinch State and Pinch Strength
			bool bTracked = (InternalHandState.Status & static_cast<int32>(EHandStatus::HandTracked)) != 0;
			ExternalState.TrackingConfidence = InternalHandState.HandConfidence;

			ExternalState.bIsPositionTracked = bTracked && ExternalState.TrackingConfidence == EOculusXRTrackingConfidence::High;
			ExternalState.bIsPositionValid = bTracked;
			ExternalState.bIsOrientationTracked = bTracked && ExternalState.TrackingConfidence == EOculusXRTrackingConfidence::High;
			ExternalState.bIsOrientationValid = bTracked;
			ExternalState.bIsPointerPoseValid = (InternalHandState.Status & static_cast<int32>(EHandStatus::InputValid)) != 0;

			const XrPosef PointerPose = InternalHandState.PointerPose;
			FVector PointerPoseTranslation = XrPoseVectorToFVector(PointerPose.position);
			FQuat PointerPoseRotation = XrPoseQuatToFQuat(PointerPose.orientation);
			ExternalState.PointerPose.SetTranslation(PointerPoseTranslation);
			ExternalState.PointerPose.SetRotation(PointerPoseRotation);

			ExternalState.bIsDominantHand = (InternalHandState.Status & static_cast<int32>(EHandStatus::DominantHand)) != 0;

			// Poll for finger confidence
			for (uint32 FingerIndex = 0; FingerIndex < static_cast<int32>(EOculusHandAxes::TotalAxisCount); FingerIndex++)
			{
				ExternalState.FingerConfidences[FingerIndex] = InternalHandState.FingerConfidences[FingerIndex];
			}

			// Poll for specific actions
			const IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
			auto PlatformUser = DeviceMapper.GetPrimaryPlatformUser();
			auto DeviceId = DeviceMapper.GetDefaultInputDevice();
			const FPlatformUserId UserId = DeviceMapper.GetPrimaryPlatformUser();
			const bool handMenuButtonPressed = (InternalHandState.Status & static_cast<int32>(EHandStatus::MenuPressed)) != 0;
			if (handMenuButtonPressed != InternalHandsState.bHandMenuButtonPressed)
			{
				InternalHandsState.bHandMenuButtonPressed = handMenuButtonPressed;
				SendControllerButtonPressed(EKeys::OculusTouch_Left_Menu_Click.GetFName(), InternalHandsState.bHandMenuButtonPressed, PlatformUser, DeviceId);
			}

			// Poll for finger pinches
			for (uint32 FingerIndex = 0; FingerIndex < static_cast<uint32>(EOculusHandButton::TotalButtonCount); FingerIndex++)
			{
				FOculusButtonState& PinchState = ExternalState.HandButtons[FingerIndex];
				check(!PinchState.Key.IsNone());

				bool bPressed = false;
				if (FingerIndex < static_cast<uint32>(EOculusHandButton::System))
				{
					bPressed = ((static_cast<uint32>(InternalHandState.Pinches) & (1 << FingerIndex)) != 0);
					bPressed &= (InternalHandState.HandConfidence == EOculusXRTrackingConfidence::High) && (InternalHandState.FingerConfidences[FingerIndex] == EOculusXRTrackingConfidence::High);
				}
				else if (FingerIndex == static_cast<uint32>(EOculusHandButton::System))
				{
					bPressed = (InternalHandState.Status & static_cast<int32>(EHandStatus::SystemGestureInProgress)) != 0;
				}
				else
				{
					bPressed = InternalHandsState.bHandMenuButtonPressed && !ExternalState.bIsDominantHand;
				}

				if (bPressed != PinchState.bIsPressed)
				{
					PinchState.bIsPressed = bPressed;
					SendControllerButtonPressed(PinchState.Key, PinchState.bIsPressed, PlatformUser, DeviceId);
				}
			}

			// Poll for finger strength
			for (uint32 FingerIndex = 0; FingerIndex < static_cast<uint32>(EOculusHandAxes::TotalAxisCount); FingerIndex++)
			{
				FOculusAxisState& PinchStrength = ExternalState.HandAxes[FingerIndex];
				check(!PinchStrength.Axis.IsNone());

				float PinchValue = 0.0f;
				if (InternalHandState.HandConfidence == EOculusXRTrackingConfidence::High)
				{
					PinchValue = InternalHandState.PinchStrength[FingerIndex];
				}

				if (PinchValue != PinchStrength.State)
				{
					MessageHandler->OnControllerAnalog(PinchStrength.Axis, PlatformUser, DeviceId, PinchValue);
					PinchStrength.State = PinchValue;
				}
			}
		}

		bHandTrackingActive = bHandTrackingActiveCurrentTick;
	}

	void FHandTrackingExtensionPlugin::PostSyncActions(XrSession InSession)
	{
		if (MicrogestureActionSet == XR_NULL_HANDLE)
		{
			return;
		}

		IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
		const FPlatformUserId UserId = DeviceMapper.GetPrimaryPlatformUser();
		const FInputDeviceId DeviceId = DeviceMapper.GetDefaultInputDevice();

		for (const auto& MicrogestureAction : MicrogestureActions)
		{
			XrActionStateBoolean State{ XR_TYPE_ACTION_STATE_BOOLEAN };
			XrActionStateGetInfo Info{ XR_TYPE_ACTION_STATE_GET_INFO };
			Info.action = MicrogestureAction.Action;

			// Send a button press whenever the state changes for the given action
			const XrResult Result = xrGetActionStateBoolean(InSession, &Info, &State);
			if (XR_SUCCEEDED(Result) && State.isActive && State.changedSinceLastSync)
			{
				const bool bIsPressed = static_cast<bool>(State.currentState);
				SendControllerButtonPressed(MicrogestureAction.InputKey.GetFName(), bIsPressed, UserId, DeviceId);
			}
		}
	}

	bool FHandTrackingExtensionPlugin::GetSkeleton(EOculusXRHandType HandType, TSharedPtr<FHandSkeleton> Skeleton)
	{
		if (!IsHandTrackingAvailable() && bExtHandTrackingMeshAvailable)
		{
			return false;
		}

		if (!Skeleton)
		{
			return false;
		}

		const bool bIsLeft = HandType == EOculusXRHandType::HandLeft;
		XrHandTrackerEXT& HandTracker = bIsLeft ? OculusHandTrackers[EOculusXRHandType::HandLeft] : OculusHandTrackers[EOculusXRHandType::HandRight];
		XrHandTrackingMeshFB XrMesh = { XR_TYPE_HAND_TRACKING_MESH_FB };
		if (XR_SUCCEEDED(xrGetHandMeshFB(HandTracker, &XrMesh)))
		{
			const uint32_t NumCapsules = XR_FB_HAND_TRACKING_CAPSULE_COUNT;
			const uint32_t NumJoints = XrMesh.jointCountOutput;
			const uint32_t NumVerts = XrMesh.vertexCountOutput;
			const uint32_t NumIndices = XrMesh.indexCountOutput;

			if ((NumJoints > static_cast<int32>(EHandSkeletonConstants::MaxBones)) || (NumCapsules > static_cast<int32>(EHandSkeletonConstants::MaxBoneCapsules)))
			{
				// fail if runtime mesh data exceeds capacity of Skeleton struct
				return false;
			}

			// intermediary storage for skeleton data
			TArray<XrPosef> JointBindPoses;
			JointBindPoses.SetNum(NumJoints);
			XrMesh.jointBindPoses = JointBindPoses.GetData();

			TArray<XrHandJointEXT> JointParents;
			JointParents.SetNum(NumJoints);
			XrMesh.jointParents = JointParents.GetData();

			TArray<float> JointRadii;
			JointRadii.SetNum(NumJoints);
			XrMesh.jointRadii = JointRadii.GetData();

			FHandMesh HandMesh;
			XrMesh.vertexPositions = HandMesh.VertexPositions;
			XrMesh.vertexNormals = HandMesh.VertexNormals;
			XrMesh.vertexUVs = HandMesh.VertexUV0;
			XrMesh.vertexBlendIndices = HandMesh.BlendIndices;
			XrMesh.vertexBlendWeights = HandMesh.BlendWeights;
			XrMesh.indices = HandMesh.Indices;

			XrMesh.jointCapacityInput = NumJoints;
			XrMesh.vertexCapacityInput = NumVerts;
			XrMesh.indexCapacityInput = NumIndices;

			XrHandTrackingCapsulesStateFB CapsuleState = { XR_TYPE_HAND_TRACKING_CAPSULES_STATE_FB };

			XrMesh.next = &CapsuleState;
			XrResult Result = xrGetHandMeshFB(HandTracker, &XrMesh);
			if (XR_SUCCEEDED(Result))
			{
				Skeleton->HandType = HandType;
				Skeleton->NumBones = static_cast<int32>(EHandSkeletonConstants::MaxHandBones);
				Skeleton->NumBoneCapsules = XR_FB_HAND_TRACKING_CAPSULE_COUNT;

				// Compute bone poses without hierarchial transform (needed for capsules)
				for (int i = 0; i < static_cast<int32>(EHandSkeletonConstants::MaxHandBones); ++i)
				{
					const EHandBoneId HandBoneId = (EHandBoneId)i;
					const XrHandJointEXT Joint = MapBoneToXrHandJoint(HandBoneId);

					const XrPosef RootPose = XrMesh.jointBindPoses[XR_HAND_JOINT_WRIST_EXT];
					XrPosef JointPose = (Joint != XR_HAND_JOINT_MAX_ENUM_EXT) ? XrMesh.jointBindPoses[Joint] : XrPosef_Identity();

					// fix up thumb0 since it does not exist in OpenXR
					if (HandBoneId == EHandBoneId::Thumb0)
					{
						JointPose = bIsLeft ? XrLeftHandLegacyBindPoseThumb0 : XrRightHandLegacyBindPoseThumb0;
					}

					JointPose = TransformXrHandJointPose(HandType, Joint, JointPose, RootPose, true);

					Skeleton->Bones[i].BoneId = HandBoneId;
					Skeleton->Bones[i].Pose = JointPose;
				}

				// Compute bone capsules using non-hierarchial bone poses
				for (int i = 0, OutIndex = 0;
					 i < XR_FB_HAND_TRACKING_CAPSULE_COUNT && OutIndex < static_cast<int32>(EHandSkeletonConstants::MaxBoneCapsules);
					 ++i)
				{
					const EHandBoneId HandBoneId = MapXrHandJointToBone(CapsuleState.capsules[i].joint);

					if (HandBoneId != EHandBoneId::Invalid)
					{
						XrPosef InvBonePose;
						XrPosef_Invert(&InvBonePose, &Skeleton->Bones[static_cast<int32>(HandBoneId)].Pose);
						XrVector3f Point0;
						XrPosef_TransformVector3f(&Point0, &InvBonePose, &CapsuleState.capsules[i].points[0]);
						XrVector3f Point1;
						XrPosef_TransformVector3f(&Point1, &InvBonePose, &CapsuleState.capsules[i].points[1]);

						Skeleton->BoneCapsules[OutIndex].BoneIndex = static_cast<int32>(HandBoneId);
						Skeleton->BoneCapsules[OutIndex].Points[0] = Point0;
						Skeleton->BoneCapsules[OutIndex].Points[1] = Point1;
						Skeleton->BoneCapsules[OutIndex].Radius = CapsuleState.capsules[i].radius;

						OutIndex++;
					}
				}

				// Compute final bone poses with hierarchial transform
				for (int i = 0; i < static_cast<int32>(EHandSkeletonConstants::MaxHandBones); ++i)
				{
					const EHandBoneId HandBoneId = (EHandBoneId)i;

					const XrHandJointEXT Joint = MapBoneToXrHandJoint(HandBoneId);
					XrHandJointEXT ParentJoint =
						(Joint != XR_HAND_JOINT_MAX_ENUM_EXT) ? XrMesh.jointParents[Joint] : XR_HAND_JOINT_MAX_ENUM_EXT;
					EHandBoneId ParentHandBoneId = MapXrHandJointToBone(ParentJoint);

					// fix up parent bone mappings for VrApi hand compat
					if ((ParentHandBoneId == EHandBoneId::Invalid) && (Joint != XR_HAND_JOINT_WRIST_EXT))
					{
						ParentHandBoneId = EHandBoneId::WristRoot;
						ParentJoint = MapBoneToXrHandJoint(ParentHandBoneId);
					}

					XrPosef ParentPose =
						(ParentJoint != XR_HAND_JOINT_MAX_ENUM_EXT) ? XrMesh.jointBindPoses[ParentJoint] : XrPosef_Identity();

					// fix up thumb1 parent since thumb0 does not exist in OpenXR
					if (HandBoneId == EHandBoneId::Thumb1)
					{
						ParentHandBoneId = EHandBoneId::Thumb0;
						ParentJoint = XR_HAND_JOINT_MAX_ENUM_EXT;
						ParentPose = bIsLeft ? XrLeftHandLegacyBindPoseThumb0 : XrRightHandLegacyBindPoseThumb0;
					}

					ParentPose = TransformXrHandJointPose(HandType, ParentJoint, ParentPose, XrPosef_Identity(), false);
					XrPosef ParentPoseInv;
					XrPosef_Invert(&ParentPoseInv, &ParentPose);
					XrPosef JointPose{};
					XrPosef_Multiply(&JointPose, &ParentPoseInv, (XrPosef*)&Skeleton->Bones[i].Pose);

					Skeleton->Bones[i].BoneId = HandBoneId;
					Skeleton->Bones[i].Pose = JointPose;
					Skeleton->Bones[i].ParentBoneIndex = static_cast<int32>(ParentHandBoneId);
				}

				return true;
			}
		}

		return false;
	}

	bool FHandTrackingExtensionPlugin::GetMesh(EOculusXRHandType HandType, TSharedPtr<FHandMesh> HandMesh)
	{
		if (!IsHandTrackingAvailable() && bExtHandTrackingMeshAvailable)
		{
			return false;
		}
		if (!HandMesh)
		{
			return false;
		}

		if (HandType != EOculusXRHandType::HandLeft && HandType != EOculusXRHandType::HandRight)
		{
			return false;
		}

		const bool bIsLeft = HandType == EOculusXRHandType::HandLeft;
		XrHandTrackerEXT& HandTracker = bIsLeft ? OculusHandTrackers[EOculusXRHandType::HandLeft] : OculusHandTrackers[EOculusXRHandType::HandRight];

		XrHandTrackingMeshFB XrMesh = { XR_TYPE_HAND_TRACKING_MESH_FB };

		if (XR_SUCCEEDED(xrGetHandMeshFB(HandTracker, &XrMesh)))
		{
			const uint32_t NumJoints = XrMesh.jointCountOutput;
			const uint32_t NumVerts = XrMesh.vertexCountOutput;
			const uint32_t NumIndices = XrMesh.indexCountOutput;

			if ((NumVerts > static_cast<int32>(EHandMeshConstants::MaxVertices)) || (NumIndices > static_cast<int32>(EHandMeshConstants::MaxIndices)))
			{
				// fail if runtime mesh data exceeds capacity of Mesh struct
				return false;
			}

			// temp storage for skeleton data since extension spec requires query with joints
			TArray<XrPosef> JointBindPoses;
			JointBindPoses.SetNum(NumJoints);
			XrMesh.jointBindPoses = JointBindPoses.GetData();

			TArray<XrHandJointEXT> JointParents;
			JointParents.SetNum(NumJoints);
			XrMesh.jointParents = JointParents.GetData();

			TArray<float> jointRadii;
			jointRadii.SetNum(NumJoints);
			XrMesh.jointRadii = jointRadii.GetData();

			// mesh data
			XrMesh.vertexPositions = HandMesh->VertexPositions;
			XrMesh.vertexNormals = HandMesh->VertexNormals;
			XrMesh.vertexUVs = HandMesh->VertexUV0;
			XrMesh.vertexBlendIndices = HandMesh->BlendIndices;
			XrMesh.vertexBlendWeights = HandMesh->BlendWeights;
			XrMesh.indices = HandMesh->Indices;

			XrMesh.jointCapacityInput = NumJoints;
			XrMesh.vertexCapacityInput = NumVerts;
			XrMesh.indexCapacityInput = NumIndices;

			if (XR_SUCCEEDED(xrGetHandMeshFB(HandTracker, &XrMesh)))
			{
				HandMesh->HandType = HandType;
				HandMesh->NumVertices = NumVerts;
				HandMesh->NumIndices = NumIndices;

					for (int i = 0; i < (int)NumVerts; i++)
					{
						HandMesh->BlendIndices[i].x = (short)MapXrHandJointToBone((XrHandJointEXT)HandMesh->BlendIndices[i].x);
						HandMesh->BlendIndices[i].y = (short)MapXrHandJointToBone((XrHandJointEXT)HandMesh->BlendIndices[i].y);
						HandMesh->BlendIndices[i].z = (short)MapXrHandJointToBone((XrHandJointEXT)HandMesh->BlendIndices[i].z);
						HandMesh->BlendIndices[i].w = (short)MapXrHandJointToBone((XrHandJointEXT)HandMesh->BlendIndices[i].w);
					}
				return true;
			}
		}
		return false;
	}

	XrPosef FHandTrackingExtensionPlugin::TransformXrHandJointPose(
		EOculusXRHandType HandType,
		XrHandJointEXT Joint,
		XrPosef JointPose,
		XrPosef RootPose,
		bool ApplyRootPose)
	{
		if ((Joint == XR_HAND_JOINT_MAX_ENUM_EXT) || ((HandType != EOculusXRHandType::HandLeft) && (HandType != EOculusXRHandType::HandRight)))
		{
			return JointPose;
		}

		// back out last mile OpenXR spec pose adjustment and convert all bones to wrist bone space
		const bool bIsLeft = (HandType == EOculusXRHandType::HandLeft);
		const FQuat4f RotTip = bIsLeft ? FQuat4f({ 1.0f, 0.0f, 0.0f }, FMath::DegreesToRadians(180.0f)) * FQuat4f({ 0.0f, 1.0f, 0.0f }, FMath::DegreesToRadians(-90.0f))
									   : FQuat4f({ 0.0f, 1.0f, 0.0f }, FMath::DegreesToRadians(90.0f));
		const FQuat4f RotThumb = bIsLeft ? RotTip * FQuat4f({ 0.0f, 0.0f, 1.0f }, FMath::DegreesToRadians(90.0f))
										 : RotTip * FQuat4f({ 0.0f, 0.0f, 1.0f }, FMath::DegreesToRadians(-90.0f));

		XrQuaternionf InvRotTip = XrQuaternionf_Inverse((XrQuaternionf&)RotTip);
		XrQuaternionf InvRotThumb = XrQuaternionf_Inverse((XrQuaternionf&)RotThumb);
		InvRotTip = XrQuaternionf_Inverse((XrQuaternionf&)RotTip);
		InvRotThumb = XrQuaternionf_Inverse((XrQuaternionf&)RotThumb);

		if (XR_CURRENT_API_VERSION >= XR_MAKE_VERSION(1, 0, 23))
		{
			InvRotThumb = InvRotTip;
		}

		if (ApplyRootPose)
		{
			RootPose.orientation = XrQuaternionf_Multiply(RootPose.orientation, InvRotTip);
			XrPosef RootPoseInv;
			XrPosef_Invert(&RootPoseInv, &RootPose);
			XrPosef_Multiply(&JointPose, &RootPoseInv, &JointPose);
		}

		switch (Joint)
		{
			case XR_HAND_JOINT_THUMB_METACARPAL_EXT:
			case XR_HAND_JOINT_THUMB_PROXIMAL_EXT:
			case XR_HAND_JOINT_THUMB_DISTAL_EXT:
			case XR_HAND_JOINT_THUMB_TIP_EXT:
				JointPose.orientation = XrQuaternionf_Multiply(JointPose.orientation, InvRotThumb);
				break;
			case XR_HAND_JOINT_PALM_EXT:
			case XR_HAND_JOINT_WRIST_EXT:
			case XR_HAND_JOINT_INDEX_METACARPAL_EXT:
			case XR_HAND_JOINT_INDEX_PROXIMAL_EXT:
			case XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT:
			case XR_HAND_JOINT_INDEX_DISTAL_EXT:
			case XR_HAND_JOINT_INDEX_TIP_EXT:
			case XR_HAND_JOINT_MIDDLE_METACARPAL_EXT:
			case XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT:
			case XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT:
			case XR_HAND_JOINT_MIDDLE_DISTAL_EXT:
			case XR_HAND_JOINT_MIDDLE_TIP_EXT:
			case XR_HAND_JOINT_RING_METACARPAL_EXT:
			case XR_HAND_JOINT_RING_PROXIMAL_EXT:
			case XR_HAND_JOINT_RING_INTERMEDIATE_EXT:
			case XR_HAND_JOINT_RING_DISTAL_EXT:
			case XR_HAND_JOINT_RING_TIP_EXT:
			case XR_HAND_JOINT_LITTLE_METACARPAL_EXT:
			case XR_HAND_JOINT_LITTLE_PROXIMAL_EXT:
			case XR_HAND_JOINT_LITTLE_INTERMEDIATE_EXT:
			case XR_HAND_JOINT_LITTLE_DISTAL_EXT:
			case XR_HAND_JOINT_LITTLE_TIP_EXT:
				JointPose.orientation = XrQuaternionf_Multiply(JointPose.orientation, InvRotTip);
				break;
			default:
				break;
		}

		return JointPose;
	}

	XrHandJointEXT FHandTrackingExtensionPlugin::MapBoneToXrHandJoint(EHandBoneId bone)
	{
		switch (bone)
		{
			// case undefined:
			//   return XR_HAND_JOINT_PALM_EXT;
			case EHandBoneId::WristRoot:
				return XR_HAND_JOINT_WRIST_EXT;
			case EHandBoneId::ForearmStub:
				return XR_HAND_JOINT_MAX_ENUM_EXT; // undefined
			case EHandBoneId::Thumb0:
				return XR_HAND_JOINT_MAX_ENUM_EXT; // undefined
			case EHandBoneId::Thumb1:
				return XR_HAND_JOINT_THUMB_METACARPAL_EXT;
			case EHandBoneId::Thumb2:
				return XR_HAND_JOINT_THUMB_PROXIMAL_EXT;
			case EHandBoneId::Thumb3:
				return XR_HAND_JOINT_THUMB_DISTAL_EXT;
			// case undefined:
			//  return XR_HAND_JOINT_INDEX_METACARPAL_EXT;
			case EHandBoneId::Index1:
				return XR_HAND_JOINT_INDEX_PROXIMAL_EXT;
			case EHandBoneId::Index2:
				return XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT;
			case EHandBoneId::Index3:
				return XR_HAND_JOINT_INDEX_DISTAL_EXT;
			// case undefined:
			//  return XR_HAND_JOINT_MIDDLE_METACARPAL_EXT;
			case EHandBoneId::Middle1:
				return XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT;
			case EHandBoneId::Middle2:
				return XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT;
			case EHandBoneId::Middle3:
				return XR_HAND_JOINT_MIDDLE_DISTAL_EXT;
			// case undefined:
			//  return XR_HAND_JOINT_RING_METACARPAL_EXT;
			case EHandBoneId::Ring1:
				return XR_HAND_JOINT_RING_PROXIMAL_EXT;
			case EHandBoneId::Ring2:
				return XR_HAND_JOINT_RING_INTERMEDIATE_EXT;
			case EHandBoneId::Ring3:
				return XR_HAND_JOINT_RING_DISTAL_EXT;
			case EHandBoneId::Pinky0:
				return XR_HAND_JOINT_LITTLE_METACARPAL_EXT;
			case EHandBoneId::Pinky1:
				return XR_HAND_JOINT_LITTLE_PROXIMAL_EXT;
			case EHandBoneId::Pinky2:
				return XR_HAND_JOINT_LITTLE_INTERMEDIATE_EXT;
			case EHandBoneId::Pinky3:
				return XR_HAND_JOINT_LITTLE_DISTAL_EXT;
			case EHandBoneId::ThumbTip:
				return XR_HAND_JOINT_THUMB_TIP_EXT;
			case EHandBoneId::IndexTip:
				return XR_HAND_JOINT_INDEX_TIP_EXT;
			case EHandBoneId::MiddleTip:
				return XR_HAND_JOINT_MIDDLE_TIP_EXT;
			case EHandBoneId::RingTip:
				return XR_HAND_JOINT_RING_TIP_EXT;
			case EHandBoneId::PinkyTip:
				return XR_HAND_JOINT_LITTLE_TIP_EXT;
			default:
				return XR_HAND_JOINT_MAX_ENUM_EXT; // undefined
		}
	}

	EHandBoneId FHandTrackingExtensionPlugin::MapXrHandJointToBone(XrHandJointEXT joint)
	{
		switch (joint)
		{
			case XR_HAND_JOINT_PALM_EXT:
				return EHandBoneId::Invalid; // undefined
			case XR_HAND_JOINT_WRIST_EXT:
				return EHandBoneId::WristRoot;
			// case undefined:
			//  return EHandBoneId::ForearmStub;
			// case undefined:
			//  return EHandBoneId::Thumb0;
			case XR_HAND_JOINT_THUMB_METACARPAL_EXT:
				return EHandBoneId::Thumb1;
			case XR_HAND_JOINT_THUMB_PROXIMAL_EXT:
				return EHandBoneId::Thumb2;
			case XR_HAND_JOINT_THUMB_DISTAL_EXT:
				return EHandBoneId::Thumb3;
			case XR_HAND_JOINT_INDEX_METACARPAL_EXT:
				return EHandBoneId::Invalid; // undefined
			case XR_HAND_JOINT_INDEX_PROXIMAL_EXT:
				return EHandBoneId::Index1;
			case XR_HAND_JOINT_INDEX_INTERMEDIATE_EXT:
				return EHandBoneId::Index2;
			case XR_HAND_JOINT_INDEX_DISTAL_EXT:
				return EHandBoneId::Index3;
			case XR_HAND_JOINT_MIDDLE_METACARPAL_EXT:
				return EHandBoneId::Invalid; // undefined
			case XR_HAND_JOINT_MIDDLE_PROXIMAL_EXT:
				return EHandBoneId::Middle1;
			case XR_HAND_JOINT_MIDDLE_INTERMEDIATE_EXT:
				return EHandBoneId::Middle2;
			case XR_HAND_JOINT_MIDDLE_DISTAL_EXT:
				return EHandBoneId::Middle3;
			case XR_HAND_JOINT_RING_METACARPAL_EXT:
				return EHandBoneId::Invalid; // undefined
			case XR_HAND_JOINT_RING_PROXIMAL_EXT:
				return EHandBoneId::Ring1;
			case XR_HAND_JOINT_RING_INTERMEDIATE_EXT:
				return EHandBoneId::Ring2;
			case XR_HAND_JOINT_RING_DISTAL_EXT:
				return EHandBoneId::Ring3;
			case XR_HAND_JOINT_LITTLE_METACARPAL_EXT:
				return EHandBoneId::Pinky0;
			case XR_HAND_JOINT_LITTLE_PROXIMAL_EXT:
				return EHandBoneId::Pinky1;
			case XR_HAND_JOINT_LITTLE_INTERMEDIATE_EXT:
				return EHandBoneId::Pinky2;
			case XR_HAND_JOINT_LITTLE_DISTAL_EXT:
				return EHandBoneId::Pinky3;
			case XR_HAND_JOINT_THUMB_TIP_EXT:
				return EHandBoneId::ThumbTip;
			case XR_HAND_JOINT_INDEX_TIP_EXT:
				return EHandBoneId::IndexTip;
			case XR_HAND_JOINT_MIDDLE_TIP_EXT:
				return EHandBoneId::MiddleTip;
			case XR_HAND_JOINT_RING_TIP_EXT:
				return EHandBoneId::RingTip;
			case XR_HAND_JOINT_LITTLE_TIP_EXT:
				return EHandBoneId::PinkyTip;
			default:
				return EHandBoneId::Invalid; // undefined
		}
	}

	FQuat FHandTrackingExtensionPlugin::XrBoneQuatToFQuat(XrQuaternionf XrQuat)
	{
		return FQuat(XrQuat.x, -XrQuat.y, XrQuat.z, -XrQuat.w);
	}

	FVector FHandTrackingExtensionPlugin::XrPoseVectorToFVector(XrVector3f XrVector)
	{
		return FVector(-XrVector.z, XrVector.x, XrVector.y);
	}

	FQuat FHandTrackingExtensionPlugin::XrPoseQuatToFQuat(XrQuaternionf XrQuat)
	{
		return FQuat(-XrQuat.z, XrQuat.x, XrQuat.y, -XrQuat.w);
	}

} // namespace OculusXRInput
