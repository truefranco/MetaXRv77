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

namespace IsdkInteractionUtils
{
inline FString GetPointerEventTypeName(EIsdkPointerEventType NewState)
{
  FString StateName;
  switch (NewState)
  {
    case EIsdkPointerEventType::Select:
      StateName = TEXT("Select");
      break;
    case EIsdkPointerEventType::Hover:
      StateName = TEXT("Hover");
      break;
    case EIsdkPointerEventType::Move:
      StateName = TEXT("Move");
      break;
    case EIsdkPointerEventType::Unhover:
      StateName = TEXT("Unhover");
      break;
    case EIsdkPointerEventType::Unselect:
      StateName = TEXT("Unselect");
      break;
    case EIsdkPointerEventType::Cancel:
      StateName = TEXT("Cancel");
      break;
  }

  return StateName;
}

inline FString GetInteractableStateName(EIsdkInteractableState NewState)
{
  FString StateName;
  switch (NewState)
  {
    case EIsdkInteractableState::Select:
      StateName = TEXT("Select");
      break;

    case EIsdkInteractableState::Hover:
      StateName = TEXT("Hover");
      break;

    case EIsdkInteractableState::Normal:
      StateName = TEXT("Normal");
      break;

    case EIsdkInteractableState::Disabled:
      StateName = TEXT("Disabled");
      break;
  }

  return StateName;
}

inline FString GetInteractorStateName(EIsdkInteractorState NewState)
{
  FString StateName;
  switch (NewState)
  {
    case EIsdkInteractorState::Select:
      StateName = TEXT("Select");
      break;

    case EIsdkInteractorState::Hover:
      StateName = TEXT("Hover");
      break;

    case EIsdkInteractorState::Normal:
      StateName = TEXT("Normal");
      break;

    case EIsdkInteractorState::Disabled:
      StateName = TEXT("Disabled");
      break;
  }

  return StateName;
}
} // namespace IsdkInteractionUtils
