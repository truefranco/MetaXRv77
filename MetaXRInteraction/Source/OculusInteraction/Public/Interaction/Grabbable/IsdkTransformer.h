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
#include "IsdkITransformer.h"
#include "UObject/Object.h"
#include "IsdkTransformer.generated.h"

/**
 * @class UIsdkTransformer
 * @brief Abstract class that implements baseline functionality from IIsdkITransformer for child
 * transformer classes to further implement
 * @see IIsdkITransformer
 * @addtogroup InteractionSDK
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class OCULUSINTERACTION_API UIsdkTransformer : public UObject, public IIsdkITransformer
{
  GENERATED_BODY()

 public:
  UIsdkTransformer() {}

  /**
   * @brief Initialize the transformation. Implemented from interface
   * @see IIsdkITransformer
   */
  virtual void BeginTransform(
      const TArray<FIsdkGrabPose>& SelectPoses,
      const FIsdkTargetTransform& Target) override
      PURE_VIRTUAL(UIsdkTransformer::BeginTransform, ;);

  /**
   * @brief Flush any local state and generate a final update to the target transform, for example
   * finishing some interpolation. Implemented from interface.
   * @see IIsdkITransformer
   */
  virtual FTransform EndTransform(const FIsdkTargetTransform& Target) override
      PURE_VIRTUAL(UIsdkTransformer::EndTransform, return FTransform::Identity;);

  /**
   * @brief Generate the transform update,the result should be a FTransform relative to the parent.
   * Implemented from interface
   * @see IIsdkITransformer
   */
  virtual FTransform UpdateTransform(
      const TArray<FIsdkGrabPose>& SelectPoses,
      const FIsdkTargetTransform& Target) override
      PURE_VIRTUAL(UIsdkTransformer::UpdateTransform, return FTransform::Identity;);

  /**
   * @brief Meant to be used to generate constraints based on the state of the target at some point
   * in time. Implemented from interface.
   * @see IIsdkITransformer
   */
  virtual void UpdateConstraints(const FIsdkTargetTransform& Target) override
      PURE_VIRTUAL(UIsdkTransformer::UpdateConstraints, ;);

  /**
   * @brief Returns whether or not this transformer is currently active. Implemented from interface.
   * @see IIsdkITransformer
   */
  virtual bool IsTransformerActive() const override
      PURE_VIRTUAL(UIsdkTransformer::IsActive, return false;);

  /**
   * @brief Returns maximum grab points, -1 if infinite. Implemented from interface.
   * @see IIsdkITransformer
   */
  virtual int MaxGrabPoints() const override
      PURE_VIRTUAL(UIsdkTransformer::MaxGrabPoints, return 0;);
};
