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
#include "UObject/Object.h"
#include "IsdkConditionalGroup.h"
#include "IsdkConditionalGroupNone.generated.h"

/**
 * @class UIsdkConditionalGroupNone
 * @brief Conditional Group that resolves true if NO contained conditionals are true
 *
 * @see UIsdkConditional
 * @see UIsdkConditionalGroupAll
 * @see UIsdkConditionalGroupAny
 * @see UIsdkConditionalGroupNone
 * @see UIsdkConditionalGroupSingle
 * @addtogroup InteractionSDK
 */
UCLASS(Blueprintable, DefaultToInstanced, Category = InteractionSDK)
class OCULUSINTERACTION_API UIsdkConditionalGroupNone : public UIsdkConditionalGroup
{
  GENERATED_BODY()
 public:
  UIsdkConditionalGroupNone()
  {
    SetResolvedValue(CalculateValueFromEmpty());
  };
  /**
   * @brief Iterate through stored Conditionals and return true if no stored conditional are true
   * @return bool Whether or not none of the stored Conditional are currently true
   */
  bool CalculateValueFromConditionals() const override
  {
    bool bNewValue = true;
    for (const auto& Conditional : ConditionalDelegateHandles)
    {
      bNewValue &= !Conditional.Key->GetResolvedValue();
    }
    return bNewValue;
  }
  /**
   * @brief Return the default value for this conditional if none are present
   * @return bool The default boolean value for this type of ConditionalGroup (true)
   */
  bool CalculateValueFromEmpty() const override
  {
    return true;
  }
};
