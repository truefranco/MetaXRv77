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

#include "StructTypes.h"
#include "Interaction/IsdkIInteractorState.h"
#include "Core/IsdkIGameplayTagContainer.h"

#include "IsdkSceneInteractorComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogISDK, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FSceneInteractorEnabledStateChanged,
    UIsdkSceneInteractorComponent*,
    Sender,
    bool,
    NewState);

/**
 * @class UIsdkSceneInteractorComponent
 * @brief Base class for interactors not tracked by the API
 *
 * Scene component used as base class for interactor components not being tracked by the API.
 * Provides functions and methods for getting current state, delegates, querying GameplayTag
 * containers.
 *
 * @see IIsdkIGameplayTagContainer
 * @see IIsdkIInteractorState
 * @see UIsdkSceneInteractableComponent
 * @addtogroup InteractionSDK
 */
UCLASS(
    Blueprintable,
    ClassGroup = (InteractionSDK),
    meta = (BlueprintSpawnableComponent, DisplayName = "ISDK Scene Interactor"))
class OCULUSINTERACTION_API UIsdkSceneInteractorComponent : public USceneComponent,
                                                            public IIsdkIGameplayTagContainer,
                                                            public IIsdkIInteractorState
{
  GENERATED_BODY()
 public:
  UIsdkSceneInteractorComponent()
  {
    ID = NextID++;
  }

  /**
   * @brief Returns the current state of this interactor, driven by events, implemented from
   * interface
   * @see IIsdkIInteractorState
   * @return EIsdkInteractorState Current state as an enum (Normal, Hover, Select or Disabled)
   */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InteractionSDK|Scene Interactor")
  virtual EIsdkInteractorState GetInteractorState() const override
  {
    return State;
  }

  /**
   * @brief Returns the delegate used by this interactor for broadcasting state changes.
   * Implemented from interface
   * @see IIsdkIInteractorState
   * @return FIsdkInteractorStateChanged* Delegate used for broadcasting state changes
   */
  virtual FIsdkInteractorStateChanged* GetInteractorStateChangedDelegate() override
  {
    return &StateChangedDelegate;
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
   * @brief Returns whether or not this interactor component is currently enabled
   * @return bool If this scene interactor component is enabled
   */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = InteractionSDK)
  virtual bool IsEnabled()
  {
    return Enabled;
  }

  /* Delegate broadcast when the state of this interactor changes */
  UPROPERTY(BlueprintAssignable, Category = InteractionSDK)
  FIsdkInteractorStateChanged StateChangedDelegate;

  /* Delegate broadcast when this interactor is enabled or disabled */
  UPROPERTY(BlueprintAssignable, Category = InteractionSDK)
  FSceneInteractorEnabledStateChanged EnabledStateChangedDelegate;

  int GetID() const
  {
    return ID;
  }

  virtual bool HasCandidate() const
      PURE_VIRTUAL(UIsdkSceneInteractorComponent::HasCandidate, return false;);
  virtual bool HasInteractable() const
      PURE_VIRTUAL(UIsdkSceneInteractorComponent::HasInteractable, return false;);

  // GameplayTags used for labeling and driving interaction behavior. Returned by
  // IIsdkIGameplayTagContainer::GetGameplayTagContainer().
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractionSDK)
  FGameplayTagContainer InteractorTags;

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
    ContainerOut = InteractorTags;
    return !ContainerOut.IsEmpty();
  }

 protected:
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  int ID;

  /* The current interaction state of this interactable, driven by user events */
  UPROPERTY(BlueprintReadOnly, BlueprintGetter = GetInteractorState, Category = InteractionSDK)
  EIsdkInteractorState State = EIsdkInteractorState::Normal;

  UPROPERTY(BlueprintReadOnly, BlueprintGetter = IsEnabled, Category = InteractionSDK)
  bool Enabled{false};

  static int NextID;

  /**
   * @brief Virtual function that changes the state of this interactable to a new given state
   * @param NewState New interactor state to set this interactor to
   */
  virtual void SetState(EIsdkInteractorState NewState)
  {
    if (NewState != State)
    {
      FIsdkInteractorStateEvent Event{this, {}};
      Event.Args.PreviousState = State;
      Event.Args.NewState = NewState;

      State = NewState;
      StateChangedDelegate.Broadcast(Event);
    }
  }
};
