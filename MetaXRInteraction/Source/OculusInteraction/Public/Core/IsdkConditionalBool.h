﻿/*
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
#include "Core/IsdkConditional.h"
#include "IsdkConditionalBool.generated.h"

/**
 * @class UIsdkConditionalBool
 * @brief A basic implementation of UIsdkConditional, which allows the value to be set from a simple
 * bool.
 *
 * @see UIsdkConditional
 * @addtogroup InteractionSDK
 */
UCLASS(Blueprintable, Category = InteractionSDK)
class OCULUSINTERACTION_API UIsdkConditionalBool : public UIsdkConditional
{
  GENERATED_BODY()

 public:
  /**
   * @brief Set the conditional to the given value, and then broadcast the change
   * @param bNewValue The new boolean value to set
   * @see UIsdkConditional#SetResolvedValue
   */
  void SetValue(bool bNewValue)
  {
    SetResolvedValue(bNewValue);
  }
};
