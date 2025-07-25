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
#include "Components/ActorComponent.h"
#include "Interaction/IsdkIInteractorState.h"

#include <functional>

#include "IsdkInteractionGroupRigComponent.generated.h"

class UIsdkConditional;
class UIsdkConditionalBool;

USTRUCT()
struct FIsdkInteractionGroupMemberBehavior
{
  GENERATED_BODY()

  UPROPERTY()
  bool bDisableOnOtherSelect{true};

  UPROPERTY()
  bool bDisableOnOtherNearFieldHover{false};

  UPROPERTY()
  bool bIsNearField{false};
};
USTRUCT()
struct FIsdkInteractionGroupMemberState
{
  GENERATED_BODY()

  // Will be true if the interactor is in Select state for the purposes of disabling other
  // interactors. If this is false, then the 'Select' will be ignored by this group.
  // Commonly, this will return true if the interactor has an interactable; false otherwise.
  UPROPERTY()
  bool bIsSelectStateBlocking{false};
};

USTRUCT()
struct FIsdkInteractorGroupMember
{
  GENERATED_BODY()

  using CalculateStateFn =
      std::function<FIsdkInteractionGroupMemberState(const FIsdkInteractorStateEvent&)>;

  FIsdkInteractionGroupMemberBehavior Behavior;
  CalculateStateFn CalculateState;

  UPROPERTY()
  UIsdkConditionalBool* IsEnabledConditional{};
};

UCLASS(ClassGroup = (InteractionSDK), meta = (BlueprintSpawnableComponent))
class OCULUSINTERACTIONPREBUILTS_API UIsdkInteractionGroupRigComponent : public UActorComponent
{
  GENERATED_BODY()

 public:
  UIsdkInteractionGroupRigComponent();

 protected:
  virtual void Activate(bool bReset = false) override
  {
    Super::Activate(bReset);
    UpdateConditionals();
  }

  virtual void Deactivate() override;

  FIsdkInteractorGroupMember* GetMemberInfoFromEvent(FIsdkInteractorStateEvent Event);
  void UpdateConditionals();

  UFUNCTION()
  void HandleIsdkFrameFinished();

  // Called when the game starts
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

 public:
  UFUNCTION()
  void HandleInteractorStateChanged(const FIsdkInteractorStateEvent& InteractorState);

  /**
   *
   * @param Interactor The object which owns (and invokes) the StateChangedDelegate.
   * @param StateChangedDelegate Delegate that this interactor group will listen to in order to
   * track the state of the given interactor.
   * @param CalculateInteractorState Callback that must provide additional information about the
   * interactor object. This state is used as an input into behavior decisions.
   * @param Behavior Defines how the group will respond when this interactor changes between hover,
   * select, normal etc.
   * @return A conditional that should be used by the caller to handle enable/disable requests from
   * this group. When the conditional becomes 'false', the given interactor should be disabled (and
   * enabled when the conditional becomes true)
   */
  UIsdkConditional* AddInteractor(
      UObject* Interactor,
      FIsdkInteractorStateChanged& StateChangedDelegate,
      FIsdkInteractorGroupMember::CalculateStateFn CalculateInteractorState,
      FIsdkInteractionGroupMemberBehavior Behavior);

  void RemoveInteractor(UObject* Interactor);

  UIsdkConditional* FindConditionalForInteractor(UObject* Interactor);

  const TMap<UObject*, FIsdkInteractorGroupMember>& GetMemberInfoMap()
  {
    return InteractorMemberInfoMap;
  }

 private:
  UPROPERTY()
  TArray<FIsdkInteractorStateEvent> StateChangedEvents;

  UPROPERTY()
  TMap<UObject*, FIsdkInteractorGroupMember> InteractorMemberInfoMap;

  UPROPERTY()
  TSet<UObject*> BlockingSelectors;
  UPROPERTY()
  TSet<UObject*> NearFieldHovers;
};
