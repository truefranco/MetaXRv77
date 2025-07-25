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

#include "Core/IsdkIGameplayTagContainer.h"
#include "Interaction/IsdkPokeInteractor.h"
#include "Interaction/IsdkPokeInteractable.h"
#include "Interaction/IsdkGrabberComponent.h"
#include "Interaction/IsdkGrabbableComponent.h"
#include "IsdkRuntimeSettings.h"
#include "GameplayTagContainer.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FIsdkGameplayTagsTest,
    "InteractionSDK.OculusInteraction.Source.OculusInteraction.Private.Tests.FIsdkGameplayTagsTest.All",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FIsdkGameplayTagsTest::RunTest(const FString& Parameters)
{
  // Spawn four main scene components for interactor/interactables, ensure their GameplayTags exist
  // via the interface.
  UIsdkPokeInteractor* TestInteractor = NewObject<UIsdkPokeInteractor>();
  UIsdkPokeInteractable* TestInteractable = NewObject<UIsdkPokeInteractable>();
  UIsdkGrabberComponent* TestSceneInteractor = NewObject<UIsdkGrabberComponent>();
  UIsdkGrabbableComponent* TestSceneInteractable = NewObject<UIsdkGrabbableComponent>();

  FGameplayTagContainer TestGPTC;

  IIsdkIGameplayTagContainer::Execute_GetGameplayTagContainer(TestInteractor, TestGPTC);
  TestEqual(
      "UIsdkInteractorComponent has Interactor.Poke tag",
      TestGPTC.HasTag(IsdkGameplayTags::TAG_Isdk_Type_Interactor_Poke),
      true);

  IIsdkIGameplayTagContainer::Execute_GetGameplayTagContainer(TestInteractable, TestGPTC);
  TestEqual(
      "UIsdkInteractableComponent has Interactable.Poke tag",
      TestGPTC.HasTag(IsdkGameplayTags::TAG_Isdk_Type_Interactable_Poke),
      true);

  IIsdkIGameplayTagContainer::Execute_GetGameplayTagContainer(TestSceneInteractor, TestGPTC);
  TestEqual(
      "UIsdkSceneInteractorComponent has Interactor.Grab tag",
      TestGPTC.HasTag(IsdkGameplayTags::TAG_Isdk_Type_Interactor_Grab),
      true);

  IIsdkIGameplayTagContainer::Execute_GetGameplayTagContainer(TestSceneInteractable, TestGPTC);
  TestEqual(
      "UIsdkSceneInteractableComponent has Interactable.Grab tag",
      TestGPTC.HasTag(IsdkGameplayTags::TAG_Isdk_Type_Interactable_Grab),
      true);

  return true;
}
