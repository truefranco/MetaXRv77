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

#include "Interaction/GrabDetectors/IsdkRayGrabDetector.h"

#include "IsdkFunctionLibrary.h"
#include "OculusInteractionLog.h"
#include "Interaction/IsdkGrabbableComponent.h"
#include "Interaction/IsdkGrabberComponent.h"
#include "Utilities/IsdkDebugUtils.h"
#include "DrawDebugHelpers.h"
#include "Logging/LogVerbosity.h"
#include "VisualLogger/VisualLogger.h"

void UIsdkRayGrabDetector::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  FTransform PointerTransform = FTransform::Identity;
  GrabberComponent->GetPointerTransform(PointerTransform);
  const auto Start = PointerTransform.GetLocation();
  const auto End = PointerTransform.GetLocation() +
      PointerTransform.GetRotation().GetForwardVector() * RayLength;

  TArray<FHitResult> OutHits;
  const FCollisionObjectQueryParams ObjectQueryParams(ObjectQueryTypes);
  const FCollisionQueryParams CollisionQueryParams;
  GetWorld()->LineTraceMultiByObjectType(
      OutHits, Start, End, ObjectQueryParams, CollisionQueryParams);

  HoveredGrabbable = nullptr;
  for (const auto& Hit : OutHits)
  {
    const auto Grabbable = UIsdkFunctionLibrary::FindGrabbableByComponent(Hit.GetComponent());
    if (!Grabbable || !Grabbable->IsRayGrabAllowed())
    {
      continue;
    }

    // Must match on at least one input method to be considered hovered
    const bool bPinchMatches = Grabbable->IsGrabInputMethodAllowed(EIsdkGrabInputMethod::Pinch) &&
        GrabberComponent->IsGrabInputMethodAllowed(EIsdkGrabInputMethod::Pinch);
    const bool bPalmMatches = Grabbable->IsGrabInputMethodAllowed(EIsdkGrabInputMethod::Palm) &&
        GrabberComponent->IsGrabInputMethodAllowed(EIsdkGrabInputMethod::Palm);
    if (!bPinchMatches && !bPalmMatches)
    {
      continue;
    }

    // Don't detect grabbables which are already being grabbed.  This is mostly to keep consistent
    // with distance grab, which can trigger a lot of unintentional grabs if we allow this behavior.
    if (Grabbable->GetGrabTransformer() && Grabbable->GetGrabTransformer()->GetNumGrabbers() >= 1)
    {
      continue;
    }

    HoveredGrabbable = Grabbable;
    CurrentHit = Hit;
    break;
  }

  if (!IsValid(HoveredGrabbable))
  {
    CurrentHit = FHitResult();
  }
}

void UIsdkRayGrabDetector::Select(EIsdkGrabInputMethod InputMethod)
{
  Super::Select(InputMethod);

  SelectHit = CurrentHit;

  if (IsValid(HoveredGrabbable) && IsValid(GrabberComponent))
  {
    FTransform PointerTransform = FTransform::Identity;
    GrabberComponent->GetPointerTransform(PointerTransform);
    const auto ImpactZ =
        FVector::CrossProduct(CurrentHit.Normal, PointerTransform.GetRotation().GetForwardVector());
    const auto HitRotation = FRotationMatrix::MakeFromXZ(CurrentHit.Normal, ImpactZ).ToQuat();

    const FTransform ToHitFromWorld = FTransform(HitRotation, CurrentHit.Location);
    const FTransform ToObjectFromWorld = HoveredGrabbable->GetComponentTransform();
    SelectHitTransform = ToHitFromWorld * ToObjectFromWorld.Inverse();
  }
}

void UIsdkRayGrabDetector::Unselect()
{
  Super::Unselect();
  SelectHitTransform = FTransform::Identity;
}

UIsdkGrabbableComponent* UIsdkRayGrabDetector::GetHoveredGrabbable()
{
  return HoveredGrabbable;
}

void UIsdkRayGrabDetector::GetHit(FHitResult& OutHit) const
{
  // If we aren't selecting, the current hit is whatever the result of the most recent trace
  if (!bIsSelecting)
  {
    OutHit = CurrentHit;
    return;
  }

  // Otherwise, if we're selecting, we'll emulate a hit as if the most recent hit had moved along
  // perfectly with the grabbed object.  This, for example, helps keep the cursor perfectly aligned
  // with the grabbed object.
  if (IsValid(HoveredGrabbable) && IsValid(GrabberComponent))
  {
    OutHit = SelectHit;

    // Calculate the emulated hit's world transform
    const auto ToObjectFromWorld = HoveredGrabbable->GetComponentTransform();
    const auto ToHitFromObject = SelectHitTransform;
    const auto ToHitFromWorld = ToHitFromObject * ToObjectFromWorld;

    // Populate the emulated hit
    OutHit.Location = ToHitFromWorld.GetLocation();
    OutHit.Normal = ToHitFromWorld.GetRotation().GetForwardVector();
    OutHit.ImpactNormal = ToHitFromWorld.GetRotation().GetForwardVector();

    FTransform PointerTransform = FTransform::Identity;
    GrabberComponent->GetPointerTransform(PointerTransform);

    OutHit.TraceStart = PointerTransform.GetLocation();

    // Calculate an emulated trace end location based on where the pointer transform is
    // currently pointing
    const auto AdjustedTraceVector = (OutHit.Location - OutHit.TraceStart).GetSafeNormal();
    const auto AdjustedTraceEnd = PointerTransform.GetLocation() + AdjustedTraceVector * RayLength;
    OutHit.TraceEnd = AdjustedTraceEnd;

    OutHit.bStartPenetrating = CurrentHit.bStartPenetrating;
  }
}

UIsdkGrabbableComponent* UIsdkRayGrabDetector::GetGrabCandidate(EIsdkGrabInputMethod InputMethod)
{
  // For ray, the candidate is whatever is closest.  If the candidate is incompatible with the grab
  // type (pinch, palm, etc), it will simply not respond
  return HoveredGrabbable;
}

EIsdkGrabDetectorType UIsdkRayGrabDetector::GetGrabDetectorType() const
{
  return EIsdkGrabDetectorType::RayGrab;
}

void UIsdkRayGrabDetector::DrawDebugVisuals() const
{
  if (bDisableDebugVisuals)
  {
    return;
  }

  auto InteractorState = EIsdkInteractorState::Normal;
  if (bIsSelecting)
  {
    InteractorState = EIsdkInteractorState::Select;
  }
  else if (IsValid(HoveredGrabbable))
  {
    InteractorState = EIsdkInteractorState::Hover;
  }

  FHitResult RayHit;
  GetHit(RayHit);
  FVector TraceStart = RayHit.TraceStart;
  FVector TraceEnd = RayHit.bBlockingHit ? RayHit.Location : RayHit.TraceEnd;

  FColor DebugColor = UIsdkDebugUtils::GetInteractorStateDebugColor(InteractorState);

  const auto Thickness = bIsSelecting ? 0.3f : 0.2f;
  DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, -1, 0, 0.3f);
  UE_VLOG_SEGMENT_THICK(
      GrabberComponent->GetOwner(),
      LogOculusInteraction,
      Verbose,
      TraceStart,
      TraceEnd,
      DebugColor,
      Thickness,
      TEXT_EMPTY);
}
