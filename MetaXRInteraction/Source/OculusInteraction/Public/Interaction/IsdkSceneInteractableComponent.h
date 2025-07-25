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
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "IsdkIInteractableState.h"

#include "StructTypes.h"
#include "Core/IsdkIGameplayTagContainer.h"

#include "IsdkSceneInteractableComponent.generated.h"

class UIsdkSceneInteractorComponent;

UDELEGATE(BlueprintType, Blueprintable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FSceneInteractableInteractorCountChanged,
    UIsdkSceneInteractableComponent*,
    Sender,
    int,
    NewCount);

UDELEGATE(BlueprintType, Blueprintable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FSceneInteractableEnabledStateChanged,
    UIsdkSceneInteractableComponent*,
    Sender,
    bool,
    NewState);

/**
 * @class UIsdkSceneInteractableComponent
 * @brief Base class for interactables not tracked by the API
 *
 * Scene component used as base class for interactable components not being tracked by the API.
 * Provides functions and methods for getting current state, delegates, querying relationships and
 * GameplayTag containers. Relationships are established and tracked between
 * UIsdkSceneInteractableComponents and UIsdkSceneInteractorComponents by this component.
 *
 * @see IIsdkIGameplayTagContainer
 * @see IIsdkIInteractableState
 * @see UIsdkSceneInteractorComponent
 * @addtogroup InteractionSDK
 */
UCLASS(
    Blueprintable,
    ClassGroup = (InteractionSDK),
    meta = (BlueprintSpawnableComponent, DisplayName = "ISDK Scene Interactable"))
