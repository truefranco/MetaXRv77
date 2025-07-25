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
#include "UObject/Interface.h"
#include "StructTypes.h"
#include "IsdkIInteractorState.generated.h"

class IIsdkIInteractorState;
class IIsdkIInteractableState;

/**
 * @struct FIsdkInteractorStateEvent
 * @brief A struct representing a new event generated from changing the state of this interactor
 */
USTRUCT(Category = InteractionSDK, BlueprintType)
struct OCULUSINTERACTION_API FIsdkInteractorStateEvent
{
  GENERATED_BODY()

  /**
   * The interactor that invoked this event. May be null if the interactor was deleted prior
   * to this event being emitted (a common case is when the interactor deletes itself in response
   * to a Select event - the following Hover/Normal/Disabled events will still be emitted.)
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  TScriptInterface<IIsdkIInteractorState> Interactor{};

  /**
   * A struct that contains the previous EIsdkInteractorState as well as the new one established
   * by this event
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  FIsdkInteractorStateChangeArgs Args;

  static FIsdkInteractorStateEvent
  Create(UObject* Interactor, EIsdkInteractorState PreviousState, EIsdkInteractorState NewState)
  {
    FIsdkInteractorStateChangeArgs Args;
    Args.PreviousState = PreviousState;
    Args.NewState = NewState;
    return FIsdkInteractorStateEvent{Interactor, Args};
  }
};

UDELEGATE(BlueprintType, Blueprintable, meta = (DisplayName = "Interactor State Changed Delegate"))
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FIsdkInteractorStateChanged,
    const FIsdkInteractorStateEvent&,
    InteractorState);

/**
 * @class UIsdkIInteractorState
 * @brief Base interface class for IIsdkIInteractorState, does not need to be modified.
 * @see IIsdkIInteractorState
 * @addtogroup InteractionSDK
 */
UINTERFACE()
class OCULUSINTERACTION_API UIsdkIInteractorState : public UInterface
{
  GENERATED_BODY()
};

/**
 * @class IIsdkIInteractorState
 * @brief Interface implemented by all interactor classes
 * Interface that provides broad functions and methods for getting current state, relationships with
 * interactables, as well as reliable references to delegates
 *
 * @addtogroup InteractionSDK
 */
class OCULUSINTERACTION_API IIsdkIInteractorState
{
  GENERATED_BODY()

 public:
  /**
   * @brief Return whether or not this interactor has a selected interactable
   * @return bool Whether or not this interactor has a relationship with an interactable where both
   * are set to Select, and is considered "selected" by this interactor.
   */

  virtual bool HasSelectedInteractable() const
      PURE_VIRTUAL(IIsdkIStatefulInteractor::HasSelectedInteractable, return false;);

  /**
   * @brief Retrieve the current state of this interactor
   * The current state of this interactable.
   * If there is no interactable relationship, this will be either Disabled or Normal.
   * If there is a single interactable relationship, this will be either Hover or Select.
   * If there are multiple interactable relationships, this will be Select if at least one of those
   * is a 'Select' relationship; otherwise Hover.
   * @return EIsdkInteractableState The enum representing the current state
   */
  virtual EIsdkInteractorState GetInteractorState() const
      PURE_VIRTUAL(IIsdkIStatefulInteractor::GetInteractorState, return {};);

  /**
   * Returns a structure summarizing the number of Hover and Select interactor relationships exist
   * on this interactor.
   */
  virtual FIsdkInteractionRelationshipCounts GetInteractorStateRelationshipCounts() const
      PURE_VIRTUAL(IIsdkIStatefulInteractor::GetInteractorStateRelationshipCounts, return {};);

  /**
   * @brief Queries for all interactables that match the given state in relationship to this
   * interactor component.
   * @param State for which to check relationships (Normal, Hover, Select or Disabled)
   * @param OutInteractables Array of objects that implement IIsdkIInteractableState, passed by
   * reference, representing Interactables that meet the criteria of this query
   */
  virtual void GetInteractorStateRelationships(
      EIsdkInteractableState State,
      TArray<TScriptInterface<IIsdkIInteractableState>>& OutInteractables) const
      PURE_VIRTUAL(IIsdkIStatefulInteractor::GetInteractorStateRelationships);

  /**
   * @brief Returns the delegate used by this interactor for broadcasting state changes.
   * @return FIsdkInteractorStateChanged* Delegate used for broadcasting state changes
   */
  virtual FIsdkInteractorStateChanged* GetInteractorStateChangedDelegate()
      PURE_VIRTUAL(IIsdkIStatefulInteractor::GetInteractorStateChangedDelegate, return nullptr;);
};
