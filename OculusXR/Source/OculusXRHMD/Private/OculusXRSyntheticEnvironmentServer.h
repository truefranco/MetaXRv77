// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#if PLATFORM_WINDOWS
DEFINE_LOG_CATEGORY_STATIC(LogMetaXRSES, Log, All);

struct FProcHandle;

/**  */
class FMetaXRSES
{
public:
	static void LaunchEnvironment(int32 EnvironmentIndex);
	static void StopServer();

	struct ServerInfo
	{
		FString GuiName;
		FString SynthName;
		FString Executable;
	};

	OCULUSXRHMD_API static TArray<ServerInfo>& GetSynthEnvRooms();

private:
	static void LaunchLocalSharingServer();
	static bool LaunchProcess(FString BinaryPath, FString Arguments, FString LogContext, FProcHandle& OutProcHandle);
	static void StopProcess(FProcHandle& ProcHandle, FString LogContext);

	static FString GetLocalSharingServerPath();

	static FProcHandle EnvProcHandle;
	static FProcHandle LSSProcHandle;

	static TArray<ServerInfo> SynthEnvRooms;
	static bool SynthEnvParsed;
};
#endif
