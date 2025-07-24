// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class OculusXRMovement : ModuleRules
    {
        public OculusXRMovement(ReadOnlyTargetRules Target) : base(Target)
        {
            bUseUnity = false;

            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "LiveLinkInterface",
                    "LiveLinkAnimationCore",
                });

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "ApplicationCore",
                    "Engine",
                    "InputCore",
                    "LiveLink",
                    "HeadMountedDisplay",
                    "OVRPluginXR",
                    "OculusXRHMD",
                    "XRBase",
                    "OpenXR",
                    "OpenXRHMD",
                });

            PrivateIncludePaths.AddRange(
                new string[] {
                    "OculusXRHMD/Private",
                });

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "KhronosOpenXRHeaders",
                });

            PrivateIncludePathModuleNames.Add("OpenXRHMD");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenXR");
        }
    }
}
