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
#include "IsdkConditionalGroupSingle.generated.h"

/**
 * @class UIsdkConditionalGroupSingle
 * @brief Conditional Group that resolves true if ONE and only one conditional is true
 *
 * @see UIsdkConditional
 * @see UIsdkConditionalGroupAll
 * @see UIsdkConditionalGroupAny
 * @see UIsdkConditionalGroupNone
 * @see UIsdkConditionalGroupSingle
 * @addtogroup InteractionSDK
 */
UCLASS(Blueprintable, DefaultToInstanced, Category = InteractionSDK)
class OCULUSINTERACTION_API UIsdkConditionalGroupSingle : public UIsdkConditionalGroup
{
  GENERATED_BODY()
 public:
  UIsdkConditionalGroupSingle()
  {
    SetResolvedValue(CalculateValueFromEmpty());
  };
  /**
   * @brief Iterate through stored Conditionals and return true if ONLY ONE stored conditional is
   * true
   * @return bool Whether or at least ONLY ONE stored Conditional is currently true
   */
  bool CalculateValueFromConditionals() const override
  {
    int CountTrue = 0;
    for (const auto& Conditional : ConditionalDelegateHandles)
    {
      if (Conditional.Key->GetResolvedValue())
      {
        ++CountTrue;
      }
    }
    return CountTrue == 1;
  }
  /**
   * @brief Return the default value for this conditional if none are present
   * @return bool The default boolean value for this type of ConditionalGroup (false)
   */
  bool CalculateValueFromEmpty() const override
  {
    return false;
  }
};
