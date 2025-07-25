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

#include "IsdkTestDataSources.h"
#include "Tests/IsdkCommonTestCommands.h"

#include <functional>

// A generic test step that finds the AIsdkTestDataSourcesActor test object, then passes it into
// a lambda
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(
    FIsdkTestDataSourcesLambda,
    FAutomationTestBase*,
    Test,
    std::function<void(FAutomationTestBase* Test, AIsdkTestDataSourcesActor& TestActor)>,
    TestCallback);

bool FIsdkTestDataSourcesLambda::Update()
{
  AIsdkTestDataSourcesActor* TestActor{};
  Test->AddErrorIfFalse(
      AIsdkTestDataSourcesActor::TryGetChecked(TestActor),
      TEXT("AIsdkTestDataSourcesActor was not in the test scene."));

  if (TestActor)
  {
    TestCallback(Test, *TestActor);
  }
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    FIsdkTestSpawnDataSources,
    FAutomationTestBase*,
    Test);

bool FIsdkTestSpawnDataSources::Update()
{
  AIsdkTestDataSourcesActor::SetUp();
  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkDataModifierInitOrderTests,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.IsdkDataModifierInitOrderTests",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

auto CreateOneEuroIsValidFn(bool bExpected)
{
  return [bExpected](FAutomationTestBase* Test, AIsdkTestDataSourcesActor& Actor)
  {
    const bool bIsRootPoseValid =
        IIsdkIRootPose::Execute_IsRootPoseValid(Actor.TestDataModifierOneEuro);
    Test->TestEqual(TEXT("TestDataModifierOneEuro->IsRootPoseValid"), bIsRootPoseValid, bExpected);
    const bool bIsHandJointDataValid =
        IIsdkIHandJoints::Execute_IsHandJointDataValid(Actor.TestDataModifierOneEuro);
    Test->TestEqual(
        TEXT("TestDataModifierOneEuro->IsHandJointDataValid"), bIsHandJointDataValid, bExpected);
  };
}

bool IsdkDataModifierInitOrderTests::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnDataSources(this));

  auto CreateOneEuroHandFilter = [](FAutomationTestBase* Test, AIsdkTestDataSourcesActor& Actor)
  {
    // Force creation.
    Actor.TestDataModifierOneEuro->GetApiOneEuroHandFilter();
    Test->TestTrue(
        TEXT("TestDataModifierOneEuro->IsApiInstanceValid"),
        Actor.TestDataModifierOneEuro->IsApiInstanceValid());
  };
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestDataSourcesLambda(this, CreateOneEuroHandFilter));

  // Tick - testing that ticking the OneEuroFilter works when it doesn't have an InputDataSource set
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // Test that we can check whether or not the data is valid - and that the IsValid result == false
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestDataSourcesLambda(this, CreateOneEuroIsValidFn(false)));

  // Now initialize a base data source, with (fake) valid data. Set it as the input data source.
  auto CreateExternalHandDataSource =
      [](FAutomationTestBase* Test, AIsdkTestDataSourcesActor& Actor)
  {
    // Force creation.
    Actor.TestDataSourceExternal->GetApiExternalHandSource();
    Test->TestTrue(
        TEXT("TestDataSourceExternal->IsApiInstanceValid"),
        Actor.TestDataSourceExternal->IsApiInstanceValid());

    // Assign it as the input data source of the OneEuro
    Actor.TestDataModifierOneEuro->SetInputDataSource(Actor.TestDataSourceExternal);

    Actor.TestDataSourceExternal->bIsFakeRootPoseValid = true;
    Actor.TestDataSourceExternal->bIsFakeJointDataValid = true;
  };
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestDataSourcesLambda(this, CreateExternalHandDataSource));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // Test that the IsValid result is now == true, as there is a valid input data source set.
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestDataSourcesLambda(this, CreateOneEuroIsValidFn(true)));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}
