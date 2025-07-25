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

#include "Rig/IsdkInteractionGroupRigComponent.h"

#include "Core/IsdkConditionalBool.h"
#include "Subsystem/IsdkWorldSubsystem.h"

UIsdkInteractionGroupRigComponent::UIsdkInteractionGroupRigComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
  bAutoActivate = true;
}

void UIsdkInteractionGroupRigComponent::BeginPlay()
{
  Super::BeginPlay();

  UIsdkWorldSubsystem& WorldSubsystem = UIsdkWorldSubsystem::Get(GetWorld());

  WorldSubsystem.GetFrameFinishedEventDelegate().AddUniqueDynamic(
      this, &UIsdkInteractionGroupRigComponent::HandleIsdkFrameFinished);
}

void UIsdkInteractionGroupRigComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
  UIsdkWorldSubsystem& WorldSubsystem = UIsdkWorldSubsystem::Get(GetWorld());

  WorldSubsystem.GetFrameFinishedEventDelegate().RemoveDynamic(
      this, &UIsdkInteractionGroupRigComponent::HandleIsdkFrameFinished);
}

void UIsdkInteractionGroupRigComponent::HandleInteractorStateChanged(
    const FIsdkInteractorStateEvent& InteractorState)
{
  StateChangedEvents.Add(InteractorState);
}

UIsdkConditional* UIsdkInteractionGroupRigComponent::AddInteractor(
    UObject* Interactor,
    FIsdkInteractorStateChanged& StateChangedDelegate,
    FIsdkInteractorGroupMember::CalculateStateFn CalculateInteractorState,
    FIsdkInteractionGroupMemberBehavior Behavior)
{
  FIsdkInteractorGroupMember MemberInfo{
      Behavior, std::move(CalculateInteractorState), NewObject<UIsdkConditionalBool>()};
  MemberInfo.IsEnabledConditional->SetValue(true);

  InteractorMemberInfoMap.Add(Interactor, MemberInfo);
  StateChangedDelegate.AddUniqueDynamic(
      this, &UIsdkInteractionGroupRigComponent::HandleInteractorStateChanged);

  return MemberInfo.IsEnabledConditional;
}

void UIsdkInteractionGroupRigComponent::RemoveInteractor(UObject* Interactor)
{
  if (Interactor != nullptr)
  {
    const auto* InteractorMemberInfo = InteractorMemberInfoMap.Find(Interactor);
    if (InteractorMemberInfo != nullptr)
    {
      InteractorMemberInfo->IsEnabledConditional->SetValue(false);
      InteractorMemberInfoMap.Remove(Interactor);
    }

    BlockingSelectors.Remove(Interactor);
  }
}

UIsdkConditional* UIsdkInteractionGroupRigComponent::FindConditionalForInteractor(
    UObject* Interactor)
{
  const auto* InteractorMemberInfo = InteractorMemberInfoMap.Find(Interactor);
  return InteractorMemberInfo != nullptr ? InteractorMemberInfo->IsEnabledConditional : nullptr;
}

void UIsdkInteractionGroupRigComponent::HandleIsdkFrameFinished()
{
  // Keep watching interactors even if IsActive is false, so that the Selectors and NearFieldHovers
  // sets are kept in sync.
  for (const auto& Event : StateChangedEvents)
  {
    const auto* MemberInfo = GetMemberInfoFromEvent(Event);
    auto Interactor = Event.Interactor.GetObject();
    if (MemberInfo == nullptr || !IsValid(Interactor))
    {
      continue;
    }

    const auto State = MemberInfo->CalculateState(Event);

    const bool bNewIsSelect = Event.Args.NewState == EIsdkInteractorState::Select;
    if (bNewIsSelect && State.bIsSelectStateBlocking)
    {
      if (!BlockingSelectors.Contains(Interactor))
      {
        BlockingSelectors.Add(Interactor);
      }
    }
    else
    {
      BlockingSelectors.Remove(Interactor);
    }

    const bool bNewIsHover = Event.Args.NewState == EIsdkInteractorState::Hover;
    const bool bPreviousIsHover = Event.Args.PreviousState == EIsdkInteractorState::Hover;
    if (MemberInfo->Behavior.bIsNearField && (bNewIsHover || bPreviousIsHover))
    {
      if (bNewIsHover)
      {
        NearFieldHovers.Add(Interactor);
      }
      else if (bPreviousIsHover)
      {
        NearFieldHovers.Remove(Interactor);
      }
    }
  }

  // Clear queued events before triggering any changes to interactors
  StateChangedEvents.Empty();

  UpdateConditionals();
}

void UIsdkInteractionGroupRigComponent::Deactivate()
{
  Super::Deactivate();

  // This component is de-activated, so its functionality should be disabled.
  // Setting all conditionals to 'True' removes any effects this class will have.
  for (const auto& InteractorMemberInfo : InteractorMemberInfoMap)
  {
    InteractorMemberInfo.Value.IsEnabledConditional->SetValue(true);
  }
}

void UIsdkInteractionGroupRigComponent::UpdateConditionals()
{
  if (!IsActive())
  {
    return;
  }

  // Update the conditionals on each tracked interactor to match our desired state.
  for (const auto& InteractorMemberInfo : InteractorMemberInfoMap)
  {
    const auto& Behavior = InteractorMemberInfo.Value.Behavior;
    bool bShouldBeEnabled = true;

    if (Behavior.bDisableOnOtherSelect && !BlockingSelectors.IsEmpty())
    {
      if (!BlockingSelectors.Contains(InteractorMemberInfo.Key))
      {
        // Something is selected and it was not us.
        bShouldBeEnabled = false;
      }
    }

    if (Behavior.bDisableOnOtherNearFieldHover && !NearFieldHovers.IsEmpty())
    {
      if (!NearFieldHovers.Contains(InteractorMemberInfo.Key))
      {
        // Something else is hovered in the near-field, and it was not us.
        bShouldBeEnabled = false;
      }
    }

    InteractorMemberInfo.Value.IsEnabledConditional->SetValue(bShouldBeEnabled);
  }
}

FIsdkInteractorGroupMember* UIsdkInteractionGroupRigComponent::GetMemberInfoFromEvent(
    FIsdkInteractorStateEvent Event)
{
  if (!IsValid(Event.Interactor.GetObject()))
  {
    return nullptr;
  }

  return InteractorMemberInfoMap.Find(Event.Interactor.GetObject());
}
