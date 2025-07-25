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

#include "DataSources/IsdkFromMetaXRHmdDataSource.h"
#include "GameFramework/Actor.h"

#include "IsdkDataSourcesMetaXR.h"

bool UIsdkFromMetaXRHmdDataSource::IsHmdTracked_Implementation()
{
  const auto& MetaXRModule = FIsdkDataSourcesMetaXRModule::GetChecked();
  return MetaXRModule.IsDeviceTracked_Hmd();
}

void UIsdkFromMetaXRHmdDataSource::GetHmdPose_Implementation(FTransform& HmdPose, bool& IsTracked)
{
  GetRelativeHmdPose_Implementation(HmdPose, IsTracked);
  HmdPose = HmdPose * GetOwner()->GetTransform();
}

void UIsdkFromMetaXRHmdDataSource::GetRelativeHmdPose_Implementation(
    FTransform& HmdRelativePose,
    bool& IsTracked)
{
  const auto& MetaXRModule = FIsdkDataSourcesMetaXRModule::GetChecked();
  IsTracked = MetaXRModule.IsDeviceTracked_Hmd();
  if (IsTracked)
  {
    FVector DevicePosition;
    FRotator DeviceOrientation;
    FVector NeckPosition;

    MetaXRModule.GetPose(DeviceOrientation, DevicePosition, NeckPosition);
    HmdRelativePose = FTransform(DeviceOrientation, DevicePosition);
  }
}
