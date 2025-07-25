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

#include "Interaction/Grabbable/IsdkGrabTransformerComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "VisualLogger/VisualLogger.h"
#include "Engine/World.h"
#include "Interaction/Grabbable/IsdkGrabFreeTransformer.h"
#include "Interaction/Grabbable/IsdkTransformer.h"
#include "Interaction/Motion/IsdkGrabMotion.h"

UIsdkGrabTransformerComponent::UIsdkGrabTransformerComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
  ThrowSettings = FIsdkThrowableSettings();
  DistanceGrabMotion =
      CreateDefaultSubobject<UIsdkPullToHandGrabMotion>("DefaultDistanceGrabMotion");
  SingleGrabTransformerObject =
      CreateDefaultSubobject<UIsdkGrabFreeTransformer>("DefaultSingleGrabTransformer");
  MultiGrabTransformerObject =
      CreateDefaultSubobject<UIsdkGrabFreeTransformer>("DefaultMultiGrabTransformer");
}

FIsdkCancelGrabEventDelegate* UIsdkGrabTransformerComponent::GetCancelGrabEventDelegate()
{
  return &CancelGrabEvent;
}

FIsdkGrabTransformerEventDelegate* UIsdkGrabTransformerComponent::GetGrabTransformerEventDelegate()
{
  return &GrabTransformerEvent;
}

EIsdkMultiGrabBehavior UIsdkGrabTransformerComponent::GetMultiGrabBehavior() const
{
  return MultiGrabBehavior;
}

int UIsdkGrabTransformerComponent::GetNumGrabbers() const
{
  return GrabPoses.GetSelectPoses().Num();
}

void UIsdkGrabTransformerComponent::BeginPlay()
{
  Super::BeginPlay();

  GrabPoses.ClearPoses();

  // Add Transform Target from name if there is no Transform Target
  if (IsValid(TransformTarget))
  {
    SetTransformTarget(TransformTarget);
  }
  else if (TransformTargetName != NAME_None)
  {
    TArray<USceneComponent*> Children;
    GetOwner()->GetRootComponent()->GetChildrenComponents(true, Children);
    Children.Add(GetOwner()->GetRootComponent());
    for (USceneComponent* Child : Children)
    {
      const auto ChildName = Child->GetFName();
      if (ChildName == TransformTargetName)
      {
        SetTransformTarget(Child);
        break;
      }
    }
  }
  else
  {
    // Assume root component is transform target if nothing is set.
    SetTransformTarget(GetOwner()->GetRootComponent());
  }

  if (bIsThrowable && !IsValid(ThrowableComponent))
  {
    ThrowableComponent = Cast<UIsdkThrowable>(GetOwner()->AddComponentByClass(
        UIsdkThrowable::StaticClass(), false, FTransform::Identity, true));
    ThrowableComponent->SetThrowableSettings(ThrowSettings);
    GetOwner()->FinishAddComponent(ThrowableComponent, false, FTransform::Identity);
  }

  UpdateTransformerConstraints();
}

void UIsdkGrabTransformerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  EndTransform();
  Super::EndPlay(EndPlayReason);
}

void UIsdkGrabTransformerComponent::ProcessPointerEvent(const FIsdkInteractionPointerEvent& Event)
{
  GrabPoses.UpdatePoseArrays(Event);
  LastInteractor = Event.Interactor;
  switch (Event.Type)
  {
    case EIsdkPointerEventType::Select:
    case EIsdkPointerEventType::Unselect:
    case EIsdkPointerEventType::Cancel:
      EndTransform();
      // Calling "BeginTransform" here because "Select", "Unselect", and "Cancel" change the number
      // of grab poses changes, so the grab transformer could go from a None to Single or Single to
      // Multi, and vice versa. It is possible that after a "Unselect" or "Cancel" the state goes to
      // None in which case the "BeginTransform" function would not trigger any "Transformer" as it
      // checks the number of grab points before activation.
      BeginTransform();
      break;
    case EIsdkPointerEventType::Move:
      UpdateTransform();
      break;
    default:
      break;
  }
}

UIsdkGrabMotion* UIsdkGrabTransformerComponent::GetDistanceGrabMotion() const
{
  return DistanceGrabMotion;
}

void UIsdkGrabTransformerComponent::ForceCancel()
{
  const TArray<FIsdkGrabPose> PoseIdentifiers = GrabPoses.GetSelectPoses();
  GrabPoses.ClearPoses();

  // End transform here should be called after the grab poses are removed to make sure the reset
  // physics state code is executed
  EndTransform();
  for (const FIsdkGrabPose& Pose : PoseIdentifiers)
  {
    CancelGrabEvent.Broadcast(Pose.Identifier, this);
  }
}

