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

#include "InteractableVisuals/IsdkInteractableColorVisual.h"
#include "Subsystem/IsdkWorldSubsystem.h"

UIsdkInteractableColorVisual::UIsdkInteractableColorVisual()
{
  PrimaryComponentTick.bCanEverTick = true;
  bAutoActivate = true;
}

void UIsdkInteractableColorVisual::SetInteractable(
    TScriptInterface<IIsdkIInteractableState> InInteractable)
{
  if (IsValid(Interactable.GetObject()))
  {
    Interactable->GetInteractableStateChangedDelegate()->RemoveDynamic(
        this, &UIsdkInteractableColorVisual::HandleStateChange);
  }
  Interactable = InInteractable;
  if (IsValid(Interactable.GetObject()))
  {
    Interactable->GetInteractableStateChangedDelegate()->AddUniqueDynamic(
        this, &UIsdkInteractableColorVisual::HandleStateChange);
    if (HasBegunPlay())
    {
      SetImmediateTransitionToState(Interactable->GetInteractableState());
    }
  }
}

void UIsdkInteractableColorVisual::OnRegister()
{
  Super::OnRegister();
  if (IsValid(DynamicMaterial))
  {
    DynamicMaterial->SetVectorParameterValue(ParameterName, NormalColorState.Color);
  }
}

void UIsdkInteractableColorVisual::BeginPlay()
{
  Super::BeginPlay();
  if (IsValid(Interactable.GetObject()))
  {
    SetImmediateTransitionToState(Interactable->GetInteractableState());
  }
}

void UIsdkInteractableColorVisual::BeginDestroy()
{
  Super::BeginDestroy();
  if (IsValid(Interactable.GetObject()))
  {
    Interactable->GetInteractableStateChangedDelegate()->RemoveDynamic(
        this, &UIsdkInteractableColorVisual::HandleStateChange);
  }
}

void UIsdkInteractableColorVisual::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  if (ColorTransition.IsActive())
  {
    ColorTransition.UpdateTransition(DeltaTime);
    DynamicMaterial->SetVectorParameterValue(ParameterName, ColorTransition.GetCurrentColor());
  }
}

void UIsdkInteractableColorVisual::HandleStateChange(const FIsdkInteractableStateEvent& Event)
{
  SetTransitionToState(Event.Args.NewState);
}

void UIsdkInteractableColorVisual::SetTransitionToState(EIsdkInteractableState State)
{
  ColorTransition = ColorTransition.TransitionTo(GetColorState(State));
}

void UIsdkInteractableColorVisual::SetImmediateTransitionToState(EIsdkInteractableState State)
{
  ColorTransition = FIsdkColorTransition::ImmediateTransitionTo(GetColorState(State));
}

void UIsdkInteractableColorVisual::SetColorState(
    EIsdkInteractableState State,
    FIsdkColorState ColorState)
{
  switch (State)
  {
    case EIsdkInteractableState::Normal:
      NormalColorState = ColorState;
      break;
    case EIsdkInteractableState::Hover:
      HoverColorState = ColorState;
      break;
    case EIsdkInteractableState::Select:
      SelectColorState = ColorState;
      break;
    case EIsdkInteractableState::Disabled:
      DisabledColorState = ColorState;
      break;
  }
  if (!IsValid(Interactable.GetObject()))
    return;
  if (Interactable->GetInteractableState() != State)
    return;
  if (HasBegunPlay())
  {
    SetTransitionToState(State);
  }
  else
  {
    SetImmediateTransitionToState(State);
  }
}

FIsdkColorState UIsdkInteractableColorVisual::GetColorState(EIsdkInteractableState State) const
{
  switch (State)
  {
    case EIsdkInteractableState::Normal:
      return NormalColorState;
    case EIsdkInteractableState::Hover:
      return HoverColorState;
    case EIsdkInteractableState::Select:
      return SelectColorState;
    case EIsdkInteractableState::Disabled:
      return DisabledColorState;
  }
  return FIsdkColorState();
}