class OCULUSINTERACTION_API UIsdkSceneInteractableComponent : public USceneComponent,
                                                              public IIsdkIGameplayTagContainer,
                                                              public IIsdkIInteractableState
{
  GENERATED_BODY()
 public:
  UIsdkSceneInteractableComponent() = default;

  /**
   * @brief Returns the current state of this interactable as an enum, implemented from interface,
   * calls GetCurrentState()
   * @see IIsdkIInteractableState
   * @return EIsdkInteractableState Current state as an enum (Normal, Hover, Select or Disabled)
   */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InteractionSDK|Scene Interactable")
  virtual EIsdkInteractableState GetInteractableState() const override
  {
    return State;
  }

  /**
   * @brief Returns the delegate used by this interactable for broadcasting state changes.
   * Implemented from interface
   * @see IIsdkIInteractableState
   * @return FIsdkInteractableStateChanged* Delegate used for broadcasting state changes
   */
  virtual FIsdkInteractableStateChanged* GetInteractableStateChangedDelegate() override
  {
    return &StateChangedDelegate;
  }

  /**
   * @brief Returns a struct representing the number of interactors hovering and selecting this
   * interactable. Implemented from interface
   * @see IIsdkIInteractableState
   * @return FIsdkInteractionRelationshipCounts Struct containing the number of interactors hovering
   * and/or selecting this interactable
   */
  virtual FIsdkInteractionRelationshipCounts GetInteractableStateRelationshipCounts() const override
  {
    return RelationshipCounts;
  }

  /**
   * @brief Enables this interactable component, broadcasts EnabledStateChangedDelegate
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  virtual void Enable()
  {
    if (!Enabled)
    {
      Enabled = true;
      EnabledStateChangedDelegate.Broadcast(this, Enabled);
    }
  }

  /**
   * @brief Disables this interactable component, broadcasts EnabledStateChangedDelegate
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  virtual void Disable()
  {
    if (Enabled)
    {
      Enabled = false;
      EnabledStateChangedDelegate.Broadcast(this, Enabled);
    }
  }

  /**
   * @brief Returns whether or not this interactable component is currently enabled
   * @return bool If this scene interactable component is enabled
   */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = InteractionSDK)
  virtual bool IsEnabled()
  {
    return Enabled;
  }

  /**
   * @brief Begins tracking a relationship between this Scene Interactable and a given Scene
   * Interactor
   * @param Interactor UIsdkSceneInteractorComponent being added to this component's tracked
   * relationships
   */
  virtual void AddInteractor(UIsdkSceneInteractorComponent* Interactor)
  {
    Interactors.Add(Interactor);
    InteractorCountChangedDelegate.Broadcast(this, Interactors.Num());
  }

  /**
   * @brief Removes tracking a relationship between this Scene Interactable and a given Scene
   * Interactor
   * @param Interactor UIsdkSceneInteractorComponent being removed from this component's tracked
   * relationships
   */
  virtual void RemoveInteractor(UIsdkSceneInteractorComponent* Interactor)
  {
    Interactors.Remove(Interactor);
    InteractorCountChangedDelegate.Broadcast(this, Interactors.Num());
  }

  /* Delegate broadcast when the state of this interactable changes */
  UPROPERTY(BlueprintAssignable, Category = InteractionSDK)
  FIsdkInteractableStateChanged StateChangedDelegate;

  /* Delegate broadcast when the number of interactors engaged with this interactable changes */
  UPROPERTY(BlueprintAssignable, Category = InteractionSDK)
  FSceneInteractableInteractorCountChanged InteractorCountChangedDelegate;

  /* Delegate broadcast when this interactable becomes enabled or disabled */
  UPROPERTY(BlueprintAssignable, Category = InteractionSDK)
  FSceneInteractableEnabledStateChanged EnabledStateChangedDelegate;

  // GameplayTags used for labeling and driving interaction behavior. Returned by
  // IIsdkIGameplayTagContainer::GetGameplayTagContainer().
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractionSDK)
  FGameplayTagContainer InteractableTags;

  /**
   * @brief Retrieves the current GameplayTag container for this component. Implemented from
   * interface
   * @see IIsdkIGameplayTagContainer
   * @param ContainerOut GameplayTag Container returned as reference.
   * @return bool Whether or not the returned GameplayTag container is valid and/or contains tags.
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  virtual bool GetGameplayTagContainer_Implementation(FGameplayTagContainer& ContainerOut) override
  {
    ContainerOut = InteractableTags;
    return !ContainerOut.IsEmpty();
  };

 protected:
  /* The current interaction state of this interactable, driven by interactor events */
  UPROPERTY(BlueprintReadOnly, BlueprintGetter = GetInteractableState, Category = InteractionSDK)
  EIsdkInteractableState State{};

  UPROPERTY(BlueprintReadOnly, BlueprintGetter = IsEnabled, Category = InteractionSDK)
  bool Enabled{};

  /**
   * @brief Virtual function that establishes if this interactable can change to a given state.
   * Returns if this component can change to the given state. Logic to determine this is intended to
   * be implemented by child classes, base class always returns true
   * @param NewState New interactable state to check
   * @return bool Whether or not this interactable can change to the given state
   */
  virtual bool CanChangeStateTo(EIsdkInteractableState NewState)
  {
    return true;
  }

  /**
   * @brief Virtual function that changes the state of this interactable to a new given state, given
   * it is allowed to by CanChangeStateTo(). Utilized by SetState() to change the state of this
   * interactable, one state at a time.
   * @param NewState New interactable state to set this interactable to
   * @return bool Whether or not the change of state was successful
   */
  virtual bool SetStateStep(EIsdkInteractableState NewState)
  {
    if (CanChangeStateTo(NewState))
    {
      if (NewState != State)
      {
        switch (State)
        {
          case EIsdkInteractableState::Normal:
            break;
          case EIsdkInteractableState::Hover:
            if (NewState == EIsdkInteractableState::Normal)
            {
              RelationshipCounts.NumHover--;
              if (RelationshipCounts.NumHover > 0)
              {
                return false;
              }
            }
            break;
          case EIsdkInteractableState::Select:
            RelationshipCounts.NumSelect--;
            if (RelationshipCounts.NumSelect > 0)
            {
              return false;
            }
            break;
          case EIsdkInteractableState::Disabled:
            break;
        }

        switch (NewState)
        {
          case EIsdkInteractableState::Normal:
            break;
          case EIsdkInteractableState::Hover:
            if (State == EIsdkInteractableState::Normal)
            {
              RelationshipCounts.NumHover++;
            }
            break;
          case EIsdkInteractableState::Select:
            if (State == EIsdkInteractableState::Hover)
            {
              RelationshipCounts.NumSelect++;
            }
            break;
          case EIsdkInteractableState::Disabled:
            break;
        }

        const auto Args = FIsdkInteractableStateEvent::Create(this, State, NewState);
        State = NewState;
        StateChangedDelegate.Broadcast(Args);
      }
    }

    return true;
  }

  virtual void SetState(EIsdkInteractableState NewState)
  {
    if (NewState == State)
    {
      return;
    }

    if (NewState > State)
    {
      for (int iState = static_cast<int>(State) + 1; iState <= static_cast<int>(NewState); iState++)
      {
        if (!SetStateStep(static_cast<EIsdkInteractableState>(iState)))
          break;
      }
    }
    else
    {
      for (int iState = static_cast<int>(State) - 1; iState >= static_cast<int>(NewState); iState--)
      {
        if (!SetStateStep(static_cast<EIsdkInteractableState>(iState)))
          break;
      }
    }
  }

  TArray<UIsdkSceneInteractorComponent*> Interactors{};
  FIsdkInteractionRelationshipCounts RelationshipCounts{};
};
