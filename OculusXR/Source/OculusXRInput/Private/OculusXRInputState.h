// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "IOculusXRInputModule.h"

#if OCULUS_INPUT_SUPPORTED_PLATFORMS
#include "IMotionController.h"
#include "InputCoreTypes.h"
#include "OculusXRInputFunctionLibrary.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"

namespace OculusXRInput
{

	//-------------------------------------------------------------------------------------------------
	// Button names
	//-------------------------------------------------------------------------------------------------

	enum class EOculusTouchControllerButton
	{
		// NOTE: The Trigger and Grip digital buttons are synthetic.  Oculus hardware doesn't support a digital press for these
		Trigger,
		Grip,

		XA,
		YB,
		Thumbstick,

		Thumbstick_Up,
		Thumbstick_Down,
		Thumbstick_Left,
		Thumbstick_Right,

		Menu,

		Thumbstick_Touch,
		Trigger_Touch,
		XA_Touch,
		YB_Touch,

		/** Total number of controller buttons */
		TotalButtonCount
	};

	enum class EOculusRemoteControllerButton
	{
		DPad_Up,
		DPad_Down,
		DPad_Left,
		DPad_Right,

		Enter,
		Back,

		VolumeUp,
		VolumeDown,
		Home,

		/** Total number of controller buttons */
		TotalButtonCount
	};

	enum class EOculusTouchCapacitiveAxes
	{
		Thumbstick,
		Trigger,
		XA,
		YB,
		IndexPointing,
		ThumbUp,
		ThumbRest,

		/** Total number of capacitive axes */
		TotalAxisCount
	};

	enum class EOculusHandButton
	{
		Thumb,
		Index,
		Middle,
		Ring,
		Pinky,
		System,
		Menu,
		TotalButtonCount
	};

	enum class EOculusHandAxes
	{
		Thumb,
		Index,
		Middle,
		Ring,
		Pinky,
		TotalAxisCount
	};

	enum class EOculusMicrogestures
	{
		NoGesture,
		SwipeLeft,
		SwipeRight,
		SwipeForward,
		SwipeBack,
		TapThumb,
		TotalGestureCount
	};

	//-------------------------------------------------------------------------------------------------
	// FOculusKey
	//-------------------------------------------------------------------------------------------------

	struct FOculusKey
	{
		static const FKey OculusTouch_Left_Thumbstick;
		static const FKey OculusTouch_Left_Trigger;
		static const FKey OculusTouch_Left_FaceButton1; // X or A
		static const FKey OculusTouch_Left_FaceButton2; // Y or B
		static const FKey OculusTouch_Left_IndexPointing;
		static const FKey OculusTouch_Left_Trigger_Proximity;
		static const FKey OculusTouch_Left_ThumbUp;
		static const FKey OculusTouch_Left_Thumb_Proximity;
		static const FKey OculusTouch_Left_ThumbRest;
		static const FKey OculusTouch_Left_ThumbRest_Force;
		static const FKey OculusTouch_Left_Stylus_Force;
		static const FKey OculusTouch_Left_IndexTrigger_Curl;
		static const FKey OculusTouch_Left_IndexTrigger_Slide;
		static const FKey OculusTouch_Left_IndexTrigger_Force;

		static const FKey OculusTouch_Right_Thumbstick;
		static const FKey OculusTouch_Right_Trigger;
		static const FKey OculusTouch_Right_FaceButton1; // X or A
		static const FKey OculusTouch_Right_FaceButton2; // Y or B
		static const FKey OculusTouch_Right_IndexPointing;
		static const FKey OculusTouch_Right_Trigger_Proximity;
		static const FKey OculusTouch_Right_ThumbUp;
		static const FKey OculusTouch_Right_Thumb_Proximity;
		static const FKey OculusTouch_Right_ThumbRest;
		static const FKey OculusTouch_Right_ThumbRest_Force;
		static const FKey OculusTouch_Right_Stylus_Force;
		static const FKey OculusTouch_Right_IndexTrigger_Curl;
		static const FKey OculusTouch_Right_IndexTrigger_Slide;
		static const FKey OculusTouch_Right_IndexTrigger_Force;

