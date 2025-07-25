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

#include "IsdkTestRayInteraction.h"
#include "Tests/IsdkCommonTestCommands.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

#include <functional>

static std::function<
    void(TScriptInterface<IIsdkIInteractorState>&, TScriptInterface<IIsdkIInteractableState>&)>
IsdkTestRayInteractionCreateGetActorsPlaneFn()
{
  return [](TScriptInterface<IIsdkIInteractorState>& OutInteractor,
            TScriptInterface<IIsdkIInteractableState>& OutInteractable)
  {
    OutInteractor = AIsdkTestRayInteractorActor::Get().GetTestRayInteractor();
    OutInteractable =
        AIsdkTestRayInteractableActor::Get().TestRayInteractablePlane->GetInteractable();
  };
}
static std::function<
    void(TScriptInterface<IIsdkIInteractorState>&, TScriptInterface<IIsdkIInteractableState>&)>
IsdkTestRayInteractionCreateGetActorsBoxFn()
{
  return [](TScriptInterface<IIsdkIInteractorState>& OutInteractor,
            TScriptInterface<IIsdkIInteractableState>& OutInteractable)
  {
    OutInteractor = AIsdkTestRayInteractorActor::Get().GetTestRayInteractor();
    OutInteractable =
        AIsdkTestRayInteractableActor::Get().TestRayInteractableBox->GetInteractable();
  };
}

// A generic test step that finds the AInteractorTestFixturesActor test object, then passes it into
// a lambda
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(
    FIsdkTestRayInteractionLambda,
    FAutomationTestBase*,
    Test,
    std::function<void(
        FAutomationTestBase* Test,
        AIsdkTestRayInteractorActor& InteractorActor,
        AIsdkTestRayInteractableActor& InteractableActor)>,
    TestCallback);

bool FIsdkTestRayInteractionLambda::Update()
{
  AIsdkTestRayInteractorActor* InteractorActor{};
  AIsdkTestRayInteractableActor* InteractableActor{};
  Test->AddErrorIfFalse(
      AIsdkTestRayInteractorActor::TryGetChecked(InteractorActor),
      TEXT("AInteractorTestFixturesActor was not in the test scene."));
  Test->AddErrorIfFalse(
      AIsdkTestRayInteractableActor::TryGetChecked(InteractableActor),
      TEXT("AInteractableTestFixturesActor was not in the test scene."));

  if (InteractorActor && InteractableActor)
  {
    TestCallback(Test, *InteractorActor, *InteractableActor);
  }
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    FIsdkTestSpawnInteractors,
    FAutomationTestBase*,
    Test);

bool FIsdkTestSpawnInteractors::Update()
{
  AIsdkTestRayInteractorActor::SetUp();
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    FIsdkTestSpawnInteractables,
    FAutomationTestBase*,
    Test);

