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
#include "IsdkTransformer.h"
#include "IsdkTransformerUtils.h"
#include "IsdkITransformer.h"
#include "IsdkOneGrabTranslateTransformer.generated.h"

/**
 * @class UIsdkOneGrabTranslateTransformer
 * @brief Scene component utilized for transforming a grabbable object, modifying only its
 * translation and only when grabbed by one hand/controller
 * @see UIsdkTransformer
 * @addtogroup InteractionSDK
 */
UCLASS(
    Blueprintable,
    CollapseCategories,
    ClassGroup = (InteractionSDK),
    meta = (DisplayName = "ISDK One Grab Translate Transformer"))
class OCULUSINTERACTION_API UIsdkOneGrabTranslateTransformer : public UIsdkTransformer
{
  GENERATED_BODY()

 public:
  UIsdkOneGrabTranslateTransformer();
  /**
   * @brief Returns whether or not this transformer is currently active.
   */
  virtual bool IsTransformerActive() const override;

  /**
   * @brief Returns maximum grab points, -1 if infinite (UIsdkOneGrabTranslateTransformer always
   * returns 1).
   */
  virtual int MaxGrabPoints() const override;

  /**
   * @brief Initialize the transformation.
   *
   * Empties selected poses and deltas, adds
   * @param GrabPoses Array of GrabPose structs to initialize this transform with
   * @param Target Initial transform of the target object
   * @see IIsdkITransformer
   */
  virtual void BeginTransform(
      const TArray<FIsdkGrabPose>& GrabPoses,
      const FIsdkTargetTransform& Target) override;

  /**
   * @brief Generate the transform update,the result should be a FTransform relative to the parent.
   * @param GrabPoses Array of grab poses to update this transform with
   * @param Target Current transform of the target object
   * @return FTransform Resulting updated transform
   */
  virtual FTransform UpdateTransform(
      const TArray<FIsdkGrabPose>& GrabPoses,
      const FIsdkTargetTransform& Target) override;

  /**
   * @brief Flush any local state and generate a final update to the target transform, for example
   * finishing some interpolation.
   * @return FTransform The final resulting transform
   */
  virtual FTransform EndTransform(const FIsdkTargetTransform& Target) override;

  /**
   * @brief Generate constraints based on the state of the target and how this transformer is
   * configured
   * @param Target The current transform of the target object
   */
  virtual void UpdateConstraints(const FIsdkTargetTransform& Target) override;

  /* If true, use relative space when calculating constraints */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractionSDK|Constraint")
  bool bIsRelativeConstraint = true;

  /* Struct containing potential constraint axes for translation */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractionSDK|Constraint")
  FIsdkConstraintAxes PositionConstraint = FIsdkConstraintAxes();

 private:
  bool bIsActive = false;
  FVector InitialTargetOffset = FVector::ZeroVector;
  FIsdkConstraintAxes TargetPositionConstraint = FIsdkConstraintAxes();
  FIsdkGrabPose CachedSelectedPose = FIsdkGrabPose();
  FIsdkGrabPose GetSelectedPose(const TArray<FIsdkGrabPose>& GrabPoses) const;

  void DrawDebug(const FIsdkTargetTransform& Target) const;
};
