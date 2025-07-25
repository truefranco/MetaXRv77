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

#include "IsdkExternalHandDataModifierTestFixtures.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    IsdkExternalHandDataModifierTest,
    "InteractionSDK.OculusInteraction.Source.OculusInteraction.Private.Tests.DataSources.ExternalHandDataModifier",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool IsdkExternalHandDataModifierTest::RunTest(const FString& Parameters)
{
  UIsdkExternalHandDataModifierTestFixtures* Modifier =
      NewObject<UIsdkExternalHandDataModifierTestFixtures>();
  UIsdkHandData* HandData = NewObject<UIsdkHandData>();

  Modifier->OnModify(HandData);

  const auto Index = UIsdkExternalHandDataModifierTestFixtures::JointIndex;
  const FVector RotationVector = HandData->GetJointPoses()[Index].GetRotation().Euler();

  TestTrue(
      TEXT("Joint should rotate 90 degrees around Z-axis"),
      RotationVector.Equals(FVector(0.0f, 0.0f, 90.0f), 0.01f));

  return true;
}

void UIsdkExternalHandDataModifierTestFixtures::OnModify_Implementation(
    UIsdkHandData* InputHandData)
{
  FQuat Rotation = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, 90.0f));
  Rotation.Normalize();
  // Set custom rotation for the index finger joint
  InputHandData->GetJointPoses()[JointIndex].SetRotation(Rotation);
}
