// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRCompatibilityRules.h"
#include "CoreMinimal.h"
#include "AndroidRuntimeSettings.h"
#include "AndroidSDKSettings.h"
#include "GeneralProjectSettings.h"
#include "ISettingsModule.h"
#include "ISettingsCategory.h"
#include "ISettingsContainer.h"
#include "ISettingsSection.h"
#include "OculusXRRuleProcessorSubsystem.h"
#include "GameFramework/InputSettings.h"
#include "OculusXRHMDRuntimeSettings.h"
#include "OculusXRPSTUtils.h"
#include "Misc/EngineVersionComparison.h"

#define LOCTEXT_NAMESPACE "OculusXRCompatibilityRules"
namespace
{
	constexpr int32 MinimumAndroidAPILevel = 32; // With Quest 1 support ending in Jan 2025, API level 29 is no longer supported.
	constexpr int32 TargetAndroidAPILevel = 32;	 // Target API 32 or higher is required to submit to the Meta Quest Store. See https://developer.oculus.com/blog/meta-quest-apps-android-12l-june-30/
	constexpr char AndroidNDKVersionNumber[] = "25.1.8937393";

// SDK Max API level is determined by reviewing SetupAndroid.bat for each UE version.
// SDK Min API Level is determined by Quest Store requirements as noted above.
#if UE_VERSION_OLDER_THAN(5, 4, 0)
	constexpr char AndroidSDKAPIMinLevel[] = "android-32";
	constexpr char AndroidSDKAPIMaxLevel[] = "android-32";
	constexpr int32 AndroidSDKAPIMinLevelInt = 32;
	constexpr int32 AndroidSDKAPIMaxLevelInt = 32;

	constexpr char AndroidNDKAPIMinLevel[] = "android-32";
	constexpr char AndroidNDKAPIMaxLevel[] = "android-32";
	constexpr int32 AndroidNDKAPIMinLevelInt = 32;
	constexpr int32 AndroidNDKAPIMaxLevelInt = 32;
#elif UE_VERSION_OLDER_THAN(5, 5, 0)
	constexpr char AndroidSDKAPIMinLevel[] = "android-32";
	constexpr char AndroidSDKAPIMaxLevel[] = "android-33";
	constexpr int32 AndroidSDKAPIMinLevelInt = 32;
	constexpr int32 AndroidSDKAPIMaxLevelInt = 33;

	constexpr char AndroidNDKAPIMinLevel[] = "android-32";
	constexpr char AndroidNDKAPIMaxLevel[] = "android-33";
	constexpr int32 AndroidNDKAPIMinLevelInt = 32;
	constexpr int32 AndroidNDKAPIMaxLevelInt = 33;
#else // 5.5 and newer
	constexpr char AndroidSDKAPIMinLevel[] = "android-32";
	constexpr char AndroidSDKAPIMaxLevel[] = "android-34";
	constexpr int32 AndroidSDKAPIMinLevelInt = 32;
	constexpr int32 AndroidSDKAPIMaxLevelInt = 34;

	constexpr char AndroidNDKAPIMinLevel[] = "android-32";
	constexpr char AndroidNDKAPIMaxLevel[] = "android-33"; // API level 34 is not supported by NDK 25.1.8937393
	constexpr int32 AndroidNDKAPIMinLevelInt = 32;
	constexpr int32 AndroidNDKAPIMaxLevelInt = 33;
#endif
} // namespace

namespace OculusXRCompatibilityRules
{

	FUseAndroidSDKMinimumRule::FUseAndroidSDKMinimumRule()
		: ISetupRule(
			  "Compatibility_UseAndroidSDKMinimum",
			  LOCTEXT("UseAndroidSDKMinimum_DisplayName", "Use Android SDK Minimum Version"),
			  FText::Format(
				  LOCTEXT("UseAndroidSDKMinimum_Description", "Minimum Android API level must be at least {0}."),
				  MinimumAndroidAPILevel),
			  ESetupRuleCategory::Compatibility,
			  ESetupRuleSeverity::Critical,
			  MetaQuest_All) {}

