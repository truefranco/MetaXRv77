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

#include "Interaction/Pointable/IsdkInteractionPointerEvent.h"

#include "StructTypesPrivate.h"
#include "isdk_api/isdk_api.hpp"

#include "Interaction/IsdkInteractableComponent.h"
#include "Interaction/Pointable/IsdkIPointable.h"

FIsdkInteractionPointerEvent FIsdkInteractionPointerEvent::CreateFromPointerEvent(
    const isdk_PointerEvent& Src,
    UIsdkInteractableComponent* SrcInteractable)
{
  FIsdkInteractionPointerEvent Dest{};
  Dest.Identifier = Src.identifier;
  Dest.Type = static_cast<EIsdkPointerEventType>(Src.type);
  StructTypesUtils::Copy(Src.pose, Dest.Pose);
  Dest.Interactor = nullptr;
  Dest.Interactable = SrcInteractable;

  return Dest;
}

FIsdkInteractionPointerEvent FIsdkInteractionPointerEvent::CreateCancelEvent(
    int Identifier,
    USceneComponent* Interactor,
    TScriptInterface<IIsdkIPointable> Interactable)
{
  auto Event = FIsdkInteractionPointerEvent();
  Event.Identifier = Identifier;
  Event.Type = EIsdkPointerEventType::Cancel;
  Event.Interactor = Interactor;
  Event.Interactable = Interactable;
  Event.Pose = FIsdkPosef();

  return Event;
}
