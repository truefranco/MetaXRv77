// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSoftwareOcclusionModule.h"
#include "SceneSoftwareOcclusionProvider.h"
#include "Features/IModularFeatures.h"

DEFINE_LOG_CATEGORY(LogSoftwareOcclusion);
#define LOCTEXT_NAMESPACE "OculusXRSoftwareOcclusionModule"

IMPLEMENT_MODULE(FOculusXRSoftwareOcclusionModule, OculusXRSoftwareOcclusion)

/**
 * Perform module initialization
 */
void FOculusXRSoftwareOcclusionModule::StartupModule()
{
	IOculusXRSoftwareOcclusionModule::StartupModule();
	UE_LOG(LogSoftwareOcclusion, Display, TEXT("StartupModule: OculusXRSoftwareOcclusion"));

#ifdef WITH_OCULUS_BRANCH
	auto SceneSoftwareOcclusionProvider = FSceneSoftwareOcclusionProvider::Get();
	if (SceneSoftwareOcclusionProvider.IsValid())
	{
		IModularFeatures::Get().RegisterModularFeature(ICustomOcclusionProvider::GetModularFeatureName(), SceneSoftwareOcclusionProvider.Get());
	}
#endif // WITH_OCULUS_BRANCH
}

/**
 * Perform module cleanup
 */
void FOculusXRSoftwareOcclusionModule::ShutdownModule()
{
#ifdef WITH_OCULUS_BRANCH
	auto SceneSoftwareOcclusionProvider = FSceneSoftwareOcclusionProvider::Get();
	if (SceneSoftwareOcclusionProvider.IsValid())
	{
		IModularFeatures::Get().UnregisterModularFeature(ICustomOcclusionProvider::GetModularFeatureName(), SceneSoftwareOcclusionProvider.Get());
	}
#endif // WITH_OCULUS_BRANCH
}
