// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class OculusXRPassthrough : ModuleRules
    {
        public OculusXRPassthrough(ReadOnlyTargetRules Target) : base(Target)
        {
            bUseUnity = true;

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "ProceduralMeshComponent",
                    "OculusXRHMD",
                    "KhronosOpenXRHeaders",
                    "OVRPluginXR",
                    "RHI",
                    "HeadMountedDisplay",
                    "XRBase",
                    "OpenXR",
                    "OpenXRHMD",
                    "RenderCore",
                });

            PublicIncludePaths.AddRange(new string[] {
                "Runtime/Engine/Classes/Components",
                "Runtime/Engine/Classes/Kismet",
            });

            PrivateIncludePaths.AddRange(new string[] {
                // Relative to Engine\Plugins\Runtime\Oculus\OculusVR\Source
                "OculusXRHMD/Private",
            });

            PrivateIncludePathModuleNames.Add("OpenXRHMD");

            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenXR");

            //Needed for OpenXRHMD_Swapchain include
            {
                if (Target.Platform == UnrealTargetPlatform.Win64)
                {
                    PublicDependencyModuleNames.AddRange(new string[]
                    {
                        "D3D11RHI",
                        "D3D12RHI"
                    });

                    if (!bUsePrecompiled || Target.LinkType == TargetLinkType.Monolithic)
                    {
                        PublicDependencyModuleNames.AddRange(new string[]
                        {
                            "DX11",
                            "DX12"
                        });
                    }
                }

                if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Android)
                {
                    PublicDependencyModuleNames.Add("OpenGLDrv");

                    if (!bUsePrecompiled || Target.LinkType == TargetLinkType.Monolithic)
                    {
                        PublicDependencyModuleNames.Add("OpenGL");
                    }
                }

                if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Android
                                                                  || Target.IsInPlatformGroup(UnrealPlatformGroup.Linux))
                {
                    PublicDependencyModuleNames.Add("VulkanRHI");

                    if (!bUsePrecompiled || Target.LinkType == TargetLinkType.Monolithic)
                    {
                        PublicDependencyModuleNames.Add("Vulkan");
                    }
                }
            }
        }
    }
}
