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
#include "IsdkIHandPointerPose.generated.h"

// This class does not need to be modified.
UINTERFACE(
    BlueprintType,
    Category = "InteractionSDK",
    meta = (DisplayName = "ISDK Hand Pointer Pose"))
class OCULUSINTERACTION_API UIsdkIHandPointerPose : public UInterface
{
  GENERATED_BODY()
};

/*
 * Interface containing transform and validity information about a raycast pointer pose
 */
class OCULUSINTERACTION_API IIsdkIHandPointerPose
{
  GENERATED_BODY()

 public:
  /* Returns whether or not the pointer pose on this object is valid */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = InteractionSDK)
  bool IsPointerPoseValid();
  virtual bool IsPointerPoseValid_Implementation()
      PURE_VIRTUAL(IIsdkIHandPointerPose::IsPointerPoseValid, return false;);

  /* Retrieve references to the transform of this object's raycast origin (in world space) and if
   * it's valid */
  UFUNCTION(
      BlueprintNativeEvent,
      BlueprintCallable,
      Category = InteractionSDK,
      meta = (ToolTip = "transform used for raycast origin, world space."))
  void GetPointerPose(FTransform& PointerPose, bool& IsValid);
  virtual void GetPointerPose_Implementation(FTransform& PointerPose, bool& IsValid)
      PURE_VIRTUAL(IIsdkIHandPointerPose::GetPointerPose_Implementation, return;);

  /* Retrieve references to the transform of this object's raycast origin (relative to tracking
   * space) and if it's valid */
  UFUNCTION(
      BlueprintNativeEvent,
      BlueprintCallable,
      Category = InteractionSDK,
      meta = (ToolTip = "transform used for raycast origin, relative to tracking space."))
  void GetRelativePointerPose(FTransform& PointerRelativePose, bool& IsValid);
  virtual void GetRelativePointerPose_Implementation(FTransform& PointerRelativePose, bool& IsValid)
      PURE_VIRTUAL(IIsdkIHandPointerPose::GetRelativePointerPose_Implementation, return;);
};
