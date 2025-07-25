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
#include "OculusInteractionLog.h"
#include "UObject/Interface.h"
#include "StructTypes.h"
#include "Components/SceneComponent.h"
#include "IsdkITransformer.generated.h"

/**
 * @brief Stores the pose and identifier meant to be used in the Transformer computation.
 */
USTRUCT(
    BlueprintType,
    meta = (HasNativeMake = "OculusInteraction.IsdkFunctionLibrary.MakeGrabPoseStruct"))
struct FIsdkGrabPose
{
  GENERATED_BODY()

  FIsdkGrabPose() : FIsdkGrabPose(0, FIsdkPosef()) {}
  /* @brief Constructor
   @param Identifier Should represent the source of the pose, be identifiable by it, and be unique
 in the Transformer.
 * @param Pose represents the modification point of the source, e.g. the pinch point, palm center,
 snap point, etc..
 */
  FIsdkGrabPose(int Identifier, const FIsdkPosef& Pose) : Identifier(Identifier), Pose(Pose) {}

  // Should represent the source of the pose, be identifiable by it, and be different to any other
  // sent to the same transformer
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  int Identifier = 0;

  // represents the modification point of the source, e.g. the pinch point, palm grab center, snap
  // point, etc..
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  FIsdkPosef Pose = FIsdkPosef();

  /**
   * @brief Returns the position/location of the pose
   * @return FVector The local space position of the pose
   */
  FVector Position() const
  {
    return FVector(Pose.Position);
  }
  /**
   * @brief Returns the rotation of the pose
   * @return FQuat The local quaternion rotation of the pose
   */
  FQuat Orientation() const
  {
    return Pose.Orientation;
  }
};

/**
 * @brief Stores the transformation matrices that represents the current state of the target
 * USceneComponent the user is trying to update through a Transformer.
 */
USTRUCT(
    BlueprintType,
    meta = (HasNativeMake = "OculusInteraction.IsdkFunctionLibrary.MakeTargetTransformStruct"))
struct FIsdkTargetTransform
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  FTransform WorldTransform;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  FTransform RelativeTransform;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  FTransform ParentWorldTransform;

  FIsdkTargetTransform()
  {
    WorldTransform = FTransform::Identity;
    RelativeTransform = FTransform::Identity;
    ParentWorldTransform = FTransform::Identity;
  }

  FIsdkTargetTransform(const USceneComponent* Target)
  {
    ParentWorldTransform = FTransform::Identity;
    if (!IsValid(Target))
    {
      UE_LOG(LogOculusInteraction, Warning, TEXT("Null Target provided to FIsdkTargetTransform()"))
      return;
    }

    WorldTransform = Target->GetComponentTransform();
    RelativeTransform = Target->GetRelativeTransform();
    if (Target->GetAttachParent() != nullptr)
    {
      ParentWorldTransform = Target->GetAttachParent()->GetComponentTransform();
    }
  }
};

UINTERFACE(BlueprintType, Category = "InteractionSDK")
class OCULUSINTERACTION_API UIsdkITransformer : public UInterface
{
  GENERATED_BODY()
};

/**
 * @class IIsdkITransformer
 * @brief Interface that establishes baseline functions for child transformers to operate on
 * their parent actors/meshes, including updating the transform with constraints and returning state
 * @addtogroup InteractionSDK
 */
class OCULUSINTERACTION_API IIsdkITransformer
{
  GENERATED_BODY()

 public:
  /**
   * @brief Initialize the transformation.
   */
  virtual void BeginTransform(
      const TArray<FIsdkGrabPose>& SelectPoses,
      const FIsdkTargetTransform& Target) PURE_VIRTUAL(IIsdkITransformer::BeginTransform, ;);

  /**
   * @brief Flush any local state and generate a final update to the target transform, for example
   * finishing some interpolation.
   */
  [[nodiscard]] virtual FTransform EndTransform(const FIsdkTargetTransform& Target)
      PURE_VIRTUAL(IIsdkITransformer::EndTransform, return FTransform::Identity;);

  /**
   * @brief Generate the transform update,the result should be a FTransform relative to the parent.
   */
  [[nodiscard]] virtual FTransform UpdateTransform(
      const TArray<FIsdkGrabPose>& SelectPoses,
      const FIsdkTargetTransform& Target)
      PURE_VIRTUAL(IIsdkITransformer::UpdateTransform, return FTransform::Identity;);

  /**
   * @brief Meant to be used to generate constraints based on the state of the target at some point
   * in time.
   */
  virtual void UpdateConstraints(const FIsdkTargetTransform& Target)
      PURE_VIRTUAL(IIsdkITransformer::UpdateConstraints, ;);

  /**
   * @brief Returns whether or not this transformer is currently active.
   */
  virtual bool IsTransformerActive() const PURE_VIRTUAL(IIsdkITransformer::IsActive, return false;);

  /**
   * @brief Returns maximum grab points, -1 if infinite.
   */
  virtual int MaxGrabPoints() const PURE_VIRTUAL(IIsdkITransformer::MaxGrabPoints, return 0;);
};
