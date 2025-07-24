// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS

#include "Widgets/Notifications/SNotificationList.h"

DEFINE_LOG_CATEGORY_STATIC(LogMetaXRSim, Log, All);

/**  */
class FMetaXRSimulator
{
public:
	static bool IsSimulatorActivated();
	static void ToggleOpenXRRuntime();
	static void TryActivateOnStartup();
	static FString GetPackagePath();
	static bool IsSimulatorInstalled();

private:
	static FString GetSimulatorJsonPath();
	static void InstallSimulator(const TFunction<void()>& OnSuccess);
	static FString GetVersion();
	static void UnzipSimulator(const FString& Path, const FString& TargetPath, const TSharedPtr<SNotificationItem>& Notification, const TFunction<void()>& OnSuccess);
};
#endif