		static const FKey OculusRemote_DPad_Up;
		static const FKey OculusRemote_DPad_Down;
		static const FKey OculusRemote_DPad_Left;
		static const FKey OculusRemote_DPad_Right;

		static const FKey OculusRemote_Enter;
		static const FKey OculusRemote_Back;

		static const FKey OculusRemote_VolumeUp;
		static const FKey OculusRemote_VolumeDown;
		static const FKey OculusRemote_Home;

		static const FKey OculusHand_Left_ThumbPinch;
		static const FKey OculusHand_Left_IndexPinch;
		static const FKey OculusHand_Left_MiddlePinch;
		static const FKey OculusHand_Left_RingPinch;
		static const FKey OculusHand_Left_PinkyPinch;

		static const FKey OculusHand_Right_ThumbPinch;
		static const FKey OculusHand_Right_IndexPinch;
		static const FKey OculusHand_Right_MiddlePinch;
		static const FKey OculusHand_Right_RingPinch;
		static const FKey OculusHand_Right_PinkyPinch;

		static const FKey OculusHand_Left_SystemGesture;
		static const FKey OculusHand_Right_SystemGesture;

		static const FKey OculusHand_Left_ThumbPinchStrength;
		static const FKey OculusHand_Left_IndexPinchStrength;
		static const FKey OculusHand_Left_MiddlePinchStrength;
		static const FKey OculusHand_Left_RingPinchStrength;
		static const FKey OculusHand_Left_PinkyPinchStrength;

		static const FKey OculusHand_Right_ThumbPinchStrength;
		static const FKey OculusHand_Right_IndexPinchStrength;
		static const FKey OculusHand_Right_MiddlePinchStrength;
		static const FKey OculusHand_Right_RingPinchStrength;
		static const FKey OculusHand_Right_PinkyPinchStrength;

		static const FKey OculusHand_Left_MicrogestureSwipeLeft;
		static const FKey OculusHand_Left_MicrogestureSwipeRight;
		static const FKey OculusHand_Left_MicrogestureSwipeForward;
		static const FKey OculusHand_Left_MicrogestureSwipeBack;
		static const FKey OculusHand_Left_MicrogestureTapThumb;

		static const FKey OculusHand_Right_MicrogestureSwipeLeft;
		static const FKey OculusHand_Right_MicrogestureSwipeRight;
		static const FKey OculusHand_Right_MicrogestureSwipeForward;
		static const FKey OculusHand_Right_MicrogestureSwipeBack;
		static const FKey OculusHand_Right_MicrogestureTapThumb;
	};

	//-------------------------------------------------------------------------------------------------
	// FOculusKeyNames
	//-------------------------------------------------------------------------------------------------

	struct FOculusKeyNames
	{
		typedef FName Type;

		static const FName OculusTouch_Left_Thumbstick;
		static const FName OculusTouch_Left_Trigger;
		static const FName OculusTouch_Left_FaceButton1; // X or A
		static const FName OculusTouch_Left_FaceButton2; // Y or B
		static const FName OculusTouch_Left_IndexPointing;
		static const FName OculusTouch_Left_Trigger_Proximity;
		static const FName OculusTouch_Left_ThumbUp;
		static const FName OculusTouch_Left_Thumb_Proximity;
		static const FName OculusTouch_Left_ThumbRest;

		static const FName OculusTouch_Right_Thumbstick;
		static const FName OculusTouch_Right_Trigger;
		static const FName OculusTouch_Right_FaceButton1; // X or A
		static const FName OculusTouch_Right_FaceButton2; // Y or B
		static const FName OculusTouch_Right_IndexPointing;
		static const FName OculusTouch_Right_Trigger_Proximity;
		static const FName OculusTouch_Right_ThumbUp;
		static const FName OculusTouch_Right_Thumb_Proximity;
		static const FName OculusTouch_Right_ThumbRest;

