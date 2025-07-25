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
#include "StructTypes.h"

#include "IsdkHandJointMappings.generated.h"

UCLASS(
    BlueprintType,
    DefaultToInstanced,
    Category = "InteractionSDK",
    meta = (DisplayName = "ISDK Hand Joint Mappings"))

/* Container for Finger and Thumb Joint Mappings */
class OCULUSINTERACTION_API UIsdkHandJointMappings : public UObject
{
  GENERATED_BODY()
 public:
  /* Array mapping indices and thumb joint data */
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractionSDK)
  TArray<FIsdkExternalHandPositionFrame_ThumbJointMapping> ThumbJointMappings;

  /* Array mapping indices and finger joint data */
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractionSDK)
  TArray<FIsdkExternalHandPositionFrame_FingerJointMapping> FingerJointMappings;
};
