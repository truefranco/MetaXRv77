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

#include "DataSources/IsdkFromOpenXRHmdDataSource.h"
#include "Engine/Engine.h"
#include "IXRTrackingSystem.h"

// Sets default values for this component's properties
UIsdkFromOpenXRHmdDataSource::UIsdkFromOpenXRHmdDataSource()
{
  PrimaryComponentTick.bCanEverTick = false;
}

bool UIsdkFromOpenXRHmdDataSource::IsHmdTracked_Implementation()
{
  auto XRTrackingSystem = GEngine->XRSystem.Get();
  return XRTrackingSystem && XRTrackingSystem->IsTracking(XRTrackingSystem->HMDDeviceId);
}

void UIsdkFromOpenXRHmdDataSource::GetHmdPose_Implementation(FTransform& HmdPose, bool& IsTracked)
{
  GetRelativeHmdPose_Implementation(HmdPose, IsTracked);
  if (IsTracked)
  {
    HmdPose = HmdPose * GetOwner()->GetTransform();
  }
}

void UIsdkFromOpenXRHmdDataSource::GetRelativeHmdPose_Implementation(
    FTransform& HmdRelativePose,
    bool& IsTracked)
{
  if (IXRTrackingSystem* XRTrackingSystem = GEngine->XRSystem.Get())
  {
    FQuat Orientation;
    FVector Position;
    IsTracked =
        XRTrackingSystem->GetCurrentPose(XRTrackingSystem->HMDDeviceId, Orientation, Position);
    if (IsTracked)
    {
      HmdRelativePose = FTransform(Orientation, Position);
    }
  }
}