		static const FName OculusRemote_DPad_Up;
		static const FName OculusRemote_DPad_Down;
		static const FName OculusRemote_DPad_Left;
		static const FName OculusRemote_DPad_Right;

		static const FName OculusRemote_Enter;
		static const FName OculusRemote_Back;

		static const FName OculusRemote_VolumeUp;
		static const FName OculusRemote_VolumeDown;
		static const FName OculusRemote_Home;

		static const FName OculusHand_Left_ThumbPinch;
		static const FName OculusHand_Left_IndexPinch;
		static const FName OculusHand_Left_MiddlePinch;
		static const FName OculusHand_Left_RingPinch;
		static const FName OculusHand_Left_PinkyPinch;

		static const FName OculusHand_Right_ThumbPinch;
		static const FName OculusHand_Right_IndexPinch;
		static const FName OculusHand_Right_MiddlePinch;
		static const FName OculusHand_Right_RingPinch;
		static const FName OculusHand_Right_PinkyPinch;

		static const FName OculusHand_Left_SystemGesture;
		static const FName OculusHand_Right_SystemGesture;

		static const FName OculusHand_Left_ThumbPinchStrength;
		static const FName OculusHand_Left_IndexPinchStrength;
		static const FName OculusHand_Left_MiddlePinchStrength;
		static const FName OculusHand_Left_RingPinchStrength;
		static const FName OculusHand_Left_PinkyPinchStrength;

		static const FName OculusHand_Right_ThumbPinchStrength;
		static const FName OculusHand_Right_IndexPinchStrength;
		static const FName OculusHand_Right_MiddlePinchStrength;
		static const FName OculusHand_Right_RingPinchStrength;
		static const FName OculusHand_Right_PinkyPinchStrength;
	};

	//-------------------------------------------------------------------------------------------------
	// FOculusButtonState -  Digital button state
	//-------------------------------------------------------------------------------------------------

	struct FOculusButtonState
	{
		/** The Unreal button this maps to.  Different depending on whether this is the Left or Right hand controller */
		FName Key;

		/** The Unreal button this maps to.  Different depending on whether this is the Left or Right hand controller */
		FName EmulatedKey;

		/** Whether we're pressed or not.  While pressed, we will generate repeat presses on a timer */
		bool bIsPressed;

		/** Next time a repeat event should be generated for each button */
		double NextRepeatTime;

		/** Default constructor that just sets sensible defaults */
		FOculusButtonState()
			: Key(NAME_None), EmulatedKey(NAME_None), bIsPressed(false), NextRepeatTime(0.0)
		{
		}
	};

	//-------------------------------------------------------------------------------------------------
	// FOculusTouchCapacitiveState - Capacitive Axis State
	//-------------------------------------------------------------------------------------------------

	struct FOculusAxisState
	{
		/** The axis that this button state maps to */
		FName Axis;

		/** How close the finger is to this button, from 0.f to 1.f */
		float State;

		FOculusAxisState()
			: Axis(NAME_None)
			, State(0.f)
		{
		}
	};

	struct FOculusMicrogestureState
	{
		/** The Unreal button this maps to.  Different depending on whether this is the Left or Right hand controller */
		FName Key;

		/** Wether the microgesture was detected this frame */
		bool bIsActive;

		FOculusMicrogestureState()
			: Key(NAME_None), bIsActive(false)
		{
		}
	};

	struct FOculusControllerState
	{
		/** True if the device is connected, otherwise false */
		bool bIsConnected;

		/** True if position is being tracked, otherwise false */
		bool bIsPositionTracked;

		/** True if position is valid (tracked or estimated), otherwise false */
		bool bIsPositionValid;

		/** True if orientation is being tracked, otherwise false */
		bool bIsOrientationTracked;

		/** True if orientation is valid (tracked or estimated), otherwise false */
		bool bIsOrientationValid;

		FOculusControllerState()
			: bIsConnected(false), bIsPositionTracked(false), bIsPositionValid(false), bIsOrientationTracked(false), bIsOrientationValid(false)
		{
		}
	};

