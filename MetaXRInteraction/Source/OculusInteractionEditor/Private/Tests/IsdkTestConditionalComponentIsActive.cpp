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

#include "IsdkTestConditionalComponentIsActive.h"

#include "Tests/IsdkCommonTestCommands.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    FIsdkTestConditionalComponentIsActiveSpawnComponents,
    FAutomationTestBase*,
    Test);

// Test Step: Create an instance of AIsdkTestConditionalComponentIsActive_Actor, Component
// which contains the object under test & its dependencies.
bool FIsdkTestConditionalComponentIsActiveSpawnComponents::Update()
{
  // No getting the level dirty
  FActorSpawnParameters ActorParameters{};
  ActorParameters.bNoFail = true;

  UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();

  const auto TestActor = TestWorld->SpawnActor<AIsdkTestConditionalComponentIsActive_Actor>(
      AIsdkTestConditionalComponentIsActive_Actor::StaticClass(), ActorParameters);

  if (!TestActor)
  {
    Test->AddError("Failed to find TestActor in the scene.");
  }

  return true;
}

// Test Step: Compare actual conditional value against expected value
DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(
    TestConditionalComponentIsActiveResolvedValue,
    FAutomationTestBase*,
    Test,
    bool,
    bExpectedValue,
    FString,
    TestStepName);
bool TestConditionalComponentIsActiveResolvedValue::Update()
{
  const auto* Actor = AIsdkTestConditionalComponentIsActive_Actor::Get();

  const bool bResolvedValue = Actor->Conditional->GetResolvedValue();
  const FString TestDescription = FString::Printf(
      TEXT("%s: Conditional Resolved Value (actual: %s, expected: %s)"),
      *TestStepName,
      bResolvedValue ? TEXT("true") : TEXT("false"),
      bExpectedValue ? TEXT("true") : TEXT("false"));

  this->Test->TestEqual(TestDescription, bResolvedValue, bExpectedValue);

  return true;
}

// Test Step: Simply sets whether or not the component is active.
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    TestConditionalComponentIsActiveSetComponentActive,
    bool,
    bActiveValue);
bool TestConditionalComponentIsActiveSetComponentActive::Update()
{
  AIsdkTestConditionalComponentIsActive_Actor::Get()->OtherComponent->SetActive(bActiveValue);
  return true;
}

// Test Step: Simply sets whether or not the component is active.
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    TestConditionalComponentIsActiveSetHasComponent,
    bool,
    bActiveValue);
bool TestConditionalComponentIsActiveSetHasComponent::Update()
{
  const auto TestActor = AIsdkTestConditionalComponentIsActive_Actor::Get();
  TestActor->Conditional->SetComponent(bActiveValue ? TestActor->OtherComponent : nullptr);
  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkTestConditionalComponentIsActive,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.Core.ConditionalComponentIsActive",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkTestConditionalComponentIsActive::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestConditionalComponentIsActiveSpawnComponents(this));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // Test initial state accounts for component auto-activation
  ADD_LATENT_AUTOMATION_COMMAND(
      TestConditionalComponentIsActiveResolvedValue(this, true, TEXT("Value after Auto Activate")));

  // Test value changes when component IsActive changes.
  ADD_LATENT_AUTOMATION_COMMAND(TestConditionalComponentIsActiveSetComponentActive(false));
  ADD_LATENT_AUTOMATION_COMMAND(TestConditionalComponentIsActiveResolvedValue(
      this, false, TEXT("Value after setting Activate to false")));

  ADD_LATENT_AUTOMATION_COMMAND(TestConditionalComponentIsActiveSetComponentActive(true));
  ADD_LATENT_AUTOMATION_COMMAND(TestConditionalComponentIsActiveResolvedValue(
      this, true, TEXT("Value after setting Activate to true")));

  // Test value goes to false when component is removed & re-added
  ADD_LATENT_AUTOMATION_COMMAND(TestConditionalComponentIsActiveSetHasComponent(false));
  ADD_LATENT_AUTOMATION_COMMAND(TestConditionalComponentIsActiveResolvedValue(
      this, false, TEXT("Value after removing component")));
  ADD_LATENT_AUTOMATION_COMMAND(TestConditionalComponentIsActiveSetHasComponent(true));
  ADD_LATENT_AUTOMATION_COMMAND(TestConditionalComponentIsActiveResolvedValue(
      this, true, TEXT("Value after re-adding component")));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}
