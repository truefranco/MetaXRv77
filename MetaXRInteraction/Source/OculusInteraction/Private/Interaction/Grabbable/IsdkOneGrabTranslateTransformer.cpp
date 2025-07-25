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

#include "Interaction/Grabbable/IsdkOneGrabTranslateTransformer.h"
#include "DrawDebugHelpers.h"
#include "HAL/IConsoleManager.h"

namespace isdk
{
extern TAutoConsoleVariable<bool> CVar_Meta_InteractionSDK_DebugInteractionVisuals;
}

UIsdkOneGrabTranslateTransformer::UIsdkOneGrabTranslateTransformer() {}

bool UIsdkOneGrabTranslateTransformer::IsTransformerActive() const
{
  return bIsActive;
}

int UIsdkOneGrabTranslateTransformer::MaxGrabPoints() const
{
  return 1;
}

void UIsdkOneGrabTranslateTransformer::BeginTransform(
    const TArray<FIsdkGrabPose>& GrabPoses,
    const FIsdkTargetTransform& Target)
{
  if (GrabPoses.IsEmpty())
  {
    return;
  }
  bIsActive = true;
  CachedSelectedPose = GrabPoses[0];
  const FVector ToTargetOffset =
      Target.WorldTransform.GetLocation() - CachedSelectedPose.Position();
  InitialTargetOffset = Target.ParentWorldTransform.InverseTransformVector(ToTargetOffset);
}

FTransform UIsdkOneGrabTranslateTransformer::UpdateTransform(
    const TArray<FIsdkGrabPose>& GrabPoses,
    const FIsdkTargetTransform& Target)
{
  if (GrabPoses.IsEmpty())
  {
    return Target.RelativeTransform;
  }
  CachedSelectedPose = GetSelectedPose(GrabPoses);
  const auto LocalPosition =
      Target.ParentWorldTransform.InverseTransformPosition(CachedSelectedPose.Position());
  const auto TargetRelativePosition = LocalPosition + InitialTargetOffset;
  const auto ConstrainedRelativePosition = FIsdkTransformerUtils::GetConstrainedTransformPosition(
      TargetRelativePosition, TargetPositionConstraint);
  DrawDebug(Target);

  // Return the local transform
  return FTransform(
      Target.RelativeTransform.GetRotation(),
      ConstrainedRelativePosition,
      Target.RelativeTransform.GetScale3D());
}

FTransform UIsdkOneGrabTranslateTransformer::EndTransform(const FIsdkTargetTransform& Target)
{
  bIsActive = false;
  return Target.RelativeTransform;
}

void UIsdkOneGrabTranslateTransformer::UpdateConstraints(const FIsdkTargetTransform& Target)
{
  TargetPositionConstraint = PositionConstraint;
  if (bIsRelativeConstraint)
  {
    TargetPositionConstraint = FIsdkTransformerUtils::GenerateParentPositionConstraints(
        PositionConstraint, Target.RelativeTransform.GetLocation());
  }
}

FIsdkGrabPose UIsdkOneGrabTranslateTransformer::GetSelectedPose(
    const TArray<FIsdkGrabPose>& GrabPoses) const
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

void UIsdkOneGrabTranslateTransformer::DrawDebug(const FIsdkTargetTransform& Target) const
{
  if (isdk::CVar_Meta_InteractionSDK_DebugInteractionVisuals.GetValueOnAnyThread())
  {
    const auto GrabLocation = CachedSelectedPose.Position();
    const auto GrabRotation = CachedSelectedPose.Orientation().Rotator();
    const auto TargetLocation = Target.WorldTransform.GetLocation();
    constexpr auto Depth = ESceneDepthPriorityGroup::SDPG_Foreground;
    DrawDebugCoordinateSystem(GetWorld(), GrabLocation, GrabRotation, 1.0, false, 0.0, Depth, 0.2);
    DrawDebugLine(GetWorld(), GrabLocation, TargetLocation, FColor::Red, false, 0.0, Depth, 0.2);
  }
}
