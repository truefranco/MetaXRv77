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

#include "IsdkDataSourcesOpenXR.h"
#include "IsdkDataSourcesOpenXRLog.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY(LogIsdkDataSourcesOpenXR);

#define LOCTEXT_NAMESPACE "FIsdkDataSourcesOpenXRModule"

FIsdkDataSourcesOpenXRModule* FIsdkDataSourcesOpenXRModule::Singleton = nullptr;

void FIsdkDataSourcesOpenXRModule::StartupModule()
{
  UE_LOG(LogIsdkDataSourcesOpenXR, Log, TEXT("Starting FIsdkDataSourcesOpenXRModule"));
  Singleton = this;
}

void FIsdkDataSourcesOpenXRModule::ShutdownModule()
{
  UE_LOG(LogIsdkDataSourcesOpenXR, Log, TEXT("Stopping FIsdkDataSourcesOpenXRModule"));
  Singleton = nullptr;
}

FIsdkDataSourcesOpenXRModule& FIsdkDataSourcesOpenXRModule::Get()
{
  check(Singleton);
  return *Singleton;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FIsdkDataSourcesOpenXRModule, IsdkDataSourcesOpenXR)
