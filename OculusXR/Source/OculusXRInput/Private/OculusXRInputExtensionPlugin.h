// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "khronos/openxr/openxr.h"

#include "CoreMinimal.h"
#include "IOculusXRInputModule.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRInput.h"
#include "Misc/EngineVersionComparison.h"

namespace OculusXRInput
{
	enum FDerivedActionProfile
	{
		All,
		OculusTouch,
		OculusTouchPro,
		OculusTouchPlus
	};

	struct FDerivedActionProperties
	{
		FString Name;
		XrActionType Type;
		FKey InputKey;
		FString OpenXRPath;
		XrAction Action;
		FDerivedActionProfile Profile;
	};

	struct FDerivedInputState
	{
		bool ValueBool;
		float ValueFloat;
		bool ChangedSinceLastSync;
		double NextRepeatTime;

		FDerivedInputState()
			: ValueBool(false), ValueFloat(0.f), ChangedSinceLastSync(false), NextRepeatTime(0.0)
		{
		}
	};

	class FInputExtensionPlugin : public IOpenXRExtensionPlugin, public IInputDevice
	{
	public:
		FInputExtensionPlugin()
			: InitialButtonRepeatDelay(DefaultInitialButtonRepeatDelay), ButtonRepeatDelay(DefaultButtonRepeatDelay), MessageHandler(nullptr), bExtTouchControllerProximityAvailable(false), Instance(XR_NULL_HANDLE), DerivedActions({}), DerivedKeysToState({}), DerivedActionSet(XR_NULL_HANDLE)
		{
		}

		void RegisterOpenXRExtensionPlugin()
		{
#if !UE_VERSION_OLDER_THAN(5, 5, 0)
			RegisterOpenXRExtensionModularFeature();
#endif
		}

		// IInputDevice
		virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
		void Tick(float DeltaTime) override {};
		void SendControllerEvents() override {};
		virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
		void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override {};
		void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override {};

	private:
		const FString OculusTouchProfile = TEXT("OculusTouch");
		const FString OculusTouchProProfile = TEXT("OculusTouchPro");
		const FString OculusTouchPlusProfile = TEXT("OculusTouchPlus");
		const FString OculusTouchProfilePath = TEXT("/interaction_profiles/oculus/touch_controller");
		const FString OculusTouchProProfilePath = TEXT("/interaction_profiles/facebook/touch_controller_pro");
		const FString OculusTouchPlusProfilePath = TEXT("/interaction_profiles/meta/touch_controller_plus");
		const TSet<FKey> KeysToInvert = { FOculusKey::OculusTouch_Left_IndexPointing, FOculusKey::OculusTouch_Right_IndexPointing, FOculusKey::OculusTouch_Left_ThumbUp, FOculusKey::OculusTouch_Right_ThumbUp };

		float InitialButtonRepeatDelay;
		float ButtonRepeatDelay;

		TSharedPtr<FGenericApplicationMessageHandler> MessageHandler;
		bool bExtTouchControllerProximityAvailable;
		XrInstance Instance;

		TArray<FDerivedActionProperties> DerivedActions;
		TMap<FKey, FDerivedInputState> DerivedKeysToState;
		XrActionSet DerivedActionSet;

#if !UE_VERSION_OLDER_THAN(5, 5, 0)
	public:
		// IOpenXRExtensionPlugin
		virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual const void* OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext) override;
		virtual void OnEvent(XrSession InSession, const XrEventDataBaseHeader* InHeader) override;
		virtual bool GetInputKeyOverrides(TArray<FInputKeyOpenXRProperties>& OutOverrides) override;
		virtual void PostCreateInstance(XrInstance InInstance) override;
		virtual bool GetSuggestedBindings(XrPath InInteractionProfile, TArray<XrActionSuggestedBinding>& OutBindings) override;
		virtual void PostSyncActions(XrSession InSession) override;
		virtual void AttachActionSets(TSet<XrActionSet>& OutActionSets) override;
		virtual void GetActiveActionSetsForSync(TArray<XrActiveActionSet>& OutActiveSets);

	private:
		virtual void CreateForAllProfiles(TArray<FInputKeyOpenXRProperties>& OutOverrides, FKey InKey, FString Path);
		virtual void CreateDerivedActions();
		virtual void InitializeDerivedActionsArray();
		virtual void SendControllerButtonPressed(FKey InKey, bool bIsPressed, FPlatformUserId UserId, FInputDeviceId DeviceId, bool bIsRepeat);
		virtual void SendDerivedDpadButtonPressed(FKey InKey, bool bIsPressed, FPlatformUserId UserId, FInputDeviceId DeviceId, double CurrentTime);
		virtual void DestroyDerivedActions();
#endif
	};
} // namespace OculusXRInput
