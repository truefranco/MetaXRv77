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

#include "Core/IsdkConditionalBool.h"
#include "Core/IsdkConditionalGroup.h"
#include "Core/IsdkConditionalGroupAll.h"
#include "Core/IsdkConditionalGroupAny.h"
#include "Core/IsdkConditionalGroupNone.h"
#include "Core/IsdkConditionalGroupSingle.h"

#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FIsdkConditionalEmptyListTest,
    "InteractionSDK.OculusInteraction.Source.OculusInteraction.Private.Tests.Core.ConditionalEmptyList",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

inline bool FIsdkConditionalEmptyListTest::RunTest(const FString& Parameters)
{
  UIsdkConditionalGroupAll* ConditionalGroupAll = NewObject<UIsdkConditionalGroupAll>();
  UIsdkConditionalGroupAny* ConditionalGroupAny = NewObject<UIsdkConditionalGroupAny>();
  UIsdkConditionalGroupNone* ConditionalGroupNone = NewObject<UIsdkConditionalGroupNone>();
  UIsdkConditionalGroupSingle* ConditionalGroupSingle = NewObject<UIsdkConditionalGroupSingle>();

  // Test initial state works as expected when there's nothing in the list
  TestEqual(TEXT("Operator All"), ConditionalGroupAll->GetResolvedValue(), true);
  TestEqual(TEXT("Operator Any"), ConditionalGroupAny->GetResolvedValue(), true);
  TestEqual(TEXT("Operator None"), ConditionalGroupNone->GetResolvedValue(), true);
  TestEqual(TEXT("Operator Single"), ConditionalGroupSingle->GetResolvedValue(), false);
  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FIsdkConditionalSetListTest,
    "InteractionSDK.OculusInteraction.Source.OculusInteraction.Private.Tests.Core.ConditionalSetList",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

inline bool FIsdkConditionalSetListTest::RunTest(const FString& Parameters)
{
  UIsdkConditionalBool* ConditionalA = NewObject<UIsdkConditionalBool>();
  UIsdkConditionalBool* ConditionalB = NewObject<UIsdkConditionalBool>();
  UIsdkConditionalGroupAll* ConditionalGroupAll = NewObject<UIsdkConditionalGroupAll>();

  ConditionalA->SetValue(true);
  ConditionalB->SetValue(true);

  auto AddConditionals = [&]()
  {
    ConditionalGroupAll->AddConditional(ConditionalA);
    ConditionalGroupAll->AddConditional(ConditionalB);
  };
  auto RemoveConditionals = [&]()
  {
    ConditionalGroupAll->RemoveConditional(ConditionalA);
    ConditionalGroupAll->RemoveConditional(ConditionalB);
  };

  int ChangedEventCount = 0;
  bool ChangedEventValue = false;
  ConditionalGroupAll->ResolvedValueChanged.AddLambda(
      [&ChangedEventCount, &ChangedEventValue](bool InValue)
      {
        ++ChangedEventCount;
        ChangedEventValue = InValue;
      });

  // Test that event is not fired when the list of conditionals is changed, but resolved value
  // doesn't change as a result.
  AddConditionals();
  TestEqual(
      TEXT("Initial evaluation should not invoke event as resolved value didn't change."),
      ChangedEventCount,
      0);
  TestTrueExpr(ConditionalGroupAll->GetResolvedValue());

  // Test that event is fired when the list of conditionals is changed, causing a resolved value
  // change.
  ConditionalA->SetValue(false);
  ConditionalB->SetValue(false);
  RemoveConditionals();
  ChangedEventCount = 0;
  AddConditionals();
  TestEqual(
      TEXT("Initial evaluation should invoke event as resolved value changed"),
      ChangedEventCount,
      1);
  TestTrueExpr(!ChangedEventValue);
  TestTrueExpr(!ConditionalGroupAll->GetResolvedValue());

  return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FIsdkConditionalGroupMemberTest,
    "InteractionSDK.OculusInteraction.Source.OculusInteraction.Private.Tests.Core.ConditionalGroupMember",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

inline bool FIsdkConditionalGroupMemberTest::RunTest(const FString& Parameters)
{
  UIsdkConditionalBool* ConditionalA = NewObject<UIsdkConditionalBool>();
  UIsdkConditionalBool* ConditionalB = NewObject<UIsdkConditionalBool>();
  UIsdkConditionalGroupAll* ConditionalGroupAll = NewObject<UIsdkConditionalGroupAll>();
  UIsdkConditionalGroupAny* ConditionalGroupAny = NewObject<UIsdkConditionalGroupAny>();
  UIsdkConditionalGroupNone* ConditionalGroupNone = NewObject<UIsdkConditionalGroupNone>();
  UIsdkConditionalGroupSingle* ConditionalGroupSingle = NewObject<UIsdkConditionalGroupSingle>();

  ConditionalA->SetValue(false);
  ConditionalB->SetValue(false);
  ConditionalGroupAll->AddConditional(ConditionalA);
  ConditionalGroupAll->AddConditional(ConditionalB);
  ConditionalGroupAny->AddConditional(ConditionalA);
  ConditionalGroupAny->AddConditional(ConditionalB);
  ConditionalGroupNone->AddConditional(ConditionalA);
  ConditionalGroupNone->AddConditional(ConditionalB);
  ConditionalGroupSingle->AddConditional(ConditionalA);
  ConditionalGroupSingle->AddConditional(ConditionalB);

  const auto Test = [&](auto ConditionGroup,
                        FString GroupType,
                        bool ValueA,
                        bool ValueB,
                        bool Expected,
                        int LineNo)
  {
    ConditionalA->SetValue(ValueA);
    ConditionalB->SetValue(ValueB);

    const FString Msg = FString::Format(
        TEXT(
            "ConditionalA={0}, ConditionalB={1}, Operator={2}, Resolved={3}, Expected={4} (Line {5})"),
        {ValueA, ValueB, GroupType, ConditionGroup->GetResolvedValue(), Expected, LineNo});

    TestEqual(Msg, ConditionGroup->GetResolvedValue(), Expected);
  };

  // Test all combinations of child values. This also tests that the Group is correctly subscribed
  // to its child conditions.

  // Test All Type
  Test(ConditionalGroupAll, "All", true, true, true, __LINE__);
  Test(ConditionalGroupAll, "All", true, false, false, __LINE__);
  Test(ConditionalGroupAll, "All", false, true, false, __LINE__);
  Test(ConditionalGroupAll, "All", false, false, false, __LINE__);

  // Test None type
  Test(ConditionalGroupNone, "None", true, true, false, __LINE__);
  Test(ConditionalGroupNone, "None", true, false, false, __LINE__);
  Test(ConditionalGroupNone, "None", false, true, false, __LINE__);
  Test(ConditionalGroupNone, "None", false, false, true, __LINE__);

  // Test Any Type
  Test(ConditionalGroupAny, "Any", true, true, true, __LINE__);
  Test(ConditionalGroupAny, "Any", true, false, true, __LINE__);
  Test(ConditionalGroupAny, "Any", false, true, true, __LINE__);
  Test(ConditionalGroupAny, "Any", false, false, false, __LINE__);

  // Test Single Type
  Test(ConditionalGroupSingle, "Single", true, true, false, __LINE__);
  Test(ConditionalGroupSingle, "Single", true, false, true, __LINE__);
  Test(ConditionalGroupSingle, "Single", false, true, true, __LINE__);
  Test(ConditionalGroupSingle, "Single", false, false, false, __LINE__);

  // Use the All group to sample; remove the children, then change their values. ensure that the
  // group correctly ignores those changes.
  ConditionalGroupAll->RemoveConditional(ConditionalA);
  ConditionalGroupAll->RemoveConditional(ConditionalB);
  TArray<UIsdkConditional*> Conditionals;
  ConditionalGroupAll->GetConditionals(Conditionals);
  TestEqual(TEXT("Ensure that conditionals were correctly removed"), Conditionals.Num(), 0);

  // Finally test that setting the conditional values has no impact on the group.
  Test(ConditionalGroupAll, "All", true, true, true, __LINE__);

  return true;
}
