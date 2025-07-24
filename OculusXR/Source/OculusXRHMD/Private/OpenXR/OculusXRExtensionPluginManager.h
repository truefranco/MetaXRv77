// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "CoreMinimal.h"
#include "OculusXREnvironmentDepthExtensionPlugin.h"
#include "OculusXRCoreExtensionPlugin.h"
#include "OculusXRGuardianExtensionPlugin.h"
#include "OculusXRLayerExtensionPlugin.h"
#include "OculusXRPerformanceExtensionPlugin.h"
#include "OculusXRSimulatorExtensionPlugin.h"
#include "OculusXRSpaceWarp.h"
#include "OculusXRSystemInfoExtensionPlugin.h"
#include "OculusXRMultiPlayerStateExtensionPlugin.h"

namespace OculusXR
{

	class FExtensionPluginManager
	{
	public:
		FExtensionPluginManager();
		virtual ~FExtensionPluginManager();

		void StartupOpenXRPlugins();

		FPerformanceExtensionPlugin& GetPerformanceExtensionPlugin();
		FSystemInfoExtensionPlugin& GetSystemInfoExtensionPlugin();
		FGuardianExtensionPlugin& GetGuardianExtensionPlugin();
		FLayerExtensionPlugin& GetLayerExtensionPlugin();
#ifdef WITH_OCULUS_BRANCH
		FEnvironmentDepthExtensionPlugin& GetEnvironmentDepthExtensionPlugin();
#endif
		FMultiPlayerStateExtensionPlugin& GetMultiPlayerStateExtensionPlugin();

	private:
		FCoreExtensionPlugin CoreExtensionPlugin;
		FPerformanceExtensionPlugin PerformanceExtensionPlugin;
		FXRSimulatorExtensionPlugin XRSimulatorExtensionPlugin;
		FGuardianExtensionPlugin GuardianExtensionPlugin;
		FLayerExtensionPlugin LayerExtensionPlugin;
#ifdef WITH_OCULUS_BRANCH
		FEnvironmentDepthExtensionPlugin EnvironmentDepthExtensionPlugin;
#endif
#if defined(WITH_OCULUS_BRANCH) || defined(WITH_OPENXR_BRANCH)
		FSpaceWarpExtensionPlugin SpaceWarpExtensionPlugin;
#endif // defined(WITH_OCULUS_BRANCH)
		FMultiPlayerStateExtensionPlugin MultiPlayerStateExtensionPlugin;

		FSystemInfoExtensionPlugin SystemInfoExtensionPlugin;
	};

} // namespace OculusXR
