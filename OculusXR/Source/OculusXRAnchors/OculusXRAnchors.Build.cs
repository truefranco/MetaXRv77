// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class OculusXRAnchors : ModuleRules
    {
        public OculusXRAnchors(ReadOnlyTargetRules Target) : base(Target)
        {
            bUseUnity = false;

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "HeadMountedDisplay",
                    "OculusXRHMD",
                    "OVRPluginXR",
                    "XRBase",
                    "OpenXR",
                    "OpenXRHMD",
                    "ProceduralMeshComponent",
                });

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "OculusXRAsyncRequest",
                    "KhronosOpenXRHeaders",
                });

            PrivateIncludePaths.AddRange(
                new string[] {
                    // Relative to Engine\Plugins\Runtime\Oculus\OculusVR\Source
                    "OculusXRHMD/Private",
                });

            PublicIncludePaths.AddRange(
                new string[] {
                    "Runtime/Engine/Classes/Components",
                });

            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenXR");
        }
    }
}
