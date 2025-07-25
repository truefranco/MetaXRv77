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
#include "IsdkConditional.h"
#include "IsdkConditionalComponentIsActive.generated.h"

/**
 * @class UIsdkConditionalComponentIsActive
 * @brief Watches an ActorComponents IsActive() flag, setting the state of this conditional to the
 * value of that flag.
 *
 * After an ActorComponent is set on this Conditional, it will change the resolved value of its
 * stored conditional to whether or not that component is active, keyed from the events generated
 * when the component is activated/deactivated
 *
 * @see UIsdkConditional
 * @addtogroup InteractionSDK
 */
UCLASS(Blueprintable, DefaultToInstanced, Category = InteractionSDK)
class OCULUSINTERACTION_API UIsdkConditionalComponentIsActive : public UIsdkConditional
{
  GENERATED_BODY()

 public:
  UIsdkConditionalComponentIsActive();

  /**
   * @brief Returns the Actor Component the active state of which this conditional is watching
   * @return UActorComponent* A pointer to the UActorComponent that was set to this conditional.
   * This pointer is not validated.
   */
  UFUNCTION(BlueprintPure, Category = InteractionSDK)
  UActorComponent* GetComponent() const
  {
    return ActorComponent;
  }

  /**
   * @brief Sets the Actor Component the active state of which this conditional should watch
   * @param InActorComponent The new UActorComponent to watch for activation/deactivation events
   *
   * This method will remove any previous delegate hooks set on the previous ActorComponent (if
   * present) before adding hooks for the new one. Delegate is hooked onto
   * ActorComponent->OnComponentActivated and ActorComponent->OnComponentDeactivated, which will
   * call HandleComponentActivated and HandleComponentDeactivated respectively.
   *
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void SetComponent(UActorComponent* InActorComponent);

 private:
  UPROPERTY(
      BlueprintGetter = GetComponent,
      BlueprintSetter = SetComponent,
      Category = InteractionSDK)
  UActorComponent* ActorComponent{};

  UFUNCTION()
  void HandleComponentActivated(UActorComponent* Component, bool bReset)
  {
    CalculateResolvedValue();
  }
  UFUNCTION()
  void HandleComponentDeactivated(UActorComponent* Component)
  {
    CalculateResolvedValue();
  }

  void CalculateResolvedValue();
};
