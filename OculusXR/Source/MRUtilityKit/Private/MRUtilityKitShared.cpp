// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "Generated/MRUtilityKitShared.h"
#include "MRUtilityKit.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"

MRUKShared* MRUKShared::Instance;

MRUKShared::MRUKShared(void* handle)
	: MRUKSharedHandle(handle)
{
	LoadNativeFunctions();
}

MRUKShared::~MRUKShared()
{
	UnloadNativeFunctions();

	FPlatformProcess::FreeDllHandle(MRUKSharedHandle);
	MRUKSharedHandle = nullptr;
}

void MRUKShared::LoadMRUKSharedLibrary()
{
	if (Instance != nullptr)
	{
		return;
	}

	// Load
	UE_LOG(LogMRUK, Log, TEXT("Loading MR Utility Kit Shared library"));
#if PLATFORM_WINDOWS
	const FString BinariesPath = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("OculusXR"))->GetBaseDir(), TEXT("/Source/Thirdparty/MRUtilityKitShared/Lib/Win64"));
	FPlatformProcess::PushDllDirectory(*BinariesPath);
	void* handle = FPlatformProcess::GetDllHandle(TEXT("mrutilitykitshared.dll"));
	FPlatformProcess::PopDllDirectory(*BinariesPath);
#elif PLATFORM_ANDROID
	void* handle = FPlatformProcess::GetDllHandle(TEXT("libmrutilitykitshared.so"));
#endif // PLATFORM_ANDROID

	if (handle == nullptr)
	{
		UE_LOG(LogMRUK, Error, TEXT("Failed to load MR Utility Kit Shared library"));
		return;
	}

	Instance = new MRUKShared(handle);
}

void MRUKShared::FreeMRUKSharedLibrary()
{
	if (Instance == nullptr)
	{
		return;
	}
	delete Instance;
	Instance = nullptr;
}

void* MRUKShared::LoadFunction(const TCHAR* ProcName)
{
	auto func = FPlatformProcess::GetDllExport(MRUKSharedHandle, ProcName);
	if (func == nullptr)
	{
		UE_LOG(LogMRUK, Error, TEXT("Failed to load native function: %s"), ProcName);
	}
	return func;
}
