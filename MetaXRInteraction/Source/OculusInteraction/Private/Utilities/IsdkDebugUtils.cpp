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

#include "Utilities/IsdkDebugUtils.h"

#include "IsdkRuntimeSettings.h"
#include "StructTypes.h"

FColor UIsdkDebugUtils::GetPointerEventDebugColor(EIsdkPointerEventType PointerEventType)
{
  const auto RuntimeSettings = GetDefault<UIsdkRuntimeSettings>();

  FColor DebugColor;
  switch (PointerEventType)
  {
    case EIsdkPointerEventType::Hover:
      DebugColor = RuntimeSettings->PointerEventColorHover;
      break;
    case EIsdkPointerEventType::Select:
      DebugColor = RuntimeSettings->PointerEventColorSelect;
      break;
    case EIsdkPointerEventType::Unselect:
      DebugColor = RuntimeSettings->PointerEventColorUnselect;
      break;
    case EIsdkPointerEventType::Cancel:
      DebugColor = RuntimeSettings->PointerEventColorCancel;
      break;
    case EIsdkPointerEventType::Unhover:
      DebugColor = RuntimeSettings->PointerEventColorUnhover;
      break;
    case EIsdkPointerEventType::Move:
    default:
      // We expect to never hit this path, so use a hard-coded color.
      return FColor::Magenta;
  }

  return DebugColor;
}

FColor UIsdkDebugUtils::GetInteractorStateDebugColor(EIsdkInteractorState InteractorState)
{
  const auto RuntimeSettings = GetDefault<UIsdkRuntimeSettings>();

  FColor DebugColor;
  switch (InteractorState)
  {
    case EIsdkInteractorState::Normal:
      DebugColor = RuntimeSettings->InteractorColorNormal;
      break;
    case EIsdkInteractorState::Hover:
      DebugColor = RuntimeSettings->InteractorColorHover;
      break;
    case EIsdkInteractorState::Select:
      DebugColor = RuntimeSettings->InteractorColorSelect;
      break;
    case EIsdkInteractorState::Disabled:
      DebugColor = RuntimeSettings->InteractorColorDisabled;
      break;
  }

  return DebugColor;
}

FColor UIsdkDebugUtils::GetInteractableStateDebugColor(EIsdkInteractableState InteractableState)
{
  const auto RuntimeSettings = GetDefault<UIsdkRuntimeSettings>();

  FColor DebugColor;
  switch (InteractableState)
  {
    case EIsdkInteractableState::Hover:
      DebugColor = RuntimeSettings->InteractableColorHover;
      break;
    case EIsdkInteractableState::Select:
      DebugColor = RuntimeSettings->InteractableColorSelect;
      break;
    case EIsdkInteractableState::Disabled:
      DebugColor = RuntimeSettings->InteractableColorDisabled;
      break;
    case EIsdkInteractableState::Normal:
    default:
      DebugColor = RuntimeSettings->InteractableColorNormal;
  }

  return DebugColor;
}
