// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "MRUtilityKitOpenXrExtensionPlugin.h"

#include "IOpenXRHMDModule.h"
#include "OpenXRCore.h"

void FMRUKOpenXrExtensionPlugin::RegisterAsOpenXRExtension()
{
#if defined(WITH_OCULUS_BRANCH)
	// Feature not enabled on Marketplace build. Currently only for the meta fork
	RegisterOpenXRExtensionModularFeature();
#endif
}

bool FMRUKOpenXrExtensionPlugin::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
{
	return true;
}

bool FMRUKOpenXrExtensionPlugin::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
{
	return true;
}

void FMRUKOpenXrExtensionPlugin::OnEvent(XrSession InSession, const XrEventDataBaseHeader* InHeader)
{
	if (OpenXrEventHandler)
	{
		OpenXrEventHandler((void*)InHeader, Context);
	}
}
