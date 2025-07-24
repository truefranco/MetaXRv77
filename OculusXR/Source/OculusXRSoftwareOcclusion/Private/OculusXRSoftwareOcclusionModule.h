// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "CoreMinimal.h"
#include "IOculusXRSoftwareOcclusionModule.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSoftwareOcclusion, Log, All);

/**
 * The module for the implementation of Software Occlusion
 */
class FOculusXRSoftwareOcclusionModule : public IOculusXRSoftwareOcclusionModule
{
public:
	/**
	 * IModuleInterface implementation
	 */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
