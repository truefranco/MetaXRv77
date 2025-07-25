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

#include "Subsystem/IsdkWidgetSubsystem.h"

#include "OculusInteractionLog.h"

UIsdkWidgetSubsystem::UIsdkWidgetSubsystem() {}

void UIsdkWidgetSubsystem::BeginDestroy()
{
  Super::BeginDestroy();

  VirtualUserIndexInteractors.SetNum(0);
}

void UIsdkWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
  Super::Initialize(Collection);
}

#pragma region Virtual User Info Handling
void UIsdkWidgetSubsystem::RegisterVirtualUserInfo(
    UObject* Interactor,
    FIsdkVirtualUserInfo VirtualUserInfo)
{
  VirtualUserInfoMap.Add(Interactor, VirtualUserInfo);
}

void UIsdkWidgetSubsystem::UnregisterVirtualUserInfo(UObject* Interactor)
{
  VirtualUserInfoMap.Remove(Interactor);
}

const FIsdkVirtualUserInfo& UIsdkWidgetSubsystem::GetVirtualUserInfo(UObject* Interactor)
{
  checkf(
      VirtualUserInfoMap.Contains(Interactor),
      TEXT("%s: VirtualUserIndexMap does not contain this Key!"),
      *Interactor->GetFullName());

  return VirtualUserInfoMap[Interactor];
}
#pragma endregion Virtual User Info Handling

FIsdkWidgetVirtualUserState* UIsdkWidgetSubsystem::FindOrCreateInteractorVirtualUserState(
    const FIsdkVirtualUserPointerEvent& PointerEvent)
{
  if (!ensure(FSlateApplication::IsInitialized() && !GetWorld()->IsPreviewWorld()))
  {
    return nullptr;
  }

  for (int32 Index = 0; Index < VirtualUserIndexInteractors.Num(); ++Index)
  {
    auto& VirtualUserIndexInteractor = VirtualUserIndexInteractors[Index];
    if (PointerEvent.Interactor == VirtualUserIndexInteractor.Interactor)
    {
      return &VirtualUserIndexInteractor;
    }
  }

  // No valid entry was found. Inspect the interactor to get the virtual user index then bind
  // the Pointer to the Slate.
  const auto SlateVirtualUserIndex = PointerEvent.VirtualUserIndex;

  // Bind
  if (FSlateApplication::IsInitialized())
  {
    const TSharedRef<FSlateVirtualUserHandle> SlateVirtualUser =
        FSlateApplication::Get().FindOrCreateVirtualUser(SlateVirtualUserIndex);

    const FIsdkWidgetVirtualUserState State{
        PointerEvent.Interactor,
        SlateVirtualUser,
        PointerEvent.PointerIndex,
        {},
        {},
        {},
        {},
        {},
        false,
        false};
    UE_LOG(
        LogOculusInteraction,
        Verbose,
        TEXT("Binding Interactor %s to VirtualUser %u"),
        *PointerEvent.Interactor->GetName(),
        State.VirtualUser->GetVirtualUserIndex());

    return &VirtualUserIndexInteractors.Add_GetRef(State);
  }

  checkf(
      false,
      TEXT("Could not find a UIsdkWidgetVirtualUser child component on interactor %s"),
      *PointerEvent.Interactor->GetFullName());
  return nullptr;
}

void UIsdkWidgetSubsystem::DestroyInteractorVirtualUserState(UObject* Interactor)
{
  if (Interactor == nullptr)
  {
    return;
  }

  for (int32 Index = 0; Index < VirtualUserIndexInteractors.Num(); ++Index)
  {
    if (Interactor == VirtualUserIndexInteractors[Index].Interactor)
    {
      UE_LOG(
          LogOculusInteraction,
          Verbose,
          TEXT("Unbinding Interactor %s from VirtualUser %u"),
          *Interactor->GetName(),
          Index);
      VirtualUserIndexInteractors[Index].Interactor = nullptr;
      break;
    }
  }
}

ECollisionChannel UIsdkWidgetSubsystem::GetUninitializedChannel()
{
  // A value to indicate this channel has not been set yet
  return ECC_WorldStatic;
}

ECollisionChannel UIsdkWidgetSubsystem::GetDefaultCollisionChannel()
{
  // TODO: retrieve from a config file
  return ECC_GameTraceChannel2; // IsdkCollision
}

ECollisionChannel UIsdkWidgetSubsystem::GetDefaultHoverChannel()
{
  // TODO: retrieve from a config file
  return ECC_GameTraceChannel3; // IsdkHover
}

bool UIsdkWidgetSubsystem::IsDebugDrawEnabled()
{
  // TODO: drive this by a configuration
  return true;
}

bool UIsdkWidgetSubsystem::ShouldDebugDrawMoveEvents()
{
  // TODO: drive this by a configuration
  return false;
}
