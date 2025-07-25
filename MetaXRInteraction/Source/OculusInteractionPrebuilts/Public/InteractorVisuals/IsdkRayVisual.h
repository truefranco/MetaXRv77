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
#include "GameFramework/Actor.h"
#include "IsdkRayVisualComponent.h"
#include "IsdkRayVisual.generated.h"

/**
 * @class AIsdkRayVisual
 * @brief Actor used for visualizing the ray cursor and pinch arrow
 *
 * Spawns a Ray Visual Component as a child component, which maintains the cursor and pinch arrow
 * subcomponents for visualizing the state of ray interactions
 * @see UIsdkRayVisualComponent
 * @addtogroup InteractionSDKPrebuilts
 */
UCLASS()
class OCULUSINTERACTIONPREBUILTS_API AIsdkRayVisual : public AActor
{
  GENERATED_BODY()

 public:
  AIsdkRayVisual();

  /**
   * @brief The ray visual component created by this Actor when constructed
   */
  UPROPERTY(Category = InteractionSDK, VisibleAnywhere, BlueprintReadOnly)
  UIsdkRayVisualComponent* RayVisualComponent;
};
