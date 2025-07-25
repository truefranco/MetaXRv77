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

#include "Interaction/Motion/IsdkGrabMotion.h"
#include "Interaction/IsdkGrabbableComponent.h"
#include "Interaction/IsdkGrabberComponent.h"
#include "Interaction/Grabbable/IsdkGrabTransformerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"
#include "DrawDebugHelpers.h"
#include "Logging/LogVerbosity.h"
#include "VisualLogger/VisualLogger.h"
#include "Engine/World.h"

void UIsdkGrabMotion::Start(
    UIsdkGrabbableComponent* InGrabbableComponent,
    UIsdkGrabberComponent* InGrabberComponent,
    UIsdkGrabTransformerComponent* InGrabTransformerComponent)
{
  GrabbableComponent = InGrabbableComponent;
  GrabberComponent = InGrabberComponent;
  GrabTransformerComponent = InGrabTransformerComponent;

  if (!HasValidDependencies())
  {
    Stop();
    return;
  }

  bIsActive = true;
}

void UIsdkGrabMotion::Stop()
{
  bIsActive = false;
}

bool UIsdkGrabMotion::IsActive() const
{
  return bIsActive;
}

bool UIsdkGrabMotion::HasValidDependencies() const
{
  return IsValid(GrabbableComponent) && IsValid(GrabberComponent) &&
      IsValid(GrabTransformerComponent) &&
      IsValid(GrabbableComponent->GetGrabTransformer()->GetTransformTarget());
}

const FTransform& UIsdkGrabMotion::GetTransform() const
{
  return Transform;
}

UWorld* UIsdkGrabMotion::GetWorld() const
{
  if (IsValid(GrabbableComponent))
  {
    return GrabbableComponent->GetWorld();
  }

  if (IsValid(GrabberComponent))
  {
    return GrabberComponent->GetWorld();
  }

  return Super::GetWorld();
}

void UIsdkPullToHandGrabMotion::Start(
    UIsdkGrabbableComponent* InGrabbableComponent,
    UIsdkGrabberComponent* InGrabberComponent,
    UIsdkGrabTransformerComponent* InGrabTransformerComponent)
{
  Super::Start(InGrabbableComponent, InGrabberComponent, InGrabTransformerComponent);
  StartTimestamp = GetWorld()->GetTimeSeconds();

  if (!HasValidDependencies())
  {
    Stop();
    return;
  }

  bHasReachedTarget = false;
  StartTransform = GrabbableComponent->GetComponentTransform();
  Transform = StartTransform;

  // Track the difference in rotation between the grabber and grabbable so we can preserve it during
  // motion.
  FTransform GrabberTransform;
  GrabberComponent->GetGrabberTransform(GrabberTransform);
  StartRotationOffset = GrabberTransform.GetRotation().Inverse() * StartTransform.GetRotation();
}

void UIsdkPullToHandGrabMotion::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (!HasValidDependencies())
  {
    Stop();
    return;
  }

  FTransform GrabberTransform;
  GrabberComponent->GetGrabberTransform(GrabberTransform);

  if (!bHasReachedTarget)
  {
    // While the grabbable is in-transit to the grabber, interpolate it in
    if (bUseAbsoluteTime)
    {
      // If using absolute time, we'll interpolate the grabbable toward the grabber
      // Note, this may look odd if there's a lot of movement in the grabber at high values of
      // PullTime It is expected tat PullTime is fairly low here
      float Alpha =
          FMath::Clamp((GetWorld()->GetTimeSeconds() - StartTimestamp) / PullTime, 0.f, 1.f);

      // Use pull curve to adjust alpha
      if (IsValid(PullCurve))
      {
        Alpha = PullCurve->GetFloatValue(Alpha);
      }
      Transform = UKismetMathLibrary::TLerp(StartTransform, GrabberTransform, Alpha);

      if (FMath::IsNearlyEqual(Alpha, 1.f))
      {
        bHasReachedTarget = true;
      }
    }
    else
    {
      auto FinalPullSpeed = PullSpeed;
      if (PullCurve)
      {
        const auto ElapsedTime = GetWorld()->GetTimeSeconds() - StartTimestamp;
        FinalPullSpeed = PullCurve->GetFloatValue(ElapsedTime) * PullSpeed;
      }
      // If using PullSpeed, just move the grabbable toward the grabber at a fixed rate
      const FVector CurrentLocation = Transform.GetLocation();
      const FVector TargetLocation = GrabberTransform.GetLocation();
      const FVector TotalDelta = TargetLocation - CurrentLocation;
      FVector DesiredDelta = TotalDelta.GetSafeNormal() * FinalPullSpeed * DeltaTime;
      if (DesiredDelta.Length() > TotalDelta.Length())
      {
        DesiredDelta = TotalDelta;
        bHasReachedTarget = true;
      }

      Transform.SetLocation(Transform.GetLocation() + DesiredDelta);
    }
  }
  else
  {
    // Once we've reached the target location, we should just snap to the grabber
    Transform.SetLocation(GrabberTransform.GetLocation());
  }

  Transform.SetRotation(GrabberTransform.GetRotation() * StartRotationOffset);
}

