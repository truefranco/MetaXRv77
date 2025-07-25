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
#include "IsdkConditional.h"
#include "UObject/Object.h"
#include "Delegates/IDelegateInstance.h"
#include "IsdkConditionalGroup.generated.h"

/**
 * @class UIsdkConditionalGroup
 * @brief Collection of IsdkConditionals that are resolved as a group (abstract base class)
 * bool.
 *
 * Stores IsdkConditionals as a TMap of Conditionals and delegates, provides multiple methods for
 * querying and updating the state of conditionals
 *
 * @see UIsdkConditional
 * @see UIsdkConditionalGroupAll
 * @see UIsdkConditionalGroupAny
 * @see UIsdkConditionalGroupNone
 * @see UIsdkConditionalGroupSingle
 * @addtogroup InteractionSDK
 */
UCLASS(Abstract, NotBlueprintable, Category = InteractionSDK)
class OCULUSINTERACTION_API UIsdkConditionalGroup : public UIsdkConditional
{
  GENERATED_BODY()
 public:
  /**
   * @brief Returns the conditionals collectively resolved in this group
   * @param Conditionals Reference to the TArray of UIsdkConditionals retrieved
   */
  UFUNCTION(BlueprintPure, Category = InteractionSDK)
  void GetConditionals(TArray<UIsdkConditional*>& Conditionals) const
  {
    ConditionalDelegateHandles.GetKeys(Conditionals);
  }

  /**
   * @brief Removes a conditional from being resolved in this group
   * @param InConditional Pointer to the UIsdkConditional intended to be removed
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void RemoveConditional(UIsdkConditional* InConditional)
  {
    auto Handle = ConditionalDelegateHandles.FindAndRemoveChecked(InConditional);
    if (Handle != FDelegateHandle{})
    {
      check(InConditional != nullptr);
      InConditional->ResolvedValueChanged.Remove(Handle);

      ReCalculateResolvedValue();
    }
  }
  /**
   * @brief Add a conditional to be resolved in this group
   * @param InConditional Pointer to the UIsdkConditional intended to be added
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void AddConditional(UIsdkConditional* InConditional)
  {
    if (ensure(InConditional) && !ConditionalDelegateHandles.Contains(InConditional))
    {
      auto Handle = InConditional->ResolvedValueChanged.AddWeakLambda(
          this, [this](bool) { ReCalculateResolvedValue(); });
      ConditionalDelegateHandles.Add(InConditional, Handle);

      ReCalculateResolvedValue();
    }
  }
  /**
   * @brief Function to retrieve whether or not the TMap of conditionals is empty
   * @return bool Returns true if there are no UIsdkConditionals present in this object
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  bool IsEmpty()
  {
    return ConditionalDelegateHandles.Num() == 0;
  }

 protected:
  UIsdkConditionalGroup(){};
  /**
   * @brief Iterate through stored Conditionals and return a resolved value
   * @return bool The calculated boolean value of the stored Conditionals
   */
  virtual bool CalculateValueFromConditionals() const
      PURE_VIRTUAL(UIsdkConditionalGroup::CalculateValueFromConditionals, return false;);
  /**
   * @brief Return the default value for this conditional if none are present
   * @return bool The default boolean value for this ConditionalGroup
   */
  virtual bool CalculateValueFromEmpty() const
      PURE_VIRTUAL(UIsdkConditionalGroup::CalculateValueFromEmpty, return false;);
  /**
   * @brief Method for recalculating the resolved value for this ConditionalGroup
   *
   * Depending on the value of IsEmpty, this method either utilizes CalculateValueFromEmpty or
   * CalculateValueFromConditionals to derive the current boolean value of this ConditionalsGroup
   * and sets that resolve value
   *
   */
  void ReCalculateResolvedValue();

  /**
   * Map of Conditionals and delegates (UIsdkConditional*, FDelegateHandle), set/queried in
   * all Conditional operations.
   */
  TMap<UIsdkConditional*, FDelegateHandle> ConditionalDelegateHandles;
};
