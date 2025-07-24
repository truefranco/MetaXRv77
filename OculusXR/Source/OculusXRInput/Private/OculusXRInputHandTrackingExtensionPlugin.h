// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "khronos/openxr/openxr.h"

#include "CoreMinimal.h"
#include "IOculusXRInputModule.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRInputHandTrackingTypes.h"
#include "OculusXRInputState.h"

namespace OculusXRInput
{
	struct FMicrogestureActionProperties
	{
		FString Name;
		FKey InputKey;
		FString PathString;
		XrAction Action = XR_NULL_HANDLE;
		XrPath Path = XR_NULL_PATH;
	};

	class FHandTrackingExtensionPlugin : public IOpenXRExtensionPlugin, public IInputDevice
	{
	public:
		FHandTrackingExtensionPlugin()
		{
			HandControllerStates.Add(EOculusXRHandType::HandLeft, FOculusHandControllerState(EControllerHand::Left));
			HandControllerStates.Add(EOculusXRHandType::HandRight, FOculusHandControllerState(EControllerHand::Right));
			memset(XrLeftHandJointLocations, 0, sizeof(XrLeftHandJointLocations));
			memset(XrRightHandJointLocations, 0, sizeof(XrRightHandJointLocations));
			memset(XrLeftHandJointVelocities, 0, sizeof(XrLeftHandJointVelocities));
			memset(XrRightHandJointVelocities, 0, sizeof(XrRightHandJointVelocities));
		}

		void RegisterOpenXRExtensionPlugin()
		{
			RegisterOpenXRExtensionModularFeature();
		}

		// IInputDevice
		virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
		void Tick(float DeltaTime) override {};
		void SendControllerEvents() override {};
		virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
		void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override {};
		void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override {};

		// IOpenXRExtensionPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual void AttachActionSets(TSet<XrActionSet>& OutActionSets) override;
		virtual void GetActiveActionSetsForSync(TArray<XrActiveActionSet>& OutActiveSets) override;
		virtual const void* OnGetSystem(XrInstance InInstance, const void* InNext) override;
		virtual const void* OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext) override;
		virtual void PostCreateInstance(XrInstance InInstance) override;
		virtual const void* OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext) override;
		virtual const void* OnBeginSession(XrSession InSession, const void* InNext) override;
		virtual void UpdateDeviceLocations(XrSession InSession, XrTime DisplayTime, XrSpace TrackingSpace) override;
		virtual void PostSyncActions(XrSession InSession) override;
		virtual void OnDestroySession(XrSession InSession) override;

		virtual bool IsHandTrackingAvailable();
		virtual bool IsHandTrackingActive();
		virtual bool GetSkeleton(EOculusXRHandType HandType, TSharedPtr<FHandSkeleton> Skeleton);
		virtual bool GetMesh(EOculusXRHandType HandType, TSharedPtr<FHandMesh> Mesh);

		TMap<EOculusXRHandType, FOculusHandControllerState> HandControllerStates;
		bool bIsInitialized = false;

	private:
		virtual void SendControllerButtonPressed(FName Key, bool IsPressed, FPlatformUserId UserId, FInputDeviceId DeviceId);
		void CreateActions();
		void DestroyActions();

		TArray<FMicrogestureActionProperties> MicrogestureActions;

		XrActionSet MicrogestureActionSet = XR_NULL_HANDLE;

		XrPath XrPathHandInteractionProfile = XR_NULL_PATH;

		XrHandJointLocationEXT XrLeftHandJointLocations[XR_HAND_JOINT_COUNT_EXT];
		XrHandJointLocationEXT XrRightHandJointLocations[XR_HAND_JOINT_COUNT_EXT];
		XrHandJointVelocityEXT XrLeftHandJointVelocities[XR_HAND_JOINT_COUNT_EXT];
		XrHandJointVelocityEXT XrRightHandJointVelocities[XR_HAND_JOINT_COUNT_EXT];

		PFN_xrGetHandMeshFB xrGetHandMeshFB = nullptr;
		PFN_xrLocateHandJointsEXT xrLocateHandJointsEXT = nullptr;
		PFN_xrCreateHandTrackerEXT xrCreateHandTrackerEXT = nullptr;
		PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerEXT = nullptr;

		virtual XrPosef TransformXrHandJointPose(EOculusXRHandType HandType, XrHandJointEXT Joint, XrPosef JointPose, XrPosef RootPose, bool ApplyRootPose);
		virtual XrHandJointEXT MapBoneToXrHandJoint(EHandBoneId Bone);
		virtual EHandBoneId MapXrHandJointToBone(XrHandJointEXT Joint);

		virtual FVector XrPoseVectorToFVector(XrVector3f XrVector);
		virtual FQuat XrPoseQuatToFQuat(XrQuaternionf XrQuat);
		virtual FQuat XrBoneQuatToFQuat(XrQuaternionf XrQuat);

		bool bExtHandTrackingMeshAvailable = false;
		bool bExtMicrogesturesAvailable = false;
		bool bHandTrackingAvailable = false;
		bool bHandTrackingActive = false;

		TSharedPtr<FGenericApplicationMessageHandler> MessageHandler;
		TMap<EOculusXRHandType, XrHandTrackerEXT> OculusHandTrackers;
		XrInstance Instance = XR_NULL_HANDLE;
		FInternalHandsState InternalHandsState;
	};
} // namespace OculusXRInput
