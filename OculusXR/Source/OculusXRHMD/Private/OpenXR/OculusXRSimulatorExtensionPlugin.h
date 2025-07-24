// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "OpenXR/IOculusXRExtensionPlugin.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOculusXRSimulatorPlugin, Log, All);

namespace OculusXR
{

	class FXRSimulatorExtensionPlugin : public IOculusXRExtensionPlugin
	{
	public:
		// IOpenXRExtensionPlugin
		virtual bool GetCustomLoader(PFN_xrGetInstanceProcAddr* OutGetProcAddr) override;
	};

} // namespace OculusXR
