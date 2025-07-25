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

#include "Widget/IsdkPointableWidget.h"

#include "Subsystem/IsdkWidgetSubsystem.h"

UIsdkPointableWidget::UIsdkPointableWidget()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UIsdkPointableWidget::HandleInteractionPointerEvent(
    const FIsdkInteractionPointerEvent& PointerEvent)
{
  UIsdkWidgetSubsystem& WidgetSubsystem = UIsdkWidgetSubsystem::Get(GetWorld());
  const auto& [VirtualUserIndex, PointerIndex] =
      WidgetSubsystem.GetVirtualUserInfo(PointerEvent.Interactor);

  const FIsdkVirtualUserPointerEvent VirtualUserPointerEvent{
      PointerEvent.Interactor,
      PointerEvent.Type,
      FVector(PointerEvent.Pose.Position),
      VirtualUserIndex,
      PointerIndex};

  UIsdkWidget::HandleVirtualUserPointerEvent(
      VirtualUserPointerEvent,
      AttachedWidget,
      WidgetEventDelegate,
      WidgetSubsystem,
      MinMoveTravelDistance,
      bStopBroadcastOnDrag);
}

void UIsdkPointableWidget::Setup(
    UWidgetComponent* Widget,
    TScriptInterface<IIsdkIPointable> IPointable)
{
  AttachedWidget = Widget;
  IPointable->GetInteractionPointerEventDelegate().AddUniqueDynamic(
      this, &UIsdkPointableWidget::HandleInteractionPointerEvent);
}
