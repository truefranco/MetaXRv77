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
#include "MathUtil.h"
#include "IsdkITransformer.h"
#include "IsdkOneGrabRotateTransformer.generated.h"

/**
 * @brief A struct defining how the rotation constraint should be configured when used in a
 * rotational transformer
 * @see UIsdkOneGrabRotateTransformer
 */
USTRUCT(Blueprintable)
struct FOneGrabRotationConstraint
{
  GENERATED_BODY()

 public:
  /** Whether or not to calculate the angle from the axis when calculating constraint */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  bool bUseAngleFromAxisConstraint;

  /** The axis to use in this constraint */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  TEnumAsByte<EAxis::Type> RotationAxis;

  /** The per-axis rotational constraints to apply */
  UPROPERTY(
      EditAnywhere,
      BlueprintReadWrite,
      Category = InteractionSDK,
      meta =
          (EditCondition = "(!bUseAngleFromAxisConstraint) && (RotationAxis == 0)",
           EditConditionHides))
  FIsdkConstraintAxes RotationConstraint;

  /** The ceiling on the angle allowed during constraint calculation */
  UPROPERTY(
      EditAnywhere,
      BlueprintReadWrite,
      Category = InteractionSDK,
      meta =
          (EditCondition = "bUseAngleFromAxisConstraint",
           EditConditionHides,
           ClampMin = "0.0",
           ClampMax = "180.0",
           UIMin = "0.0",
           UIMax = "180.0"))
  float MaxAngleFromAxis = 0.0;

  /** Minimum and maximum allowable angle for constraint calculation*/
  UPROPERTY(
      EditAnywhere,
      BlueprintReadWrite,
      Category = InteractionSDK,
      meta =
          (EditCondition = "(!bUseAngleFromAxisConstraint) && (RotationAxis != 0)",
           EditConditionHides))
  FIsdkAxisConstraints AxisAngleRange = FIsdkAxisConstraints();

  FOneGrabRotationConstraint()
  {
    bUseAngleFromAxisConstraint = false;
    RotationAxis = EAxis::Type::X;
    RotationConstraint = FIsdkConstraintAxes();
    MaxAngleFromAxis = 0.0;
    AxisAngleRange = FIsdkAxisConstraints();
  }
};

/**
 * @class UIsdkOneGrabRotateTransformer
 * @brief Scene component utilized for transforming a grabbable object, modifying only its
 * rotation and only when grabbed by one hand/controller
 * @see UIsdkTransformer
 * @addtogroup InteractionSDK
 */
UCLASS(
    Blueprintable,
    CollapseCategories,
    ClassGroup = (InteractionSDK),
    meta = (DisplayName = "ISDK One Grab Rotate Transformer"))
class OCULUSINTERACTION_API UIsdkOneGrabRotateTransformer : public UIsdkTransformer
{
  GENERATED_BODY()
 public:
  UIsdkOneGrabRotateTransformer();

  /**
   * @brief Returns whether or not this transformer is currently active.
   */
  virtual bool IsTransformerActive() const override
  {
    return bIsActive;
  }

  /**
   * @brief Returns maximum grab points, -1 if infinite (UIsdkOneGrabTranslateTransformer always
   * returns 1).
   */
  virtual int MaxGrabPoints() const override
  {
    return 1;
  }

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
   * configured. (Not used by rotational constraints)
   * @param Target The current transform of the target object
   */
  virtual void UpdateConstraints(const FIsdkTargetTransform& Target) override;

  /* The rotation constraint to use when updating the transform */
  UPROPERTY(
      EditAnywhere,
      BlueprintReadWrite,
      Category = InteractionSDK,
      meta = (ShowOnlyInnerProperties))
  FOneGrabRotationConstraint Constraint = FOneGrabRotationConstraint();

 private:
  bool bIsActive = false;
  FQuat CachedTargetRelativeRotation = FQuat::Identity;
  FVector CachedPoseVector = FVector::ZeroVector;
  FIsdkGrabPose CachedSelectedPose = FIsdkGrabPose();
  FIsdkGrabPose GetSelectedPose(const TArray<FIsdkGrabPose>& GrabPoses)
  {
    for (const auto& GrabPose : GrabPoses)
    {
      if (GrabPose.Identifier == CachedSelectedPose.Identifier)
      {
        return GrabPose;
      }
    }
    return CachedSelectedPose;
  }

  FVector TransformGrabVector(const FVector& Vector);
  void DrawDebug(
      const FTransform& PivotTransform,
      const FVector& Axis,
      const FVector& StartAxis,
      const FVector& CurrentAxis);
};
