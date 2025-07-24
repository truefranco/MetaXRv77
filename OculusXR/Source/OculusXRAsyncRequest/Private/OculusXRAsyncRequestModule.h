// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IOculusXRAsyncRequestModule.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOculusXRAsyncRequest, Log, All);

#define LOCTEXT_NAMESPACE "OculusXRAsyncRequest"

#if OCULUS_ASYNC_REQUEST_SUPPORTED_PLATFORMS

class FOculusXRAsyncRequestModule : public IOculusXRAsyncRequestModule
{
public:
	virtual ~FOculusXRAsyncRequestModule() = default;

	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#else // OCULUS_ASYNC_REQUEST_SUPPORTED_PLATFORMS

class FOculusXRAsyncRequestModule : public FDefaultModuleImpl
{
};

#endif // OCULUS_ASYNC_REQUEST_SUPPORTED_PLATFORMS

#undef LOCTEXT_NAMESPACE
