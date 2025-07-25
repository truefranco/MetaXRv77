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
#include "GameplayTagContainer.h"
#include "IsdkIGameplayTagContainer.generated.h"

UINTERFACE()
class OCULUSINTERACTION_API UIsdkIGameplayTagContainer : public UInterface
{
  GENERATED_BODY()
};

/* Interface that focuses on calls for interacting with gameplay tag containers across different
 * classes (typically Interactors and Interactables) */
class OCULUSINTERACTION_API IIsdkIGameplayTagContainer
{
  GENERATED_BODY()

 public:
  /* Implementing class will replace the given container with its GameplayTagContainer for use in
   * interactions
   */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = InteractionSDK)
  bool GetGameplayTagContainer(FGameplayTagContainer& ContainerOut);

  virtual bool GetGameplayTagContainer_Implementation(FGameplayTagContainer& ContainerOut)
      PURE_VIRTUAL(IIsdkIGameplayTagContainer::GetGameplayTagContainer, return {};);
};
