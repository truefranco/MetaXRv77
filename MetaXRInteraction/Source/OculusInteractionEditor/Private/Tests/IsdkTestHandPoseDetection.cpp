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

#include "IsdkTestHandPoseDetection.h"
#include "Tests/IsdkCommonTestCommands.h"
#include "Misc/AutomationTest.h"

// A generic test step that finds the AIsdkTestHandPoseDetectionActor test object, then passes it
// into a lambda
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(
    FIsdkTestHandPoseDetectionLambda,
    FAutomationTestBase*,
    Test,
    std::function<void(FAutomationTestBase* Test, AIsdkTestHandPoseDetectionActor& TestActor)>,
    TestCallback);

bool FIsdkTestHandPoseDetectionLambda::Update()
{
  AIsdkTestHandPoseDetectionActor* TestActor{};
  Test->AddErrorIfFalse(
      AIsdkTestHandPoseDetectionActor::TryGetChecked(TestActor),
      TEXT("AIsdkTestHandPoseDetectionActor was not in the test scene."));

  if (TestActor)
  {
    TestCallback(Test, *TestActor);
  }
  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(
    FIsdkTestSpawnHandPoseDetectionActor,
    FAutomationTestBase*,
    Test);

bool FIsdkTestSpawnHandPoseDetectionActor::Update()
{
  AIsdkTestHandPoseDetectionActor::SetUp();
  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkTestHandPoseDetection,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.HandPose.IsdkHandPoseDetection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkTestHandPoseDetection::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnHandPoseDetectionActor(this));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // Test that the hand pose detection has valid data.
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestHandPoseDetectionLambda(
      this,
      [](FAutomationTestBase* Test, AIsdkTestHandPoseDetectionActor& TestActor)
      {
        Test->TestFalse(
            TEXT(
                "HandVisual should not have a valid HandPositionFrame instance until it requested at least once."),
            TestActor.TestTrackedHandVisual->IsApiInstanceValid());
        Test->TestNotNull(
            TEXT("HandVisual.GetApiIHandPositionFrame() should succeed."),
            TestActor.TestTrackedHandVisual->TryGetApiIHandPositionFrame());
        Test->TestTrue(
            TEXT("HandVisual should have a valid HandPositionFrame instance."),
            TestActor.TestTrackedHandVisual->IsApiInstanceValid());
        TestActor.TestFingerPinchGrabRecognizer->UpdateState(0.0f);
        Test->TestFalse(
            TEXT("Recognizer should not detect a pinch."),
            TestActor.TestFingerPinchGrabRecognizer->IsActive());
      }));

  // Change to pinching pose
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestHandPoseDetectionLambda(
      this,
      [](FAutomationTestBase* Test, AIsdkTestHandPoseDetectionActor& TestActor)
      { TestActor.TestDataSourceExternal->SetHandJointsToPinchPose(); }));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));

  // Update the recognizer - it should now detect pinching pose.
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestHandPoseDetectionLambda(
      this,
      [](FAutomationTestBase* Test, AIsdkTestHandPoseDetectionActor& TestActor)
      {
        TestActor.TestFingerPinchGrabRecognizer->UpdateState(0.1f);
        Test->TestTrue(
            TEXT("Recognizer should detect a pinch."),
            TestActor.TestFingerPinchGrabRecognizer->IsActive());
      }));

  return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(
    FIsdkTestHandPoseDetectionSetForwards,
    FVector,
    WristForwardIn,
    FVector,
    HMDForwardIn);

bool FIsdkTestHandPoseDetectionSetForwards::Update()
{
  AIsdkTestHandPoseDetectionActor* TestActor{};
  AIsdkTestHandPoseDetectionActor::TryGetChecked(TestActor);
  TestActor->TestFingerPinchGrabRecognizer->CurrentWristForward = WristForwardIn;
  TestActor->TestFingerPinchGrabRecognizer->CurrentHMDForward = HMDForwardIn;
  TestActor->TestFingerPinchGrabRecognizer->UpdatePinchConfidence();
  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkRigPinchRecognizerTests,
    "InteractionSDK.OculusInteraction.Source.OculusInteractionEditor.Private.Tests.HandPose.IsdkPinchConfidence",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkRigPinchRecognizerTests::RunTest(const FString& Parameters)
{
  isdk::test::AddInitPieTestSteps(this);

  const FVector GoodWristForward = {-0.830, 0.520, -0.120};
  const FVector GoodHMDForward = {-0.630, 0.760, 0.110};

  const FVector BadWristForward = {0.999, 0.900, -0.125};
  const FVector BadHMDForward = {0.999, 0.991, 0.999};

  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestSpawnHandPoseDetectionActor(this));
  ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(isdk::test::OneFrameDelay));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestHandPoseDetectionLambda(
      this,
      [](FAutomationTestBase* Test, AIsdkTestHandPoseDetectionActor& TestActor)
      {
        TestActor.TestTrackedHandVisual->TryGetApiIHandPositionFrame();
        TestActor.TestTrackedHandVisual->IsApiInstanceValid();
      }));

  ADD_LATENT_AUTOMATION_COMMAND(
      FIsdkTestHandPoseDetectionSetForwards(GoodWristForward, GoodHMDForward));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestHandPoseDetectionLambda(
      this,
      [](FAutomationTestBase* Test, AIsdkTestHandPoseDetectionActor& TestActor)
      {
        Test->TestTrue(
            TEXT("Recognizer should have high confidence."),
            TestActor.TestFingerPinchGrabRecognizer->bLastPinchConfidence);
      }));
  ADD_LATENT_AUTOMATION_COMMAND(
      FIsdkTestHandPoseDetectionSetForwards(BadWristForward, BadHMDForward));
  ADD_LATENT_AUTOMATION_COMMAND(FIsdkTestHandPoseDetectionLambda(
      this,
      [](FAutomationTestBase* Test, AIsdkTestHandPoseDetectionActor& TestActor)
      {
        Test->TestFalse(
            TEXT("Recognizer should have low confidence."),
            TestActor.TestFingerPinchGrabRecognizer->bLastPinchConfidence);
      }));

  ADD_LATENT_AUTOMATION_COMMAND(FEndPlayMapCommand);

  return true;
}
