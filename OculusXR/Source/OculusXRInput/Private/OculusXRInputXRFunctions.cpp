// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRInputXRFunctions.h"
#include "OculusXRInputModule.h"
#include "OpenXR/OculusXROpenXRUtilities.h"

namespace OculusXRInput
{
	PFN_xrGetDeviceSampleRateFB xrGetDeviceSampleRateFB = nullptr;

	void InitOpenXRFunctions(XrInstance InInstance)
	{
		const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
		if (!InputModule)
		{
			UE_LOG(LogOcInput, Error, TEXT("Failed getting Oculus XR input module."));
			return;
		}
		if (InputModule->GetHapticsOpenXRExtension()->IsPCMExtensionAvailable())
		{
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetDeviceSampleRateFB", &xrGetDeviceSampleRateFB);
		}
	}

} // namespace OculusXRInput
