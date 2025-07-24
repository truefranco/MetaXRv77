// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "MRUtilityKit.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "MRUtilityKitOpenXrExtensionPlugin.h"
#include "OculusXRHMDRuntimeSettings.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif // WITH_EDITOR

#define LOCTEXT_NAMESPACE "FMRUKModule"

DEFINE_LOG_CATEGORY(LogMRUK);

const FString FMRUKLabels::Floor("FLOOR");
const FString FMRUKLabels::WallFace("WALL_FACE");
const FString FMRUKLabels::InvisibleWallFace("INVISIBLE_WALL_FACE");
const FString FMRUKLabels::Ceiling("CEILING");
const FString FMRUKLabels::DoorFrame("DOOR_FRAME");
const FString FMRUKLabels::WindowFrame("WINDOW_FRAME");
const FString FMRUKLabels::Couch("COUCH");
const FString FMRUKLabels::Table("TABLE");
const FString FMRUKLabels::Screen("SCREEN");
const FString FMRUKLabels::Bed("BED");
const FString FMRUKLabels::Lamp("LAMP");
const FString FMRUKLabels::Plant("PLANT");
const FString FMRUKLabels::Storage("STORAGE");
const FString FMRUKLabels::WallArt("WALL_ART");
const FString FMRUKLabels::GlobalMesh("GLOBAL_MESH");
const FString FMRUKLabels::Other("OTHER");

bool FMRUKLabelFilter::PassesFilter(const TArray<FString>& Labels) const
{
	for (const auto& ExcludedLabel : ExcludedLabels)
	{
		if (Labels.Contains(ExcludedLabel))
		{
			return false;
		}
	}
	for (const auto& IncludedLabel : IncludedLabels)
	{
		if (Labels.Contains(IncludedLabel))
		{
			return true;
		}
	}
	return IncludedLabels.IsEmpty();
}

FMRUKModule& FMRUKModule::GetInstance()
{
	return FModuleManager::LoadModuleChecked<FMRUKModule>("MRUtilityKit");
}

void FMRUKModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified
	// in the .uplugin file per-module
	OpenXrExtension = MakeUnique<FMRUKOpenXrExtensionPlugin>();
	OpenXrExtension->RegisterAsOpenXRExtension();
}

void FMRUKModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support
	// dynamic reloading, we call this function before unloading the module.
	OpenXrExtension->UnregisterOpenXRExtensionModularFeature();
	OpenXrExtension.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMRUKModule, MRUtilityKit)
