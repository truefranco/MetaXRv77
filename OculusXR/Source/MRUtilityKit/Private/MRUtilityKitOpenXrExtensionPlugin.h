// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include <functional>
#include <mutex>

#include "khronos/openxr/openxr.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRHMDTypes.h"
#include "Shader.h"
#include "Misc/EngineVersionComparison.h"

class FMRUKOpenXrExtensionPlugin : public IOpenXRExtensionPlugin
{
public:
	void (*OpenXrEventHandler)(void* Data, void* Context) = nullptr;
	void* Context = nullptr;

	void RegisterAsOpenXRExtension();

	virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;

	virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;

	virtual void OnEvent(XrSession InSession, const XrEventDataBaseHeader* InHeader) override;
};