void UIsdkGrabTransformerComponent::SetSingleGrabTransformer(
    TScriptInterface<IIsdkITransformer> Transformer)
{
  // Cast interface to concrete UIsdkGrabTransformer.
  const auto TransformerObject = Cast<UIsdkTransformer>(Transformer.GetObject());
  if (Transformer.GetObject() && !TransformerObject)
  {
    ensureMsgf(false, TEXT("Transformer must be a subclass of UIsdkGrabTransformer"));
    return;
  }

  SetSingleGrabTransformerObject(TransformerObject);
}

void UIsdkGrabTransformerComponent::SetSingleGrabTransformerObject(UIsdkTransformer* Transformer)
{
  SingleGrabTransformerObject = Transformer;
  if (IsValid(SingleGrabTransformerObject))
  {
    SingleGrabTransformerObject->UpdateConstraints(TargetInitialTransform);
  }
  RestartTransformer();
}

TScriptInterface<IIsdkITransformer> UIsdkGrabTransformerComponent::GetSingleGrabTransformer() const
{
  return SingleGrabTransformerObject;
}

UIsdkTransformer* UIsdkGrabTransformerComponent::GetSingleGrabTransformerObject() const
{
  return SingleGrabTransformerObject;
}

void UIsdkGrabTransformerComponent::SetMultiGrabTransformer(
    TScriptInterface<IIsdkITransformer> Transformer)
{
  // Cast interface to concrete UIsdkGrabTransformer.
  const auto TransformerObject = Cast<UIsdkTransformer>(Transformer.GetObject());
  if (Transformer.GetObject() && !TransformerObject)
  {
    ensureMsgf(false, TEXT("Transformer must be a subclass of UIsdkGrabTransformer"));
    return;
  }

  SetMultiGrabTransformerObject(TransformerObject);
}

void UIsdkGrabTransformerComponent::SetMultiGrabTransformerObject(UIsdkTransformer* Transformer)
{
  MultiGrabTransformerObject = Transformer;
  if (IsValid(MultiGrabTransformerObject))
  {
    MultiGrabTransformerObject->UpdateConstraints(TargetInitialTransform);
  }
  RestartTransformer();
}

TScriptInterface<IIsdkITransformer> UIsdkGrabTransformerComponent::GetMultiGrabTransformer() const
{
  return MultiGrabTransformerObject;
}

UIsdkTransformer* UIsdkGrabTransformerComponent::GetMultiGrabTransformerObject() const
{
  return MultiGrabTransformerObject;
}

void UIsdkGrabTransformerComponent::SetTransformTarget(USceneComponent* NewTarget)
{
  TransformTarget = NewTarget;
  TargetInitialTransform = FIsdkTargetTransform(TransformTarget);
  UpdateTransformerConstraints();
}

USceneComponent* UIsdkGrabTransformerComponent::GetTransformTarget() const
{
  return TransformTarget;
}

const FIsdkGrabPoseCollection& UIsdkGrabTransformerComponent::GetGrabPoses() const
{
  return GrabPoses;
}

TScriptInterface<IIsdkITransformer> UIsdkGrabTransformerComponent::GetActiveGrabTransformer() const
{
  return ActiveGrabTransformer;
}

void UIsdkGrabTransformerComponent::BeginTransform()
{
  if (!IsValid(TransformTarget))
  {
    return;
  }

  int SelectCount = GrabPoses.GetSelectPoses().Num();

  switch (SelectCount)
  {
    case 0:
      ActiveGrabTransformer = {};
      break;
    case 1:
      ActiveGrabTransformer = SingleGrabTransformerObject;
      break;
    default:
      ActiveGrabTransformer = MultiGrabTransformerObject;
      break;
  }

  if (!IsValid(ActiveGrabTransformer.GetObject()))
  {
    return;
  }

  // If a transformer was activated then stop the physics simulation
  // and cache its current state
  if (!bWasPhysicsCached)
  {
    bWasPhysicsCached = true;
    auto bIsSimulatingPhysics = TransformTarget->IsSimulatingPhysics();
    if (bIsSimulatingPhysics)
    {
      auto* TargetPrimitiveComponent = Cast<UPrimitiveComponent>(TransformTarget);
      if (IsValid(TargetPrimitiveComponent))
      {
        bHadGravity = TargetPrimitiveComponent->IsGravityEnabled();
        const auto Velocity = TargetPrimitiveComponent->GetPhysicsLinearVelocity();
        TargetPrimitiveComponent->AddForce(-Velocity);
        TargetPrimitiveComponent->SetSimulatePhysics(false);
        TargetPrimitiveComponent->SetEnableGravity(false);
      }
    }
    bWasSimulatingPhysics = bIsSimulatingPhysics;
  }

  if (bIsThrowable && IsValid(ThrowableComponent))
  {
    ThrowableComponent->StartTracking(TransformTarget);
  }

  ActiveGrabTransformer->BeginTransform(
      GrabPoses.GetSelectPoses(), FIsdkTargetTransform(TransformTarget));
  GrabTransformerEvent.Broadcast(TransformEvent::BeginTransform, this);
}

