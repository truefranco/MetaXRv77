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

#include "Interaction/Grabbable/IsdkOneGrabRotateTransformer.h"
#include "DrawDebugHelpers.h"
#include "HAL/IConsoleManager.h"

namespace isdk
{
extern TAutoConsoleVariable<bool> CVar_Meta_InteractionSDK_DebugInteractionVisuals;
}

UIsdkOneGrabRotateTransformer::UIsdkOneGrabRotateTransformer() {}

void UIsdkOneGrabRotateTransformer::BeginTransform(
    const TArray<FIsdkGrabPose>& GrabPoses,
    const FIsdkTargetTransform& Target)
{
  if (GrabPoses.IsEmpty())
  {
    return;
  }
  bIsActive = true;
  CachedSelectedPose = GrabPoses[0];
  CachedPoseVector = Target.ParentWorldTransform.InverseTransformVectorNoScale(
      CachedSelectedPose.Position() - Target.WorldTransform.GetLocation());
  CachedTargetRelativeRotation = Target.RelativeTransform.GetRotation();
}

FTransform UIsdkOneGrabRotateTransformer::UpdateTransform(
    const TArray<FIsdkGrabPose>& GrabPoses,
    const FIsdkTargetTransform& Target)
{
  if (GrabPoses.IsEmpty())
  {
    return Target.RelativeTransform;
  }
  auto CurrentPose = GetSelectedPose(GrabPoses);

  const FTransform& PivotTransform = FTransform(
      Target.ParentWorldTransform.GetRotation() * CachedTargetRelativeRotation,
      Target.WorldTransform.GetLocation());

  auto StartVector = PivotTransform.InverseTransformVector(
      Target.ParentWorldTransform.TransformVectorNoScale(CachedPoseVector));
  auto CurrentVector = PivotTransform.InverseTransformPosition(CurrentPose.Position());

  // Too close to the pivot origin to get a meaningful transformation
  if (StartVector.IsNearlyZero() || CurrentVector.IsNearlyZero())
  {
    return Target.RelativeTransform;
  }

  auto StartGrabAxis = TransformGrabVector(StartVector);
  auto CurrentGrabAxis = TransformGrabVector(CurrentVector);

  auto TransformRotation = FQuat::FindBetweenNormals(StartGrabAxis, CurrentGrabAxis);
  DrawDebug(PivotTransform, TransformRotation.GetRotationAxis(), StartGrabAxis, CurrentGrabAxis);
  auto UnconstrainedRotation = CachedTargetRelativeRotation * TransformRotation;

  const auto Axis = FIsdkTransformerUtils::GetAxis(Constraint.RotationAxis);

  auto ConstrainedRotation = UnconstrainedRotation;
  if (Constraint.bUseAngleFromAxisConstraint)
  {
    if (Constraint.RotationAxis != TEnumAsByte(EAxis::Type::None))
    {
      ConstrainedRotation = FIsdkTransformerUtils::GetConstrainedRotationFromAxis(
          UnconstrainedRotation, Axis, Constraint.MaxAngleFromAxis);
    }
  }
  else
  {
    if (Constraint.RotationAxis == TEnumAsByte(EAxis::Type::None))
    {
      ConstrainedRotation = FIsdkTransformerUtils::GetConstrainedTransformRotation(
          UnconstrainedRotation, Constraint.RotationConstraint);
    }
    else
    {
      ConstrainedRotation = FIsdkTransformerUtils::GetConstrainedTwistRotation(
          UnconstrainedRotation, Axis, Constraint.AxisAngleRange);
    }
  }

  return FTransform(
      ConstrainedRotation,
      Target.RelativeTransform.GetLocation(),
      Target.RelativeTransform.GetScale3D());
}

FTransform UIsdkOneGrabRotateTransformer::EndTransform(const FIsdkTargetTransform& Target)
{
  bIsActive = false;
  return Target.RelativeTransform;
}

void UIsdkOneGrabRotateTransformer::UpdateConstraints(const FIsdkTargetTransform& Target)
{
  // This transformer does not use Relative Constraints
}

FVector UIsdkOneGrabRotateTransformer::TransformGrabVector(const FVector& Vector)
{
  if (Constraint.RotationAxis != EAxis::Type::None && !Constraint.bUseAngleFromAxisConstraint)
  {
    const auto Axis = FIsdkTransformerUtils::GetAxis(Constraint.RotationAxis);
    auto Plane = FPlane(FVector::ZeroVector, Axis);
    return FVector::PointPlaneProject(Vector, Plane).GetSafeNormal();
  }
  else
  {
    return Vector.GetSafeNormal();
  }
}

void UIsdkOneGrabRotateTransformer::DrawDebug(
    const FTransform& PivotTransform,
    const FVector& Axis,
    const FVector& StartAxis,
    const FVector& CurrentAxis)
{
  if (isdk::CVar_Meta_InteractionSDK_DebugInteractionVisuals.GetValueOnAnyThread())
  {
    const auto Depth = ESceneDepthPriorityGroup::SDPG_Foreground;

    auto Origin = PivotTransform.GetLocation();
    DrawDebugSphere(GetWorld(), Origin, 1.0, 6, FColor::Emerald, false, 0.0, Depth, 0.1);

    const auto Length = 6.0;

    auto AxisWorld = PivotTransform.TransformPosition(Axis * Length);
    DrawDebugLine(GetWorld(), Origin, AxisWorld, FColor::Red, false, 0.0, Depth, 0.2);

    auto StartGrabWorld = PivotTransform.TransformPosition(StartAxis * Length);
    DrawDebugLine(GetWorld(), Origin, StartGrabWorld, FColor::Green, false, 0.0, Depth, 0.2);

    auto CurrentGrabWorld = PivotTransform.TransformPosition(CurrentAxis * Length);
    DrawDebugLine(GetWorld(), Origin, CurrentGrabWorld, FColor::Blue, false, 0.0, Depth, 0.2);
  }
}
