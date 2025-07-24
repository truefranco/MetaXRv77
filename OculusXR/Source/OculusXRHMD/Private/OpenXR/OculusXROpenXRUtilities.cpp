// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXROpenXRUtilities.h"
#include "RHICommandList.h"
#include "RenderingThread.h"

namespace OculusXR
{
	XrResult CheckXrResult(XrResult Result, const char* Cmd)
	{
		if (!XR_SUCCEEDED(Result))
		{
			UE_LOG(LogHMD, Error, TEXT("%hs failed (%d)"), Cmd, Result);
		}
		return Result;
	}

	IXRTrackingSystem* GetOpenXRTrackingSystem()
	{
		static FName SystemName(TEXT("OpenXR"));
		if (GEngine && GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName))
		{
			return GEngine->XRSystem.Get();
		}
		return nullptr;
	}

} // namespace OculusXR