FTransform UIsdkGrabTransformerComponent::CalculateInteractorSnapTransform(
    FTransform& TargetTransformIn,
    FTransform& InteractorTransformIn)
{
  // Calculate Rotated Component Transform
  FTransform RotatedGrabTransform;
  RotatedGrabTransform.SetRotation(
      TargetTransformIn.GetRotation() * InteractorSnapOffset.GetRotation());
  RotatedGrabTransform.SetScale3D(InteractorSnapOffset.GetScale3D());
  RotatedGrabTransform.SetLocation(
      TargetTransformIn.GetLocation() +
      TargetTransformIn.GetRotation().RotateVector(InteractorSnapOffset.GetLocation()));

  // Calculate Rotation (finding the rotation for the parent so the child component can match
  // rotation with the interactor)
  FRotator FinalRotation = UKismetMathLibrary::ComposeRotators(
      RotatedGrabTransform.GetRotation().Rotator().GetInverse(),
      InteractorTransformIn.GetRotation().Rotator());

  // Calculate Location (Snapping the component location to the interactor)
  FVector FinalLocation =
      (InteractorTransformIn.GetLocation() - RotatedGrabTransform.GetLocation());

  // Compose Final Transform
  FTransform FinalTransform = TargetTransformIn;
  FinalTransform.SetRotation(FinalRotation.Quaternion() * TargetTransformIn.GetRotation());
  FinalTransform.SetLocation(TargetTransformIn.GetLocation() + FinalLocation);

  return FinalTransform;
}

void UIsdkGrabTransformerComponent::UpdateTransform()
{
  if (!IsValid(TransformTarget))
  {
    return;
  }

  if (!IsValid(ActiveGrabTransformer.GetObject()))
  {
    return;
  }

  if (bQueuedInteractorSnap)
  {
    ExecuteInteractorSnap(QueuedInteractorSnapPoint, QueuedMoveSnapDuration);
    bQueuedInteractorSnap = false;
    QueuedInteractorSnapPoint = FTransform::Identity;
    QueuedMoveSnapDuration = 0.f;
  }

  // If we are executing an interactor snap, calculate the new transform using the interactor and
  // offset and apply it to the target transform instead of utilizing
  // ActiveGrabTransformer->UpdateTransform(). We currently only employ a snap if we're grabbing
  // with one hand.
  if (bInteractorSnapEnabled && GetNumGrabbers() == 1)
  {
    if (IsValid(LastInteractor))
    {
      FTransform TargetTransform = TransformTarget->GetComponentToWorld();
      FTransform InteractorTransform = LastInteractor->GetComponentToWorld();
      FTransform ResultTransform =
          CalculateInteractorSnapTransform(TargetTransform, InteractorTransform);

      // We initially execute the calculation twice as the snap transform is relative to the target
      // component location and rotation
      if (bFirstSnapFrame)
      {
        ResultTransform = CalculateInteractorSnapTransform(ResultTransform, InteractorTransform);
        bFirstSnapFrame = false;
      }

      // Are we Move Snapping?
      if (bInteractorMoveSnapEnabled && MoveSnapState.SecondsDuration != 0.f)
      {
        const float MoveProgressAlpha = FMath::Min(
            (GetWorld()->GetRealTimeSeconds() - MoveSnapState.StartTime) /
                MoveSnapState.SecondsDuration,
            1.f);
        ResultTransform = UKismetMathLibrary::TLerp(
            MoveSnapState.StartingTransform, ResultTransform, MoveProgressAlpha);

        if (MoveProgressAlpha >= 1.f)
        {
          bInteractorMoveSnapEnabled = false;
        }
      }

      TransformTarget->SetRelativeTransform(ResultTransform);
    }
    else
    {
      UE_LOG(
          LogOculusInteraction,
          Error,
          TEXT(
              "UIsdkGrabTransformerComponent::UpdateTransform() - LastInteractor for Interactor Snapping was invalid!"));
      // Defaulting to normal behavior
      const auto TargetRelativeTransform = ActiveGrabTransformer->UpdateTransform(
          GrabPoses.GetSelectPoses(), FIsdkTargetTransform(TransformTarget));
      TransformTarget->SetRelativeTransform(TargetRelativeTransform);
    }
  }
  else
  {
    const auto TargetRelativeTransform = ActiveGrabTransformer->UpdateTransform(
        GrabPoses.GetSelectPoses(), FIsdkTargetTransform(TransformTarget));
    TransformTarget->SetRelativeTransform(TargetRelativeTransform);
  }
  GrabTransformerEvent.Broadcast(TransformEvent::UpdateTransform, this);
}

