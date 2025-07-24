// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class OculusXRAsyncRequest : ModuleRules
    {
        public OculusXRAsyncRequest(ReadOnlyTargetRules Target) : base(Target)
        {
            bUseUnity = true;

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                });

            PrivateIncludePaths.AddRange(
                new string[] {
                });

            PublicIncludePaths.AddRange(
                new string[] {
                    "Runtime/Engine/Classes/Components",
                });
        }
    }
}
