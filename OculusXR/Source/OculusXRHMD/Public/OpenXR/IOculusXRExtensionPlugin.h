// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

// DO NOT REMOVE
// This include is needed in order to have the latest openxr header (from Thirdparty/KhronosOpenXR/)
// Without it, IOpenXRExtensionPlugin.h will include the openxr header from the Engine itself, which is outdated.
#include "khronos/openxr/openxr.h"

#include "IOpenXRExtensionPlugin.h"

class IOculusXRExtensionPlugin : public IOpenXRExtensionPlugin
{
public:
	void RegisterOpenXRExtensionPlugin()
	{
		RegisterOpenXRExtensionModularFeature();
	}
};
