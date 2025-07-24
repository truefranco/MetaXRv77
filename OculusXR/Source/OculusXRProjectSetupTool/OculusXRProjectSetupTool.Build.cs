// Copyright (c) Meta Platforms, Inc. and affiliates.

using UnrealBuildTool;
using System.IO;

public class OculusXRProjectSetupTool : ModuleRules
{
    public OculusXRProjectSetupTool(ReadOnlyTargetRules Target) : base(Target)
    {

        bUseUnity = true;

        PrivateIncludePaths.AddRange(
            new string[] {
                "OculusXRHMD/Private",
                "OculusXRUncookedOnly/Private",
                Path.Combine(EngineDirectory, "Source/Developer/Android/AndroidPlatformEditor/Private")
            });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Projects",
                "UnrealEd",
                "LevelEditor",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "EngineSettings",
                "OculusXRHMD",
                "OculusXRMovement",
                "OculusXRPassthrough",
                "OculusXRAnchors",
                "OculusXRScene",
                "OculusXRUncookedOnly",
                "AndroidRuntimeSettings",
                "AndroidPlatformEditor",
                "LauncherServices",
                "ToolWidgets",
                "WorkspaceMenuStructure",
                "PluginBrowser",
                "ToolMenus",
                "RHI",
                "BlueprintGraph",
            }
        );
    }
}