bool FIsdkTestSpawnInteractables::Update()
{
  AIsdkTestRayInteractableActor::SetUp();
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(
    FIsdkTestCheckRayInteractorCurrentState,
    FAutomationTestBase*,
    Test,
    EIsdkInteractorState,
    ExpectedState,
    FString,
    TestStepName);

bool FIsdkTestCheckRayInteractorCurrentState::Update()
{
  const auto* TestRayInteractor = AIsdkTestRayInteractorActor::Get().GetTestRayInteractor();
  const auto CurrentState = TestRayInteractor->GetCurrentState();
  const FString TestDescription = FString::Printf(
      TEXT("%s: Ray Interactor State (actual: %d, expected: %d)"),
      *TestStepName,
      CurrentState,
      ExpectedState);
  Test->TestEqual(*TestDescription, CurrentState, ExpectedState);
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(
    FIsdkTestCheckRayInteractableCurrentState,
    FAutomationTestBase*,
    Test,
    EIsdkInteractableState,
    ExpectedState,
    FString,
    TestStepName);

bool FIsdkTestCheckRayInteractableCurrentState::Update()
{
  const auto* TestRayInteractable =
      AIsdkTestRayInteractableActor::Get().TestRayInteractablePlane->GetInteractable();
  const auto CurrentState = TestRayInteractable->GetCurrentState();
  const FString TestDescription = FString::Printf(
      TEXT("%s: Ray Interactable State (actual: %d, expected: %d)"),
      *TestStepName,
      CurrentState,
      ExpectedState);
  Test->TestEqual(*TestDescription, CurrentState, ExpectedState);
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    FIsdkTestSetPointerPoseIsAssigned,
    bool,
    bPointerPoseIsAssigned);

bool FIsdkTestSetPointerPoseIsAssigned::Update()
{
  auto* TestRayInteractor = AIsdkTestRayInteractorActor::Get().GetTestRayInteractor();
  const TScriptInterface<IIsdkIHandPointerPose> TestHandPointerPose = bPointerPoseIsAssigned
      ? AIsdkTestRayInteractorActor::Get().GetTestHandPointerPose()
      : nullptr;
  TestRayInteractor->SetHandPointerPose(TestHandPointerPose);
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    FIsdkTestSetPointerPoseIsValid,
    bool,
    bPointerPoseIsValid);

bool FIsdkTestSetPointerPoseIsValid::Update()
{
  auto* TestHandPointerPose = AIsdkTestRayInteractorActor::Get().GetTestHandPointerPose();
  TestHandPointerPose->bIsPointerPoseValid = bPointerPoseIsValid;
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(
    FIsdkTestSetPointerPoseTransform,
    FTransform,
    Transform,
    bool,
    bPointerPoseIsValid);

bool FIsdkTestSetPointerPoseTransform::Update()
{
  auto* TestRayInteractor = AIsdkTestRayInteractorActor::Get().GetTestRayInteractor();
  auto* TestHandPointerPose = AIsdkTestRayInteractorActor::Get().GetTestHandPointerPose();

  TestRayInteractor->SetHandPointerPose(TestHandPointerPose);
  TestHandPointerPose->bIsPointerPoseValid = bPointerPoseIsValid;
  TestHandPointerPose->RelativePointerPose = {};
  TestHandPointerPose->PointerPose = Transform;

  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkRayInteractionPointerPoseTests,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.IsdkRayInteractionPointerPoseTests",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkRayInteractionPointerPoseTests::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractors(this));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Disabled, TEXT("State with un-ticked interactor")));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSetPointerPoseIsAssigned(false));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Normal, TEXT("State with NULL IPointerPose")));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSetPointerPoseIsValid(false));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSetPointerPoseIsAssigned(true));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this,
      EIsdkInteractorState::Disabled,
      TEXT("State with IPointerPose.bIsPointerPoseValid = false")));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSetPointerPoseIsValid(true));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this,
      EIsdkInteractorState::Normal,
      TEXT("State with IPointerPose.bIsPointerPoseValid = true")));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkRayInteractionHoverPlaneTests,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.IsdkRayInteractionHoverPlaneTests",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkRayInteractionHoverPlaneTests::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractors(this));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractables(this));

  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Hover, TEXT("State with interactor hovering on plane")));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckInteractionRelationships(
      this,
      IsdkTestRayInteractionCreateGetActorsPlaneFn(),
      {1, 0},
      TEXT("Relationships with interactor hovering on plane")));

  // Move interactor upwards so that the ray misses the plane
  const FVector PointerPoseNotHovered =
      FVector::UpVector * AIsdkTestRayInteractableActor::PlaneSize.Y * 2.0f;
  ADD_LATENT_AUTOMATION_COMMAND(
      FIsdkTestSetPointerPoseTransform(FTransform(PointerPoseNotHovered), true));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Normal, TEXT("State with interactor not pointing at plane")));

  // Move interactor back to origin so that the ray hits the plane; but disable the interactable.
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSetPointerPoseTransform(FTransform(), true));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestRayInteractionLambda(
      this,
      [](auto*, AIsdkTestRayInteractorActor&, AIsdkTestRayInteractableActor& InteractableActor)
      { InteractableActor.TestRayInteractablePlane->GetInteractable()->SetActive(false); }));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this,
      EIsdkInteractorState::Normal,
      TEXT("State with interactor pointing at disabled plane")));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkRayInteractionSelectPlaneTests,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.IsdkRayInteractionSelectPlaneTests",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkRayInteractionSelectPlaneTests::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractors(this));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractables(this));

  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Hover, TEXT("State with interactor hovering on plane")));

  // Select the interactor
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestRayInteractionLambda(
      this,
      [](auto*, AIsdkTestRayInteractorActor& InteractorActor, AIsdkTestRayInteractableActor&)
      { InteractorActor.GetTestRayInteractor()->Select(); }));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Select, TEXT("State with interactor selected")));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckInteractionRelationships(
      this,
      IsdkTestRayInteractionCreateGetActorsPlaneFn(),
      {0, 1},
      TEXT("Relationships with interactor selected")));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkRayInteractionHoverBoxTests,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.IsdkRayInteractionHoverBoxTests",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkRayInteractionHoverBoxTests::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractors(this));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractables(this));

  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // The box is located along the Right axis; make the vector point towards that.
  const FTransform PointerPoseHovered =
      FTransform(FQuat::FindBetweenNormals(FVector::ForwardVector, FVector::RightVector));
  ADD_LATENT_AUTOMATION_COMMAND(
      FIsdkTestSetPointerPoseTransform(FTransform(PointerPoseHovered), true));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Hover, TEXT("State with interactor hovering on box")));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // Move interactor upwards so that the ray misses the plane
  const FVector PointerPoseNotHovered =
      FVector::UpVector * AIsdkTestRayInteractableActor::BoxSize.Y * 2.0f;
  ADD_LATENT_AUTOMATION_COMMAND(
      FIsdkTestSetPointerPoseTransform(FTransform(PointerPoseNotHovered), true));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Normal, TEXT("State with interactor not pointing at box")));

  // Move interactor back to origin so that the ray hits the plane; but disable the interactable.
  ADD_LATENT_AUTOMATION_COMMAND(
      FIsdkTestSetPointerPoseTransform(FTransform(PointerPoseHovered), true));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestRayInteractionLambda(
      this,
      [](auto*, AIsdkTestRayInteractorActor&, AIsdkTestRayInteractableActor& InteractableActor)
      { InteractableActor.TestRayInteractableBox->GetInteractable()->SetActive(false); }));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Normal, TEXT("State with interactor pointing at disabled box")));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkRayInteractionSelectBoxTests,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.IsdkRayInteractionSelectBoxTests",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkRayInteractionSelectBoxTests::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractors(this));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractables(this));

  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // The box is located along the Right axis; make the vector point towards that.
  const FTransform PointerPoseHovered =
      FTransform(FQuat::FindBetweenNormals(FVector::ForwardVector, FVector::RightVector));
  ADD_LATENT_AUTOMATION_COMMAND(
      FIsdkTestSetPointerPoseTransform(FTransform(PointerPoseHovered), true));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Hover, TEXT("State with interactor hovering on box")));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckInteractionRelationships(
      this,
      IsdkTestRayInteractionCreateGetActorsBoxFn(),
      {1, 0},
      TEXT("Relationships with interactor hovered")));

  // Select the interactor
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestRayInteractionLambda(
      this,
      [](auto*, AIsdkTestRayInteractorActor& InteractorActor, AIsdkTestRayInteractableActor&)
      { InteractorActor.GetTestRayInteractor()->Select(); }));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Select, TEXT("State with interactor selected")));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckInteractionRelationships(
      this,
      IsdkTestRayInteractionCreateGetActorsBoxFn(),
      {0, 1},
      TEXT("Relationships with interactor selected")));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkRayInteractionCallbackDestroyInteractorTests,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.IsdkRayInteractionCallbackDestroyInteractorTests",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkRayInteractionCallbackDestroyInteractorTests::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractors(this));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnInteractables(this));

  // Tick (to make sure the interactor/interactables are created) then validate initial state.
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractorCurrentState(
      this, EIsdkInteractorState::Hover, TEXT("Initial interactor state")));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractableCurrentState(
      this, EIsdkInteractableState::Hover, TEXT("Initial interactable state")));

  // Wire up an event handler that will be invoked later
  const auto DestroyTask =
      [](auto*, AIsdkTestRayInteractorActor& InteractorActor, AIsdkTestRayInteractableActor&)
  {
    InteractorActor.GetInteractorStateChangedHandler()->SetLambda(
        [](FIsdkInteractorStateEvent State)
        {
          if (State.Args.NewState == EIsdkInteractorState::Select)
          {
            Cast<UActorComponent>(State.Interactor.GetObject())->DestroyComponent();
          }
        });
  };
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestRayInteractionLambda(this, DestroyTask));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // Select the interactable, which should trigger the callback to destroy the interactor.
  const auto SelectTask =
      [](auto*, AIsdkTestRayInteractorActor& InteractorActor, AIsdkTestRayInteractableActor&)
  { InteractorActor.GetTestRayInteractor()->Select(); };
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestRayInteractionLambda(this, SelectTask));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // Check that interactor is actually destroyed
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestRayInteractionLambda(
      this,
      [](FAutomationTestBase* Test,
         AIsdkTestRayInteractorActor& InteractorActor,
         AIsdkTestRayInteractableActor&)
      {
        Test->TestFalse(
            TEXT("RayInteractor should have been marked invalid when it was destroyed."),
            IsValid(InteractorActor.GetTestRayInteractor()));
      }));
  // Now that interactor is destroyed, check that the interactable is in state Normal.
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestCheckRayInteractableCurrentState(
      this,
      EIsdkInteractableState::Normal,
      TEXT("Interactable state after interactor was destroyed")));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}