void UIsdkGrabTransformerComponent::EndTransform()
{
  if (!IsValid(TransformTarget))
  {
    return;
  }

  if (!IsValid(ActiveGrabTransformer.GetObject()))
  {
    return;
  }

  auto TargetRelativeTransform =
      ActiveGrabTransformer->EndTransform(FIsdkTargetTransform(TransformTarget));
  TransformTarget->SetRelativeTransform(TargetRelativeTransform);
  GrabTransformerEvent.Broadcast(TransformEvent::EndTransform, this);

  ActiveGrabTransformer = {};

  LastInteractor = nullptr;
  bInteractorSnapEnabled = false;
  bInteractorMoveSnapEnabled = false;
  bFirstSnapFrame = false;
  InteractorSnapOffset = FTransform::Identity;
  MoveSnapState.ResetState();

  // Target was released
  if (GrabPoses.GetSelectPoses().IsEmpty())
  {
    bWasPhysicsCached = false;
    auto* TargetPrimitiveComponent = Cast<UPrimitiveComponent>(TransformTarget);
    if (IsValid(TargetPrimitiveComponent))
    {
      TargetPrimitiveComponent->SetSimulatePhysics(bWasSimulatingPhysics);

      // Throw
      if (bIsThrowable && IsValid(ThrowableComponent))
      {
        const FVector ReleaseVelocity = ThrowableComponent->GetVelocity();
        const FVector AngularVelocity = ThrowableComponent->GetAngularVelocity().Euler();

        TargetPrimitiveComponent->SetPhysicsLinearVelocity(ReleaseVelocity);
        TargetPrimitiveComponent->SetPhysicsAngularVelocityInDegrees(AngularVelocity);
        TargetPrimitiveComponent->SetEnableGravity(
            bRetainGravityWhenThrown ? bHadGravity : !bHadGravity);

        ThrowableComponent->StopTracking();
      }
    }
  }
}

void UIsdkGrabTransformerComponent::RestartTransformer()
{
  EndTransform();
  BeginTransform();
}

void UIsdkGrabTransformerComponent::UpdateTransformerConstraints()
{
  if (IsValid(SingleGrabTransformerObject))
  {
    SingleGrabTransformerObject->UpdateConstraints(TargetInitialTransform);
  }
  if (IsValid(MultiGrabTransformerObject))
  {
    MultiGrabTransformerObject->UpdateConstraints(TargetInitialTransform);
  }
}

void UIsdkGrabTransformerComponent::ExecuteInteractorSnap(
    FTransform& RelativeSnapPoint,
    float MoveSnapDuration)
{
  if (MoveSnapDuration > 0.f)
  {
    bInteractorMoveSnapEnabled = true;
    MoveSnapState.StartingTransform = TransformTarget->GetComponentToWorld();
    MoveSnapState.StartTime = GetWorld()->GetRealTimeSeconds();
    MoveSnapState.SecondsDuration = MoveSnapDuration;
  }
  bInteractorSnapEnabled = true;
  bFirstSnapFrame = true;
  InteractorSnapOffset = RelativeSnapPoint;
}

void UIsdkGrabTransformerComponent::SetInteractorSnap(
    FTransform& RelativeSnapPoint,
    float MoveSnapDuration = 0.f)
{
  // Interrupted snap, queue this requested snap until our next update
  if (bInteractorSnapEnabled && LastInteractor != nullptr)
  {
    bQueuedInteractorSnap = true;
    QueuedInteractorSnapPoint = RelativeSnapPoint;
    QueuedMoveSnapDuration = MoveSnapDuration;
  }
  else
  {
    ExecuteInteractorSnap(RelativeSnapPoint, MoveSnapDuration);
  }
}
