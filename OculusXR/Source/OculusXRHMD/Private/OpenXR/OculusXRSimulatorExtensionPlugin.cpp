// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSimulatorExtensionPlugin.h"

#include "OculusXRSimulator.h"

DEFINE_LOG_CATEGORY(LogOculusXRSimulatorPlugin);

namespace OculusXR
{
	bool FXRSimulatorExtensionPlugin::GetCustomLoader(PFN_xrGetInstanceProcAddr* OutGetProcAddr)
	{
#if PLATFORM_WINDOWS
		FMetaXRSimulator::TryActivateOnStartup();
#endif // PLATFORM_WINDOWS
		return false;
	}

} // namespace OculusXR