	bool FUseAndroidSDKMinimumRule::IsApplied() const
	{
		const UAndroidRuntimeSettings* Settings = GetMutableDefault<UAndroidRuntimeSettings>();

		return Settings->MinSDKVersion >= MinimumAndroidAPILevel;
	}

	void FUseAndroidSDKMinimumRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OCULUSXR_UPDATE_SETTINGS(UAndroidRuntimeSettings, MinSDKVersion, MinimumAndroidAPILevel);
		OutShouldRestartEditor = false;
	}

	FUseAndroidSDKTargetRule::FUseAndroidSDKTargetRule()
		: ISetupRule(
			  "Compatibility_UseAndroidSDKTarget",
			  LOCTEXT("UseAndroidSDKTarget_DisplayName", "Use Android SDK Target Version"),
			  FText::Format(
				  LOCTEXT("UseAndroidSDKTarget_Description", "Target Android API level must be at least {0}."),
				  TargetAndroidAPILevel),
			  ESetupRuleCategory::Compatibility,
			  ESetupRuleSeverity::Critical,
			  MetaQuest_All) {}

	bool FUseAndroidSDKTargetRule::IsApplied() const
	{
		const UAndroidRuntimeSettings* Settings = GetMutableDefault<UAndroidRuntimeSettings>();

		return Settings->TargetSDKVersion >= TargetAndroidAPILevel;
	}

	void FUseAndroidSDKTargetRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OCULUSXR_UPDATE_SETTINGS(UAndroidRuntimeSettings, TargetSDKVersion, TargetAndroidAPILevel);
		OutShouldRestartEditor = false;
	}
	FUseAndroidSDKLevelRule::FUseAndroidSDKLevelRule()
		: ISetupRule(
			  "Compatibility_UseAndroidSDKLevel",
			  LOCTEXT("UseAndroidSDKLevel_DisplayName", "Use Android SDK Level"),
			  FText::Format(
				  LOCTEXT("UseAndroidSDKLevel_Description", "Android SDK level should be set between {0} and {1} prior to packaging apks."),
				  FText::AsCultureInvariant(AndroidSDKAPIMinLevel), FText::AsCultureInvariant(AndroidSDKAPIMaxLevel)),
			  TEXT("https://developer.oculus.com/blog/meta-quest-apps-android-12l-june-30/"),
			  ESetupRuleCategory::Compatibility,
			  ESetupRuleSeverity::Critical,
			  MetaQuest_All,
			  true) {}

	bool FUseAndroidSDKLevelRule::IsApplied() const
	{
		const UAndroidSDKSettings* Settings = GetMutableDefault<UAndroidSDKSettings>();
		FString SDKAPILevel = Settings->SDKAPILevel;

		if (SDKAPILevel.IsEmpty())
		{
			return false;
		}
		if (SDKAPILevel.Equals(TEXT("latest")) || SDKAPILevel.Equals(TEXT("matchndk")))
		{
			return true;
		}
		if (!SDKAPILevel.Left(8).Equals(TEXT("android-")))
		{
			return false;
		}
		if (FCString::Atoi(*SDKAPILevel.Right(2)) < AndroidSDKAPIMinLevelInt)
		{
			return false;
		}
		if (FCString::Atoi(*SDKAPILevel.Right(2)) > AndroidSDKAPIMaxLevelInt)
		{
			return false;
		}

		return true;
	}

	void SaveSDKSettings()
	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (!SettingsModule)
		{
			return;
		}
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");
		if (!SettingsContainer.IsValid())
		{
			return;
		}
		ISettingsCategoryPtr SettingsCategory = SettingsContainer->GetCategory("Platforms");
		if (!SettingsCategory.IsValid())
		{
			return;
		}
		ISettingsSectionPtr SettingsSection = SettingsCategory->GetSection("AndroidSDK");
		if (!SettingsSection.IsValid())
		{
			return;
		}
		TWeakObjectPtr<UObject> SettingsObject = SettingsSection->GetSettingsObject();
		if (!SettingsObject.IsValid())
		{
			return;
		}

		SettingsObject->UpdateGlobalUserConfigFile();
	}

	void FUseAndroidSDKLevelRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OutShouldRestartEditor = false;
		OCULUSXR_UPDATE_SETTINGS(UAndroidSDKSettings, SDKAPILevel, FText::AsCultureInvariant(AndroidSDKAPIMinLevel).ToString());
		SaveSDKSettings();
	}

	FUseAndroidNDKLevelRule::FUseAndroidNDKLevelRule()
		: ISetupRule(
			  "Compatibility_UseAndroidNDKLevel",
			  LOCTEXT("UseAndroidNDKLevel_DisplayName", "Use Android NDK Level"),
			  FText::Format(
				  LOCTEXT("UseAndroidNDKLevel_Description", "Android NDK level should be set between {0} and {1} prior to packaging apks."),
				  FText::AsCultureInvariant(AndroidNDKAPIMinLevel), FText::AsCultureInvariant(AndroidNDKAPIMaxLevel)),
			  TEXT("https://developer.oculus.com/blog/meta-quest-apps-must-target-android-10-starting-september-29/"),
			  ESetupRuleCategory::Compatibility,
			  ESetupRuleSeverity::Critical,
			  MetaQuest_All,
			  true) {}

	bool FUseAndroidNDKLevelRule::IsApplied() const
	{
		const UAndroidSDKSettings* Settings = GetMutableDefault<UAndroidSDKSettings>();
		FString NDKAPILevel = Settings->NDKAPILevel;

		if (NDKAPILevel.IsEmpty())
		{
			return false;
		}
		if (NDKAPILevel.Equals(TEXT("latest")))
		{
			return true;
		}
		if (!NDKAPILevel.Left(8).Equals(TEXT("android-")))
		{
			return false;
		}
		if (FCString::Atoi(*NDKAPILevel.Right(2)) < AndroidNDKAPIMinLevelInt)
		{
			return false;
		}
		if (FCString::Atoi(*NDKAPILevel.Right(2)) > AndroidNDKAPIMaxLevelInt)
		{
			return false;
		}

		return true;
	}

	void FUseAndroidNDKLevelRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OutShouldRestartEditor = false;
		OCULUSXR_UPDATE_SETTINGS(UAndroidSDKSettings, NDKAPILevel, FText::AsCultureInvariant(AndroidNDKAPIMinLevel).ToString());
		SaveSDKSettings();
	}

	bool FUseArm64CPURule::IsApplied() const
	{
		const UAndroidRuntimeSettings* Settings = GetMutableDefault<UAndroidRuntimeSettings>();

		return Settings->bBuildForArm64 && !Settings->bBuildForX8664;
	}

	void FUseArm64CPURule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OCULUSXR_UPDATE_SETTINGS(UAndroidRuntimeSettings, bBuildForArm64, true);
		OCULUSXR_UPDATE_SETTINGS(UAndroidRuntimeSettings, bBuildForX8664, false);
		OutShouldRestartEditor = false;
	}
	bool FEnablePackageForMetaQuestRule::IsApplied() const
	{
		const UAndroidRuntimeSettings* Settings = GetMutableDefault<UAndroidRuntimeSettings>();

		return Settings->bPackageForMetaQuest && !Settings->bSupportsVulkanSM5 && !Settings->bBuildForES31 && Settings->ExtraApplicationSettings.Find("com.oculus.supportedDevices") != INDEX_NONE;
	}

	void FEnablePackageForMetaQuestRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OCULUSXR_UPDATE_SETTINGS(UAndroidRuntimeSettings, bPackageForMetaQuest, true);
		OCULUSXR_UPDATE_SETTINGS(UAndroidRuntimeSettings, bSupportsVulkanSM5, false);
		OCULUSXR_UPDATE_SETTINGS(UAndroidRuntimeSettings, bBuildForES31, false);

		UAndroidRuntimeSettings* Settings = GetMutableDefault<UAndroidRuntimeSettings>();
		if (Settings->ExtraApplicationSettings.Find("com.oculus.supportedDevices") == INDEX_NONE)
		{
			const FString SupportedDevicesValue("quest|quest2|questpro");
			Settings->ExtraApplicationSettings.Append("<meta-data android:name=\"com.oculus.supportedDevices\" android:value=\"" + SupportedDevicesValue + "\" />");
			Settings->UpdateSinglePropertyInConfigFile(Settings->GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UAndroidRuntimeSettings, ExtraApplicationSettings)), Settings->GetDefaultConfigFilename());
		}

		OutShouldRestartEditor = false;
	}

	bool FQuest2SupportedDeviceRule::IsApplied() const
	{
		const UOculusXRHMDRuntimeSettings* Settings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();

		return Settings->SupportedDevices.Contains(EOculusXRSupportedDevices::Quest2);
	}

	void FQuest2SupportedDeviceRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		UOculusXRHMDRuntimeSettings* Settings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();

		Settings->SupportedDevices.Add(EOculusXRSupportedDevices::Quest2);
		// UpdateSinglePropertyInConfigFile does not support arrays
		Settings->TryUpdateDefaultConfigFile();
		OutShouldRestartEditor = false;
	}

	bool FQuestProSupportedDeviceRule::IsApplied() const
	{
		const UOculusXRHMDRuntimeSettings* Settings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();

		return Settings->SupportedDevices.Contains(EOculusXRSupportedDevices::QuestPro);
	}

	void FQuestProSupportedDeviceRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		UOculusXRHMDRuntimeSettings* Settings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();

		Settings->SupportedDevices.Add(EOculusXRSupportedDevices::QuestPro);
		// UpdateSinglePropertyInConfigFile does not support arrays
		Settings->TryUpdateDefaultConfigFile();
		OutShouldRestartEditor = false;
	}

	bool FQuest3SupportedDeviceRule::IsApplied() const
	{
		const UOculusXRHMDRuntimeSettings* Settings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();

		return Settings->SupportedDevices.Contains(EOculusXRSupportedDevices::Quest3);
	}

	void FQuest3SupportedDeviceRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		UOculusXRHMDRuntimeSettings* Settings = GetMutableDefault<UOculusXRHMDRuntimeSettings>();

		Settings->SupportedDevices.Add(EOculusXRSupportedDevices::Quest3);
		// UpdateSinglePropertyInConfigFile does not support arrays
		Settings->TryUpdateDefaultConfigFile();
		OutShouldRestartEditor = false;
	}

	bool FEnableFullscreenRule::IsApplied() const
	{
		const UAndroidRuntimeSettings* Settings = GetMutableDefault<UAndroidRuntimeSettings>();

		return Settings->bFullScreen;
	}

	void FEnableFullscreenRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OCULUSXR_UPDATE_SETTINGS(UAndroidRuntimeSettings, bFullScreen, true);
		OutShouldRestartEditor = false;
	}

	bool FEnableStartInVRRule::IsApplied() const
	{
		const UGeneralProjectSettings* Settings = GetDefault<UGeneralProjectSettings>();

		return Settings->bStartInVR != 0;
	}

	void FEnableStartInVRRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OCULUSXR_UPDATE_SETTINGS(UGeneralProjectSettings, bStartInVR, true);
		OutShouldRestartEditor = false;
	}

	bool FDisableTouchInterfaceRule::IsApplied() const
	{
		const UInputSettings* Settings = GetDefault<UInputSettings>();

		return Settings->DefaultTouchInterface.IsNull();
	}

	void FDisableTouchInterfaceRule::ApplyImpl(bool& OutShouldRestartEditor)
	{
		OCULUSXR_UPDATE_SETTINGS(UInputSettings, DefaultTouchInterface, nullptr);
		OutShouldRestartEditor = false;
	}
} // namespace OculusXRCompatibilityRules

#undef LOCTEXT_NAMESPACE