	//-------------------------------------------------------------------------------------------------
	// FOculusTouchControllerState - Input state for an Oculus motion controller
	//-------------------------------------------------------------------------------------------------

	struct FOculusTouchControllerState : FOculusControllerState
	{
		/** Analog trigger */
		float TriggerAxis;

		/** Grip trigger */
		float GripAxis;

		/** Thumbstick */
		FVector2D ThumbstickAxes;

		/** Thumbstick */
		FVector2D TouchpadAxes;

		/** Button states */
		FOculusButtonState Buttons[(int32)EOculusTouchControllerButton::TotalButtonCount];

		/** Capacitive Touch axes */
		FOculusAxisState CapacitiveAxes[(int32)EOculusTouchCapacitiveAxes::TotalAxisCount];

		/** Thumb Rest Force **/
		float ThumbRestForce;

		/** Stylus tip**/
		float StylusForce;

		/** Index trigger Curl**/
		float IndexTriggerCurl;

		/** Index trigger Slide**/
		float IndexTriggerSlide;

		/** Second stage Index trigger force **/
		float IndexTriggerForce;

		/** Whether or not we're playing a haptic effect.  If true, force feedback calls will be early-outed in favor of the haptic effect */
		bool bPlayingHapticEffect;

		/** Haptic frequency (zero to disable) */
		float HapticFrequency;

		/** Haptic amplitude (zero to disable) */
		float HapticAmplitude;

		/** Force feedback haptic frequency (zero to disable) */
		float ForceFeedbackHapticFrequency;

		/** Force feedback haptic amplitude (zero to disable) */
		float ForceFeedbackHapticAmplitude;

		/** Number of times that controller was recentered (for mobile controllers) */
		int RecenterCount;

	public:
		FHapticFeedbackBuffer ResampledHapticBuffer;
		void ResampleHapticBufferData(const FHapticFeedbackBuffer& HapticBuffer, TMap<const uint8*, TSharedPtr<TArray<uint8>>>& ResampledRawDataCache);

