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
#include "StructTypes.h"
#include "IsdkInteractionPointerEvent.generated.h"

class UIsdkInteractableComponent;
class IIsdkIPointable;

// Forward declarations of internal types
struct isdk_PointerEvent_;
typedef isdk_PointerEvent_ isdk_PointerEvent;

USTRUCT(BlueprintType)
struct OCULUSINTERACTION_API FIsdkInteractionPointerEvent
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  int Identifier{};

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  EIsdkPointerEventType Type{};

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  FIsdkPosef Pose{};

  /**
   * The interactor that invoked this event. May be null if the interactor was deleted prior
   * to this event being emitted (a common case is when the interactor deletes itself in response
   * to a Select event - the following Hover/Normal/Disabled events will still be emitted.)
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  USceneComponent* Interactor{};

  /**
   * The interactable that received this event. May be null if the interactable was deleted prior
   * to this event being emitted (a common case is when the interactable is deleted in response
   * to a Select event - the following Hover/Normal/Disabled events will still be emitted.)
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  TScriptInterface<IIsdkIPointable> Interactable{};

  static FIsdkInteractionPointerEvent CreateFromPointerEvent(
      const isdk_PointerEvent& Src,
      UIsdkInteractableComponent* SrcInteractable);

  static FIsdkInteractionPointerEvent CreateCancelEvent(
      int Identifier,
      USceneComponent* Interactor,
      TScriptInterface<IIsdkIPointable> Interactable);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FIsdkInteractionPointerEventDelegate,
    const FIsdkInteractionPointerEvent&,
    PointerEvent);
