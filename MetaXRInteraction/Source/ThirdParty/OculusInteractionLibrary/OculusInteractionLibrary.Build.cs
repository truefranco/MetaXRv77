/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 *
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

using System.IO;
using UnrealBuildTool;

public class OculusInteractionLibrary : ModuleRules
{
	public OculusInteractionLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		
		string ModuleSourceDirectory = "$(PluginDir)/Source/ThirdParty/OculusInteractionLibrary";
		if (Target.Platform == UnrealTargetPlatform.Android)
	    {
	        var SoPath = Path.Combine(ModuleSourceDirectory, "Lib/arm64-v8a/libInteractionSdk.so");

	        // Add the import library (used at build time)
	        PublicAdditionalLibraries.Add(SoPath);
	    }
		// Add DLL as a RuntimeDependency so that it is copied to the output directory during packaging.
		// Add to DelayLoadDLLS so we can load it manually in OculusInteractionModule::StartupModule; otherwise
		// the engine will look for it in the wrong location.
		else if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			var PlatformPath = ModuleSourceDirectory + "/Lib/" + Target.Platform + "/";

			RuntimeDependencies.Add(PlatformPath + "InteractionSDK.dll");
			PublicAdditionalLibraries.Add(PlatformPath + "InteractionSDK.dll.imp.lib");
			PublicDelayLoadDLLs.Add("InteractionSDK.dll");

		}
	}
}
