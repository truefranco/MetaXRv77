// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "khronos/openxr/openxr.h"

#include "CoreMinimal.h"
#include "IOculusXRInputModule.h"
#include "IOpenXRExtensionPlugin.h"
#include "Misc/EngineVersionComparison.h"

namespace OculusXRInput
{

	class FTouchPlusInputExtensionPlugin : public IOpenXRExtensionPlugin
	{
	public:
		void RegisterOpenXRExtensionPlugin()
		{
#if !UE_VERSION_OLDER_THAN(5, 5, 0)
			RegisterOpenXRExtensionModularFeature();
#endif
		}

		// IOpenXRExtensionPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual void PostCreateInstance(XrInstance InInstance) override;
		virtual bool GetInteractionProfile(XrInstance InInstance, FString& OutKeyPrefix, XrPath& OutPath, bool& OutHasHaptics) override;

	private:
		XrPath InteractionProfile;
	};

} // namespace OculusXRInput