bool UIsdkPullToHandGrabMotion::IsUsingAbsoluteTime() const
{
  return bUseAbsoluteTime;
}

float UIsdkPullToHandGrabMotion::GetPullSpeed() const
{
  return PullSpeed;
}

float UIsdkPullToHandGrabMotion::GetPullTime() const
{
  return PullTime;
}

const FTransform& UIsdkPullToHandGrabMotion::GetStartTransform() const
{
  return StartTransform;
}

void UIsdkRelativeToPointerGrabMotion::Start(
    UIsdkGrabbableComponent* InGrabbableComponent,
    UIsdkGrabberComponent* InGrabberComponent,
    UIsdkGrabTransformerComponent* InGrabTransformerComponent)
{
  Super::Start(InGrabbableComponent, InGrabberComponent, InGrabTransformerComponent);

  if (!HasValidDependencies())
  {
    Stop();
    return;
  }

  FTransform ToPointerFromWorld = FTransform::Identity;
  GrabberComponent->GetPointerTransform(ToPointerFromWorld);
  const auto ToObjectFromWorld = GrabbableComponent->GetComponentTransform();
  const auto ToWorldFromPointer = ToPointerFromWorld.Inverse();
  ToObjectFromPointer = ToObjectFromWorld * ToWorldFromPointer;
  Transform = ToObjectFromPointer * ToPointerFromWorld;
}

void UIsdkRelativeToPointerGrabMotion::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (!HasValidDependencies())
  {
    Stop();
    return;
  }

  FTransform ToPointerFromWorld = FTransform::Identity;
  GrabberComponent->GetPointerTransform(ToPointerFromWorld);
  Transform = ToObjectFromPointer * ToPointerFromWorld;
}

void UIsdkManipulateInPlaceGrabMotion::Start(
    UIsdkGrabbableComponent* InGrabbableComponent,
    UIsdkGrabberComponent* InGrabberComponent,
    UIsdkGrabTransformerComponent* InGrabTransformerComponent)
{
  Super::Start(InGrabbableComponent, InGrabberComponent, InGrabTransformerComponent);

  if (!HasValidDependencies())
  {
    Stop();
    return;
  }

  FTransform GrabberTransform;
  GrabberComponent->GetGrabberTransform(GrabberTransform);

  ToObjectStartFromWorld = GrabbableComponent->GetComponentTransform();
  ToGrabberStartFromWorld = GrabberTransform;
  ToObjectFromGrabber = ToObjectStartFromWorld * ToGrabberStartFromWorld.Inverse();

  Transform = ToObjectStartFromWorld;
}

void UIsdkManipulateInPlaceGrabMotion::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (!HasValidDependencies())
  {
    Stop();
    return;
  }

  FTransform ToGrabberFromWorld;
  GrabberComponent->GetGrabberTransform(ToGrabberFromWorld);
  const auto ToWorldFromGrabberStart = ToGrabberStartFromWorld.Inverse();
  const auto ToGrabberFromGrabberStart = ToGrabberFromWorld * ToWorldFromGrabberStart;
  const auto ToNewObjectFromObjectStart = ToGrabberFromGrabberStart;
  const auto LocationFix = FTransform(
      ToGrabberStartFromWorld.GetRotation(),
      ToObjectStartFromWorld.GetLocation(),
      ToGrabberStartFromWorld.GetScale3D());
  auto RotationFix =
      FTransform(ToObjectFromGrabber.GetRotation(), FVector::ZeroVector, FVector::OneVector);

  Transform = RotationFix * ToNewObjectFromObjectStart * LocationFix;
}
