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

#include "OculusInteraction.h"

#include "OculusInteractionLog.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "StructTypesPrivate.h"
#include "isdk_api/isdk_api.hpp"
#include "HAL/IConsoleManager.h"

#include <array>

#define LOCTEXT_NAMESPACE "FOculusInteractionModule"

DEFINE_LOG_CATEGORY(LogOculusInteraction);

namespace isdk
{
TAutoConsoleVariable<bool> CVar_Meta_InteractionSDK_DebugInteractionVisuals(
    TEXT("Meta.InteractionSDK.DebugInteractionVisuals"),
    false,
    TEXT("Draws debug visuals for interactions at runtime.  Also draws to visual logger."));
TAutoConsoleVariable<bool> CVar_Meta_InteractionSDK_DisableDistanceDebugging(
    TEXT("Meta.InteractionSDK.DisableDistanceGrabDebugging"),
    false,
    TEXT("Disables showing distance visuals when debugging interactions"));
} // namespace isdk

void FOculusInteractionModule::StartupModule()
{
  // Get the base directory of this plugin
  FString BaseDir = IPluginManager::Get().FindPlugin("OculusInteraction")->GetBaseDir();

  // Add on the relative location of the third party dll and load it
  FString LibraryPath;
#if PLATFORM_WINDOWS
  LibraryPath = FPaths::Combine(
      *BaseDir, TEXT("Source/ThirdParty/OculusInteractionLibrary/Lib/Win64/InteractionSdk.dll"));
#elif PLATFORM_ANDROID
  LibraryPath = TEXT("libInteractionSdk.so");
#endif

  InteractionSDKLibraryHandle =
      !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

  if (InteractionSDKLibraryHandle == nullptr)
  {
    UE_LOG(LogOculusInteraction, Error, TEXT("Failed to load Interaction SDK library."));
  }
  else
  {
    StructTypesUtils::CreateStructTypesStatics();

    std::array<char, 32> VersionStringData{0};
    isdk::api::ApiContext::getVersion(VersionStringData.data(), VersionStringData.size());

    VersionString = FString(VersionStringData.data());
  }
}

void FOculusInteractionModule::ShutdownModule()
{
  // Free the dll handle
  FPlatformProcess::FreeDllHandle(InteractionSDKLibraryHandle);
  InteractionSDKLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOculusInteractionModule, OculusInteraction)
