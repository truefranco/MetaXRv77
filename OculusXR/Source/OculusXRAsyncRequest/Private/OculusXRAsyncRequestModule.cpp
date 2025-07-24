// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRAsyncRequestModule.h"

#if OCULUS_ASYNC_REQUEST_SUPPORTED_PLATFORMS

DEFINE_LOG_CATEGORY(LogOculusXRAsyncRequest);

#define LOCTEXT_NAMESPACE "OculusXRAsyncRequest"

void FOculusXRAsyncRequestModule::StartupModule()
{
}

void FOculusXRAsyncRequestModule::ShutdownModule()
{
}

#endif // OCULUS_ASYNC_REQUEST_SUPPORTED_PLATFORMS

IMPLEMENT_MODULE(FOculusXRAsyncRequestModule, OculusXRAsyncRequest)

#undef LOCTEXT_NAMESPACE
