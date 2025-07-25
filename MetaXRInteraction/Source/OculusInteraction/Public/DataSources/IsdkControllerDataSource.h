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
#include "IsdkIRootPose.h"
#include "Components/ActorComponent.h"
#include "IsdkControllerDataSource.generated.h"

/* Data source Actor Component for Controllers that implements the IsdkIRootPose interface */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OCULUSINTERACTION_API UIsdkControllerDataSource : public UActorComponent,
                                                        public IIsdkIRootPose
{
  GENERATED_BODY()

 public:
  UIsdkControllerDataSource();

 protected:
  virtual void BeginPlay() override;

 public:
  virtual void TickComponent(
      float DeltaTime,
      ELevelTick TickType,
      FActorComponentTickFunction* ThisTickFunction) override;

  void UpdatePose(const FTransform& NewPose)
  {
    CurrentPose = NewPose;
  }

  virtual FTransform GetRootPose_Implementation() override
  {
    return CurrentPose;
  }

  virtual bool IsRootPoseValid_Implementation() override
  {
    return true;
  }

 private:
  FTransform CurrentPose;
};
