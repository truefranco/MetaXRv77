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

#include "Rig/IsdkRigModifier.h"
#include "Core/IsdkIGameplayTagContainer.h"
#include "Interaction/IsdkIInteractorState.h"

void UIsdkRigModifier::InitializeRigModifier_Implementation(UIsdkRigComponent* CallingRigComponent)
{
  ParentRigComponent = CallingRigComponent;
}

bool UIsdkRigModifier::ShouldModifierTrigger(
    FIsdkInteractorStateEvent StateEvent,
    UObject* InteractableReference)
{
  bool bStateTrigger = (InteractorStateTriggers.Num() == 0);
  bool bStateChangeTrigger = (InteractorStateChangeTriggers.Num() == 0);
  bool bInteractableGPTTrigger = RequiredInteractableGameplayTags.IsEmpty();
  bool bInteractorGPTTrigger = RequiredInteractorGameplayTags.IsEmpty();

  // If we have no requirements, it should always trigger
  if (bStateTrigger == true && bStateChangeTrigger == true && bInteractorGPTTrigger == true &&
      bInteractableGPTTrigger == true)
  {
    return true;
  }

  // Check state triggers
  if (!bStateTrigger && InteractorStateTriggers.Contains(StateEvent.Args.NewState))
  {
    bStateTrigger = true;
  }

  if (!bStateChangeTrigger)
  {
    // If any of these state change triggers match the event, we should trigger
    for (const FIsdkInteractorStateChangeArgs& ThisStateChange : InteractorStateChangeTriggers)
    {
      if (ThisStateChange.PreviousState == StateEvent.Args.PreviousState &&
          ThisStateChange.NewState == StateEvent.Args.NewState)
      {
        bStateChangeTrigger = true;
        break;
      }
    }
  }

  // Check if the Interactor has one of the required gameplay tags
  auto* Interactor = StateEvent.Interactor.GetObject();
  if (!bInteractorGPTTrigger && IsValid(Interactor))
  {
    if (Interactor->Implements<UIsdkIGameplayTagContainer>())
    {
      FGameplayTagContainer FoundGPTContainer;
      IIsdkIGameplayTagContainer::Execute_GetGameplayTagContainer(Interactor, FoundGPTContainer);
      if (FoundGPTContainer.HasAny(RequiredInteractorGameplayTags))
      {
        bInteractorGPTTrigger = true;
      }
    }
  }

  // Check if the Interactable has one of our required gameplay tags
  if (!bInteractableGPTTrigger && IsValid(InteractableReference))
  {
    if (InteractableReference->Implements<UIsdkIGameplayTagContainer>())
    {
      FGameplayTagContainer FoundGPTContainer;
      IIsdkIGameplayTagContainer::Execute_GetGameplayTagContainer(
          InteractableReference, FoundGPTContainer);
      if (FoundGPTContainer.HasAny(RequiredInteractableGameplayTags))
      {
        bInteractableGPTTrigger = true;
      }
    }
  }

  return (bStateTrigger && bStateChangeTrigger && bInteractableGPTTrigger && bInteractorGPTTrigger);
}
