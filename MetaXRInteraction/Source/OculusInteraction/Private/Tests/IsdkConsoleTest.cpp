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

#include "IsdkConsoleTest.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FIsdkConsoleTest,
    "InteractionSDK.OculusInteraction.Source.OculusInteraction.Private.Tests.FIsdkConsoleTest.All",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FIsdkConsoleTest::RunTest(const FString& Parameters)
{
  UIsdkConsoleParser::Init();
  UIsdkMockConsoleReceiver* rcvr = NewObject<UIsdkMockConsoleReceiver>();
  rcvr->BeginPlay();

  TestEqual(TEXT("Verify console receiver has not received strings"), rcvr->LastArgs.Num(), 0);

  FString teststr = TEXT("I'm a string");
  TArray<FString> commands{{TEXT("test0"), teststr}};
  UIsdkConsoleParser::ParseConsoleCommand(commands, nullptr);
  TestEqual(TEXT("Console string value"), UIsdkConsoleParser::GetFStringValue("test0"), teststr);

  TestEqual(TEXT("Console receiver test new arg count"), rcvr->LastArgs.Num(), 2);
  TestEqual(TEXT("Console receiver test matching command"), rcvr->LastArgs[0], commands[0]);

  commands = {TEXT("test1"), TEXT("42")};
  UIsdkConsoleParser::ParseConsoleCommand(commands, nullptr);
  TestEqual(TEXT("Console integer value"), UIsdkConsoleParser::GetIntValue("test1"), 42);

  commands = {TEXT("test2"), TEXT("true")};
  UIsdkConsoleParser::ParseConsoleCommand(commands, nullptr);
  TestEqual(TEXT("Console bool value"), UIsdkConsoleParser::GetBoolValue("test2"), true);

  rcvr->EndPlay();
  return true;
}
