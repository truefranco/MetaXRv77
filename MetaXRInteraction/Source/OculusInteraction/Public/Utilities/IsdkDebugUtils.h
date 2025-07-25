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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IsdkDebugUtils.generated.h"

enum class EIsdkInteractableState : uint8;
enum class EIsdkInteractorState : uint8;
enum class EIsdkPointerEventType : uint8;

/**
 * Miscellaneous functions helpful for debugging.  Tools here may be specific
 * to the ISDK library, or for general purpose use.
 */
UCLASS()
class OCULUSINTERACTION_API UIsdkDebugUtils : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

 public:
  static FColor GetPointerEventDebugColor(EIsdkPointerEventType PointerEventType);
  static FColor GetInteractorStateDebugColor(EIsdkInteractorState InteractorState);
  static FColor GetInteractableStateDebugColor(EIsdkInteractableState InteractableState);
};
