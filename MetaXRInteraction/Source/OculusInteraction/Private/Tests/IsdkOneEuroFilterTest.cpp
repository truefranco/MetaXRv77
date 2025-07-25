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

#include "IsdkOneEuroFilterTestFixtures.h"
#include "CoreMinimal.h"
#include "isdk_api/isdk_api.hpp"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FIsdkOneEuroFilterTest,
    "InteractionSDK.OculusInteraction.Source.OculusInteraction.Private.Tests.DataSources.OneEuroFilter",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

inline bool FIsdkOneEuroFilterTest::RunTest(const FString& Parameters)
{
  UIsdkOneEuroFilterTestFixtures* TestFixture = NewObject<UIsdkOneEuroFilterTestFixtures>();
  TestFixture->SetUp();

  UIsdkOneEuroFilterDataModifier* OneEuroFilterDataModifier =
      TestFixture->OneEuroFilterDataModifier;

  UIsdkExternalHandDataSource* ExternalHandDataSource = TestFixture->ExternalHandDataSource;

  // Apply the filter

  OneEuroFilterDataModifier->GetApiOneEuroHandFilter()->updateData();
  TestTrue(TEXT("One Euro Filter Api is Valid"), OneEuroFilterDataModifier->IsApiInstanceValid());

  // Retrieve hand data

  isdk_HandData HandData;
  HandData.root.Position = {-9999.f, -9999.f, -9999.f};

  OneEuroFilterDataModifier->GetApiIHandDataSource()->getData(&HandData);
  // Check root position is at origin
  const FVector RootPosition =
      FVector(HandData.root.Position.x, HandData.root.Position.y, HandData.root.Position.z);

  TestTrue(TEXT("Root position is zero"), RootPosition.Equals(FVector::ZeroVector));

  // Add non-zero data to the root position and joint rotation

  HandData.root.Position = {0.01f, 0.012f, 0.014f};
  HandData.joints[0] = ovrpQuatf{0.01f, 0.01f, 0.0f, 0.99f};

  // Update data sources with non-zero data and apply filter again

  ExternalHandDataSource->GetApiExternalHandSource()->setData(&HandData);
  ExternalHandDataSource->GetApiExternalHandSource()->updateData();
  OneEuroFilterDataModifier->GetApiOneEuroHandFilter()->updateData();

  // Retrieve filtered data

  isdk_HandData FilteredHandData;
  OneEuroFilterDataModifier->GetApiOneEuroHandFilter()->getData(&FilteredHandData);
  const FQuat FilteredRotation = FQuat(
      FilteredHandData.joints[0].x,
      FilteredHandData.joints[0].y,
      FilteredHandData.joints[0].z,
      FilteredHandData.joints[0].w);
  const FVector FilteredPosition = FVector(
      FilteredHandData.root.Position.x,
      FilteredHandData.root.Position.y,
      FilteredHandData.root.Position.z);

  constexpr float Tolerance = 0.005f;
  // Check that the filtered rotation and position is closer to zero. This verifies that the filter
  // is smoothing the input data as expected
  TestTrue(
      TEXT("Filtered joint rotation is nearly zero"),
      FilteredRotation.Equals(FQuat::Identity, Tolerance));
  TestTrue(
      TEXT("Filtered root position is nearly zero"),
      FilteredPosition.Equals(FVector::ZeroVector, Tolerance));

  return true;
}
