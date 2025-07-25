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
#include "UObject/Interface.h"
#include "IsdkIHmdDataSource.generated.h"

// This class does not need to be modified.
UINTERFACE(
    BlueprintType,
    Category = "InteractionSDK",
    meta = (DisplayName = "ISDK Hmd Data Source"))
class OCULUSINTERACTION_API UIsdkIHmdDataSource : public UInterface
{
  GENERATED_BODY()
};

/**
 * Interface containing pose transform data about an HMD data source
 */
class OCULUSINTERACTION_API IIsdkIHmdDataSource
{
  GENERATED_BODY()

 public:
  /* Returns whether or not the HMD in this data source is tracked */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = InteractionSDK)
  bool IsHmdTracked();
  virtual bool IsHmdTracked_Implementation() = 0;

  /* Retrieve reference to the transform (world space) of the HMD and whether or its tracked */
  UFUNCTION(
      BlueprintNativeEvent,
      BlueprintCallable,
      Category = InteractionSDK,
      meta = (ToolTip = "transform of the headset, world space."))
  void GetHmdPose(FTransform& HmdPose, bool& IsTracked);
  virtual void GetHmdPose_Implementation(FTransform& HmdPose, bool& IsTracked) = 0;

  /* Retrieve reference to the transform (relative to tracking space) of the HMD and whether or not
   * its tracked */
  UFUNCTION(
      BlueprintNativeEvent,
      BlueprintCallable,
      Category = InteractionSDK,
      meta = (ToolTip = "transform of the headset, relative to tracking space."))
  void GetRelativeHmdPose(FTransform& HmdRelativePose, bool& IsTracked);
  virtual void GetRelativeHmdPose_Implementation(FTransform& HmdRelativePose, bool& IsTracked) = 0;
};
