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

#pragma once

#include "CoreMinimal.h"
#include "IsdkIHandPointerPose.h"
#include "Components/ActorComponent.h"
#include "IsdkControllerPointerPose.generated.h"

/* ActorComponent that stores the current Pointer Pose, implements the IsdkIHandPointerPose
 * interface */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OCULUSINTERACTION_API UIsdkControllerPointerPose : public UActorComponent,
                                                         public IIsdkIHandPointerPose
{
  GENERATED_BODY()

 public:
  // Sets default values for this component's properties
  UIsdkControllerPointerPose();

  void UpdatePose(const FTransform& NewPose)
  {
    CurrentPose = NewPose;
  }

  void UpdateRelativePose(const FTransform& NewRelativePose)
  {
    CurrentRelativePose = NewRelativePose;
  }

  virtual bool IsPointerPoseValid_Implementation() override
  {
    return true;
  }

  virtual void GetPointerPose_Implementation(FTransform& PointerPose, bool& IsValid) override
  {
    PointerPose = CurrentPose;
    IsValid = true; // Assume the pose is always valid
  }

  virtual void GetRelativePointerPose_Implementation(FTransform& PointerRelativePose, bool& IsValid)
      override
  {
    PointerRelativePose = CurrentRelativePose;
    IsValid = true;
  }

 protected:
  // Called when the game starts
  virtual void BeginPlay() override;
  FTransform CurrentPose;
  FTransform CurrentRelativePose;

 public:
  // Called every frame
  virtual void TickComponent(
      float DeltaTime,
      ELevelTick TickType,
      FActorComponentTickFunction* ThisTickFunction) override;
};
