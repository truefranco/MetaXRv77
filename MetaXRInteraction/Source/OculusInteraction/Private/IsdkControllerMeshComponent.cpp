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

#include "IsdkControllerMeshComponent.h"
#include "DataSources/IsdkIRootPose.h"
#include "DataSources/IsdkIHandPointerPose.h"

// Sets default values for this component's properties
UIsdkControllerMeshComponent::UIsdkControllerMeshComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
  PointerPoseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PointerPoseComponent"));
  PointerPoseComponent->SetupAttachment(this);
}

// Called every frame
void UIsdkControllerMeshComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
  UpdateController();
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  UpdateChildTransforms();
}

TScriptInterface<IIsdkIHandPointerPose> UIsdkControllerMeshComponent::GetHandPointerPoseDataSource()
    const
{
  return HandPointerPoseDataSource;
}

TScriptInterface<IIsdkIRootPose> UIsdkControllerMeshComponent::GetRootPoseDataSource() const
{
  return RootPoseDataSource;
}

USceneComponent* UIsdkControllerMeshComponent::GetPointerPoseComponent() const
{
  return PointerPoseComponent;
}

void UIsdkControllerMeshComponent::SetHandPointerPoseDataSource(
    const TScriptInterface<IIsdkIHandPointerPose>& InHandPointerPoseDataSource)
{
  // Remove tick prerequisite for old data source
  UActorComponent* ControllerActorComponent =
      Cast<UActorComponent>(HandPointerPoseDataSource.GetObject());
  if (IsValid(ControllerActorComponent))
  {
    RemoveTickPrerequisiteComponent(ControllerActorComponent);
  }

  // Assign the new data source
  HandPointerPoseDataSource = InHandPointerPoseDataSource;

  // Add tick prerequisite for new data source
  ControllerActorComponent = Cast<UActorComponent>(HandPointerPoseDataSource.GetObject());
  if (IsValid(ControllerActorComponent))
  {
    AddTickPrerequisiteComponent(ControllerActorComponent);
  }
}

void UIsdkControllerMeshComponent::SetRootPoseDataSource(
    const TScriptInterface<IIsdkIRootPose>& InRootPoseDataSource)
{
  // Remove tick prerequisite for old data source
  UActorComponent* ControllerActorComponent = Cast<UActorComponent>(RootPoseDataSource.GetObject());
  if (IsValid(ControllerActorComponent))
  {
    RemoveTickPrerequisiteComponent(ControllerActorComponent);
  }

  // Assign the new data source
  RootPoseDataSource = InRootPoseDataSource;

  // Add tick prerequisite for new data source
  ControllerActorComponent = Cast<UActorComponent>(RootPoseDataSource.GetObject());
  if (IsValid(ControllerActorComponent))
  {
    AddTickPrerequisiteComponent(ControllerActorComponent);
  }
}

void UIsdkControllerMeshComponent::UpdateController()
{
  if (IsValid(RootPoseDataSource.GetObject()))
  {
    const bool bIsControllerPoseValid =
        IIsdkIRootPose::Execute_IsRootPoseValid(RootPoseDataSource.GetObject());
    if (bIsControllerPoseValid)
    {
      const auto ControllerPose =
          IIsdkIRootPose::Execute_GetRootPose(RootPoseDataSource.GetObject());
      SetWorldTransform(ControllerPose, false, nullptr, ETeleportType::TeleportPhysics);
    }
  }

  if (IsValid(HandPointerPoseDataSource.GetObject()))
  {
    bool bIsPointerPoseValid = false;
    FTransform RelativePointerPose;
    IIsdkIHandPointerPose::Execute_GetRelativePointerPose(
        HandPointerPoseDataSource.GetObject(), RelativePointerPose, bIsPointerPoseValid);
    if (bIsPointerPoseValid)
    {
      PointerPoseComponent->SetRelativeTransform(RelativePointerPose);
    }
  }
}
