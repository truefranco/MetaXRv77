// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "khronos/openxr/openxr.h"
#include "RHI.h"
#include "HeadMountedDisplayTypes.h"
#include "OpenXRCore.h"
#include "IXRTrackingSystem.h"
#include "Engine/Engine.h"

#define ENSURE_XRCMD(cmd) \
	XR_ENSURE(cmd)

namespace OculusXR
{
	template <typename T>
	static void XRGetInstanceProcAddr(XrInstance InInstance, const char* Name, T* Function)
	{
		if (XR_FAILED(OpenXRDynamicAPI::xrGetInstanceProcAddr(InInstance, Name, reinterpret_cast<PFN_xrVoidFunction*>(Function))))
		{
			UE_LOG(LogHMD, Fatal, TEXT("Failed to bind OpenXR entry %s."), ANSI_TO_TCHAR(Name));
		}
	}

	template <typename T>
	static void XRGetInstanceProcAddr(XrInstance InInstance, const char* Name, TOptional<T>* Function)
	{
		if (XR_FAILED(OpenXRDynamicAPI::xrGetInstanceProcAddr(InInstance, Name, reinterpret_cast<PFN_xrVoidFunction*>(Function))))
		{
			UE_LOG(LogHMD, Warning, TEXT("Unable to bind optional OpenXR entry %s."), ANSI_TO_TCHAR(Name));
		}
	}

	static void XRAppendToChain(XrBaseOutStructure* ToAppend, XrBaseOutStructure* Chain)
	{
		while (Chain->next != XR_NULL_HANDLE)
		{
			if (Chain->next == ToAppend)
			{
				return;
			}
			Chain = Chain->next;
		}
		Chain->next = ToAppend;
	}

	XrResult CheckXrResult(XrResult res, const char* cmd);

	static inline double FromXrDuration(const XrDuration duration)
	{
		return (duration * 1e-9);
	}

	static inline XrDuration ToXrDuration(const double duration)
	{
		return (duration * 1e9);
	}

	static inline double FromXrTime(const XrTime time)
	{
		return (time * 1e-9);
	}

	static inline XrTime ToXrTime(const double time)
	{
		return (time * 1e9);
	}

	static bool IsOpenXRSystem()
	{
		const FName SystemName(TEXT("OpenXR"));
		return GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName);
	}

	IXRTrackingSystem* GetOpenXRTrackingSystem();

} // namespace OculusXR
