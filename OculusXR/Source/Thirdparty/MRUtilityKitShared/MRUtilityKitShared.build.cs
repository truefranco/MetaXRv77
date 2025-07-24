// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class MRUtilityKitShared : ModuleRules
{
    public MRUtilityKitShared(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            // MRUtilityKit_APL.xml is reponsible for copying over the .so library
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "MRUtilityKit_APL.xml"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "Lib/Win64/mrutilitykitshared.dll"));
        }
    }
}
