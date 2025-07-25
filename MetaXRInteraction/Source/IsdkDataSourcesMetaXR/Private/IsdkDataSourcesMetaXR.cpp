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

#include "IsdkDataSourcesMetaXR.h"

#include "IsdkDataSourcesMetaXRLog.h"

DEFINE_LOG_CATEGORY(LogIsdkDataSourcesMetaXR);

#define LOCTEXT_NAMESPACE "FIsdkDataSourcesMetaXRModule"

void FIsdkDataSourcesMetaXRModule::StartupModule()
{
  UE_LOG(LogIsdkDataSourcesMetaXR, Log, TEXT("Starting FIsdkDataSourcesMetaXRModule"));
  Instance = MakeUnique<FIsdkOculusXRModuleInstance>();
}

void FIsdkDataSourcesMetaXRModule::ShutdownModule()
{
  UE_LOG(LogIsdkDataSourcesMetaXR, Log, TEXT("Stopping FIsdkDataSourcesMetaXRModule"));
  Instance.Reset();
}

FIsdkOculusXRModuleInstance& FIsdkDataSourcesMetaXRModule::GetChecked()
{
  FIsdkDataSourcesMetaXRModule& Module =
      FModuleManager::LoadModuleChecked<FIsdkDataSourcesMetaXRModule>("IsdkDataSourcesMetaXR");

  if (!Module.Instance.IsValid())
  {
    UE_LOG(LogIsdkDataSourcesMetaXR, Log, TEXT("But FIsdkDataSourcesMetaXRModule DID exist"));
  }
  checkf(
      Module.Instance.IsValid(),
      TEXT("Attempted to get FIsdkDataSourcesMetaXRModule before it has started"));

  return *Module.Instance;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FIsdkDataSourcesMetaXRModule, IsdkDataSourcesMetaXR)