		/** Explicit constructor sets up sensible defaults */
		FOculusTouchControllerState(const EControllerHand Hand)
			: TriggerAxis(0.0f), GripAxis(0.0f), ThumbstickAxes(FVector2D::ZeroVector), bPlayingHapticEffect(false), HapticFrequency(0.0f), HapticAmplitude(0.0f), ForceFeedbackHapticFrequency(0.0f), ForceFeedbackHapticAmplitude(0.0f), RecenterCount(0)
		{
			for (FOculusButtonState& Button : Buttons)
			{
				Button.bIsPressed = false;
				Button.NextRepeatTime = 0.0;
			}

			Buttons[(int32)EOculusTouchControllerButton::Trigger].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Trigger_Click.GetFName() : EKeys::OculusTouch_Right_Trigger_Click.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::Grip].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Grip_Click.GetFName() : EKeys::OculusTouch_Right_Grip_Click.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::Thumbstick].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Thumbstick_Click.GetFName() : EKeys::OculusTouch_Right_Thumbstick_Click.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::XA].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_X_Click.GetFName() : EKeys::OculusTouch_Right_A_Click.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::YB].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Y_Click.GetFName() : EKeys::OculusTouch_Right_B_Click.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::Thumbstick_Up].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Thumbstick_Up.GetFName() : EKeys::OculusTouch_Right_Thumbstick_Up.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::Thumbstick_Down].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Thumbstick_Down.GetFName() : EKeys::OculusTouch_Right_Thumbstick_Down.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::Thumbstick_Left].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Thumbstick_Left.GetFName() : EKeys::OculusTouch_Right_Thumbstick_Left.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::Thumbstick_Right].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Thumbstick_Right.GetFName() : EKeys::OculusTouch_Right_Thumbstick_Right.GetFName();

			Buttons[(int32)EOculusTouchControllerButton::Menu].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Menu_Click.GetFName() : FName("OculusTouch_Right_System_Click");

			Buttons[(int32)EOculusTouchControllerButton::Thumbstick_Touch].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Thumbstick_Touch.GetFName() : EKeys::OculusTouch_Right_Thumbstick_Touch.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::Trigger_Touch].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Trigger_Touch.GetFName() : EKeys::OculusTouch_Right_Trigger_Touch.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::XA_Touch].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_X_Touch.GetFName() : EKeys::OculusTouch_Right_A_Touch.GetFName();
			Buttons[(int32)EOculusTouchControllerButton::YB_Touch].Key = (Hand == EControllerHand::Left) ? EKeys::OculusTouch_Left_Y_Touch.GetFName() : EKeys::OculusTouch_Right_B_Touch.GetFName();

			CapacitiveAxes[(int32)EOculusTouchCapacitiveAxes::Thumbstick].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusTouch_Left_Thumbstick : FOculusKeyNames::OculusTouch_Right_Thumbstick;
			CapacitiveAxes[(int32)EOculusTouchCapacitiveAxes::Trigger].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusTouch_Left_Trigger : FOculusKeyNames::OculusTouch_Right_Trigger;
			CapacitiveAxes[(int32)EOculusTouchCapacitiveAxes::XA].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusTouch_Left_FaceButton1 : FOculusKeyNames::OculusTouch_Right_FaceButton1;
			CapacitiveAxes[(int32)EOculusTouchCapacitiveAxes::YB].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusTouch_Left_FaceButton2 : FOculusKeyNames::OculusTouch_Right_FaceButton2;
			CapacitiveAxes[(int32)EOculusTouchCapacitiveAxes::IndexPointing].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusTouch_Left_IndexPointing : FOculusKeyNames::OculusTouch_Right_IndexPointing;
			CapacitiveAxes[(int32)EOculusTouchCapacitiveAxes::ThumbUp].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusTouch_Left_ThumbUp : FOculusKeyNames::OculusTouch_Right_ThumbUp;
			CapacitiveAxes[(int32)EOculusTouchCapacitiveAxes::ThumbRest].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusTouch_Left_ThumbRest : FOculusKeyNames::OculusTouch_Right_ThumbRest;
		}

		/** Default constructor does nothing.  Don't use it.  This only exists because we cannot initialize an array of objects with no default constructor on non-C++ 11 compliant compilers (VS 2013) */
		FOculusTouchControllerState()
		{
		}
	};

	//-------------------------------------------------------------------------------------------------
	// FOculusHandControllerState - Input state for an Oculus Hands
	//-------------------------------------------------------------------------------------------------

	struct FOculusHandControllerState : FOculusControllerState
	{
		/** True if the pointer pose for hands is valid */
		bool bIsPointerPoseValid;

		/** True if the current hand is the dominant hand */
		bool bIsDominantHand;

		/** Scale of the hand */
		float HandScale;

		/** Pose of the pointer */
		FTransform PointerPose;

		/** Tracking confidence of hand tracking */
		EOculusXRTrackingConfidence TrackingConfidence;

		/** Finger Pinch States **/
		FOculusButtonState HandButtons[(int32)EOculusHandButton::TotalButtonCount];

		/** Finger Pinch Strength States **/
		FOculusAxisState HandAxes[(int32)EOculusHandAxes::TotalAxisCount];

		/** Finger Pinch Strength States **/
		FOculusMicrogestureState Microgestures[(int32)EOculusMicrogestures::TotalGestureCount];

		/** Finger Confidences **/
		EOculusXRTrackingConfidence FingerConfidences[(int32)EOculusHandAxes::TotalAxisCount] = {};

		FQuat BoneRotations[(int32)EOculusXRBone::Bone_Max];

		FOculusHandControllerState(const EControllerHand Hand)
		{
			TrackingConfidence = EOculusXRTrackingConfidence::Low;
			bIsPointerPoseValid = false;
			bIsDominantHand = false;
			HandScale = 0.0f;
			PointerPose = FTransform::Identity;

			for (FOculusButtonState& Button : HandButtons)
			{
				Button.bIsPressed = false;
				Button.NextRepeatTime = 0.0;
			}

			for (int32 i = 0; i < (int32)EOculusXRBone::Bone_Max; i++)
			{
				BoneRotations[i] = FQuat::Identity;
			}

			HandButtons[(int32)EOculusHandButton::Thumb].Key = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_ThumbPinch : FOculusKeyNames::OculusHand_Right_ThumbPinch;
			HandButtons[(int32)EOculusHandButton::Index].Key = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_IndexPinch : FOculusKeyNames::OculusHand_Right_IndexPinch;
			HandButtons[(int32)EOculusHandButton::Middle].Key = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_MiddlePinch : FOculusKeyNames::OculusHand_Right_MiddlePinch;
			HandButtons[(int32)EOculusHandButton::Ring].Key = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_RingPinch : FOculusKeyNames::OculusHand_Right_RingPinch;
			HandButtons[(int32)EOculusHandButton::Pinky].Key = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_PinkyPinch : FOculusKeyNames::OculusHand_Right_PinkyPinch;
			HandButtons[(int32)EOculusHandButton::System].Key = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_SystemGesture : FOculusKeyNames::OculusHand_Right_SystemGesture;
			HandButtons[(int32)EOculusHandButton::Menu].Key = (Hand == EControllerHand::Left) ? FGamepadKeyNames::SpecialLeft : FGamepadKeyNames::SpecialRight;

			HandAxes[(int32)EOculusHandAxes::Thumb].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_ThumbPinchStrength : FOculusKeyNames::OculusHand_Right_ThumbPinchStrength;
			HandAxes[(int32)EOculusHandAxes::Index].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_IndexPinchStrength : FOculusKeyNames::OculusHand_Right_IndexPinchStrength;
			HandAxes[(int32)EOculusHandAxes::Middle].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_MiddlePinchStrength : FOculusKeyNames::OculusHand_Right_MiddlePinchStrength;
			HandAxes[(int32)EOculusHandAxes::Ring].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_RingPinchStrength : FOculusKeyNames::OculusHand_Right_RingPinchStrength;
			HandAxes[(int32)EOculusHandAxes::Pinky].Axis = (Hand == EControllerHand::Left) ? FOculusKeyNames::OculusHand_Left_PinkyPinchStrength : FOculusKeyNames::OculusHand_Right_PinkyPinchStrength;

			Microgestures[(int32)EOculusMicrogestures::SwipeLeft].Key = (Hand == EControllerHand::Left) ? FOculusKey::OculusHand_Left_MicrogestureSwipeLeft.GetFName() : FOculusKey::OculusHand_Right_MicrogestureSwipeLeft.GetFName();
			Microgestures[(int32)EOculusMicrogestures::SwipeRight].Key = (Hand == EControllerHand::Left) ? FOculusKey::OculusHand_Left_MicrogestureSwipeRight.GetFName() : FOculusKey::OculusHand_Right_MicrogestureSwipeRight.GetFName();
			Microgestures[(int32)EOculusMicrogestures::SwipeForward].Key = (Hand == EControllerHand::Left) ? FOculusKey::OculusHand_Left_MicrogestureSwipeForward.GetFName() : FOculusKey::OculusHand_Right_MicrogestureSwipeForward.GetFName();
			Microgestures[(int32)EOculusMicrogestures::SwipeBack].Key = (Hand == EControllerHand::Left) ? FOculusKey::OculusHand_Left_MicrogestureSwipeBack.GetFName() : FOculusKey::OculusHand_Right_MicrogestureSwipeBack.GetFName();
			Microgestures[(int32)EOculusMicrogestures::TapThumb].Key = (Hand == EControllerHand::Left) ? FOculusKey::OculusHand_Left_MicrogestureTapThumb.GetFName() : FOculusKey::OculusHand_Right_MicrogestureTapThumb.GetFName();
		}

		FOculusHandControllerState()
		{
		}
	};

	//-------------------------------------------------------------------------------------------------
	// FOculusRemoteControllerState
	//-------------------------------------------------------------------------------------------------

	struct FOculusRemoteControllerState
	{
		/** Button states */
		FOculusButtonState Buttons[(int32)EOculusRemoteControllerButton::TotalButtonCount];

		FOculusRemoteControllerState()
		{
			for (FOculusButtonState& Button : Buttons)
			{
				Button.bIsPressed = false;
				Button.NextRepeatTime = 0.0;
			}

			Buttons[(int32)EOculusRemoteControllerButton::DPad_Up].Key = FOculusKeyNames::OculusRemote_DPad_Up;
			Buttons[(int32)EOculusRemoteControllerButton::DPad_Down].Key = FOculusKeyNames::OculusRemote_DPad_Down;
			Buttons[(int32)EOculusRemoteControllerButton::DPad_Left].Key = FOculusKeyNames::OculusRemote_DPad_Left;
			Buttons[(int32)EOculusRemoteControllerButton::DPad_Right].Key = FOculusKeyNames::OculusRemote_DPad_Right;
			Buttons[(int32)EOculusRemoteControllerButton::Enter].Key = FOculusKeyNames::OculusRemote_Enter;
			Buttons[(int32)EOculusRemoteControllerButton::Back].Key = FOculusKeyNames::OculusRemote_Back;

			Buttons[(int32)EOculusRemoteControllerButton::VolumeUp].Key = FOculusKeyNames::OculusRemote_VolumeUp;
			Buttons[(int32)EOculusRemoteControllerButton::VolumeDown].Key = FOculusKeyNames::OculusRemote_VolumeDown;
			Buttons[(int32)EOculusRemoteControllerButton::Home].Key = FOculusKeyNames::OculusRemote_Home;
		}

		void MapKeysToGamepad()
		{
			Buttons[(int32)EOculusRemoteControllerButton::DPad_Up].EmulatedKey = FGamepadKeyNames::DPadUp;
			Buttons[(int32)EOculusRemoteControllerButton::DPad_Down].EmulatedKey = FGamepadKeyNames::DPadDown;
			Buttons[(int32)EOculusRemoteControllerButton::DPad_Left].EmulatedKey = FGamepadKeyNames::DPadLeft;
			Buttons[(int32)EOculusRemoteControllerButton::DPad_Right].EmulatedKey = FGamepadKeyNames::DPadRight;
			Buttons[(int32)EOculusRemoteControllerButton::Enter].EmulatedKey = FGamepadKeyNames::SpecialRight;
			Buttons[(int32)EOculusRemoteControllerButton::Back].EmulatedKey = FGamepadKeyNames::SpecialLeft;
		}
	};

	//-------------------------------------------------------------------------------------------------
	// FOculusControllerPair - A pair of Oculus controllers, hand/touch, one for either hand
	//-------------------------------------------------------------------------------------------------

	struct FOculusControllerPair
	{
		/** The input device ID for this oculus controller */
		FInputDeviceId DeviceId;

		/** Current device state for either hand */
		FOculusTouchControllerState TouchControllerStates[2];

		FOculusHandControllerState HandControllerStates[2];

		FOculusControllerPair()
			: DeviceId(INPUTDEVICEID_NONE), TouchControllerStates(), HandControllerStates()
		{
			TouchControllerStates[(int32)EControllerHand::Left] = FOculusTouchControllerState(EControllerHand::Left);
			TouchControllerStates[(int32)EControllerHand::Right] = FOculusTouchControllerState(EControllerHand::Right);

			HandControllerStates[(int32)EControllerHand::Left] = FOculusHandControllerState(EControllerHand::Left);
			HandControllerStates[(int32)EControllerHand::Right] = FOculusHandControllerState(EControllerHand::Right);
		}
	};

	const float DefaultInitialButtonRepeatDelay = 0.2f;
	const float DefaultButtonRepeatDelay = 0.1f;

} // namespace OculusXRInput

#endif // OCULUS_INPUT_SUPPORTED_PLATFORMS
