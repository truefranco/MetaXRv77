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

#include "Animation/QuestControllerAnimInstance.h"

bool UQuestControllerAnimInstance::IsAButtonDown() const
{
  return bAButtonDown;
}

void UQuestControllerAnimInstance::SetAButtonDown(bool bInAButtonDown)
{
  bAButtonDown = bInAButtonDown;
}

bool UQuestControllerAnimInstance::IsBButtonDown() const
{
  return bBButtonDown;
}

void UQuestControllerAnimInstance::SetBButtonDown(bool bInBButtonDown)
{
  bBButtonDown = bInBButtonDown;
}

bool UQuestControllerAnimInstance::IsXButtonDown() const
{
  return bXButtonDown;
}

void UQuestControllerAnimInstance::SetXButtonDown(bool bInXButtonDown)
{
  bXButtonDown = bInXButtonDown;
}

bool UQuestControllerAnimInstance::IsYButtonDown() const
{
  return bYButtonDown;
}

void UQuestControllerAnimInstance::SetYButtonDown(bool bInYButtonDown)
{
  bYButtonDown = bInYButtonDown;
}

bool UQuestControllerAnimInstance::IsLeftMenuButtonDown() const
{
  return bLeftMenuButtonDown;
}

void UQuestControllerAnimInstance::SetLeftMenuButtonDown(bool bInLeftMenuButtonDown)
{
  bLeftMenuButtonDown = bInLeftMenuButtonDown;
}

bool UQuestControllerAnimInstance::IsAButtonTouched() const
{
  return bAButtonTouched;
}

void UQuestControllerAnimInstance::SetAButtonTouched(bool bInAButtonTouched)
{
  bAButtonTouched = bInAButtonTouched;
}

bool UQuestControllerAnimInstance::IsBButtonTouched() const
{
  return bBButtonTouched;
}

void UQuestControllerAnimInstance::SetBButtonTouched(bool bInBButtonTouched)
{
  bBButtonTouched = bInBButtonTouched;
}

bool UQuestControllerAnimInstance::IsXButtonTouched() const
{
  return bXButtonTouched;
}

void UQuestControllerAnimInstance::SetXButtonTouched(bool bInXButtonTouched)
{
  bXButtonTouched = bInXButtonTouched;
}

bool UQuestControllerAnimInstance::IsYButtonTouched() const
{
  return bYButtonTouched;
}

void UQuestControllerAnimInstance::SetYButtonTouched(bool bInYButtonTouched)
{
  bYButtonTouched = bInYButtonTouched;
}

bool UQuestControllerAnimInstance::IsLeftFrontTriggerTouched() const
{
  return bLeftFrontTriggerTouched;
}

void UQuestControllerAnimInstance::SetLeftFrontTriggerTouched(bool bInLeftFrontTriggerTouched)
{
  bLeftFrontTriggerTouched = bInLeftFrontTriggerTouched;
}

float UQuestControllerAnimInstance::GetLeftFrontTriggerAxisValue() const
{
  return LeftFrontTriggerAxisValue;
}

void UQuestControllerAnimInstance::SetLeftFrontTriggerAxisValue(float InLeftFrontTriggerAxisValue)
{
  LeftFrontTriggerAxisValue = InLeftFrontTriggerAxisValue;
}

bool UQuestControllerAnimInstance::IsRightFrontTriggerTouched() const
{
  return bRightFrontTriggerTouched;
}

void UQuestControllerAnimInstance::SetRightFrontTriggerTouched(bool bInRightFrontTriggerTouched)
{
  bRightFrontTriggerTouched = bInRightFrontTriggerTouched;
}

float UQuestControllerAnimInstance::GetRightFrontTriggerAxisValue() const
{
  return RightFrontTriggerAxisValue;
}

void UQuestControllerAnimInstance::SetRightFrontTriggerAxisValue(float InRightFrontTriggerAxisValue)
{
  RightFrontTriggerAxisValue = InRightFrontTriggerAxisValue;
}

float UQuestControllerAnimInstance::GetLeftGripTriggerAxisValue() const
{
  return LeftGripTriggerAxisValue;
}

void UQuestControllerAnimInstance::SetLeftGripTriggerAxisValue(float InLeftGripTriggerAxisValue)
{
  LeftGripTriggerAxisValue = InLeftGripTriggerAxisValue;
}

float UQuestControllerAnimInstance::GetRightGripTriggerAxisValue() const
{
  return RightGripTriggerAxisValue;
}

void UQuestControllerAnimInstance::SetRightGripTriggerAxisValue(float InRightGripTriggerAxisValue)
{
  RightGripTriggerAxisValue = InRightGripTriggerAxisValue;
}

bool UQuestControllerAnimInstance::GetLeftThumbstickTouched() const
{
  return bLeftThumbstickTouched;
}

void UQuestControllerAnimInstance::SetLeftThumbstickTouched(bool bInLeftThumbstickTouched)
{
  bLeftThumbstickTouched = bInLeftThumbstickTouched;
}

float UQuestControllerAnimInstance::GetLeftThumbstickXAxisValue() const
{
  return LeftThumbstickXAxisValue;
}

void UQuestControllerAnimInstance::SetLeftThumbstickXAxisValue(float InLeftThumbstickXAxisValue)
{
  LeftThumbstickXAxisValue = InLeftThumbstickXAxisValue;
}

float UQuestControllerAnimInstance::GetLeftThumbstickYAxisValue() const
{
  return LeftThumbstickYAxisValue;
}

void UQuestControllerAnimInstance::SetLeftThumbstickYAxisValue(float InLeftThumbstickYAxisValue)
{
  LeftThumbstickYAxisValue = InLeftThumbstickYAxisValue;
}

bool UQuestControllerAnimInstance::GetRightThumbstickTouched() const
{
  return bRightThumbstickTouched;
}

void UQuestControllerAnimInstance::SetRightThumbstickTouched(bool bInRightThumbstickTouched)
{
  bRightThumbstickTouched = bInRightThumbstickTouched;
}

float UQuestControllerAnimInstance::GetRightThumbstickXAxisValue() const
{
  return RightThumbstickXAxisValue;
}

void UQuestControllerAnimInstance::SetRightThumbstickXAxisValue(float InRightThumbstickXAxisValue)
{
  RightThumbstickXAxisValue = InRightThumbstickXAxisValue;
}

float UQuestControllerAnimInstance::GetRightThumbstickYAxisValue() const
{
  return RightThumbstickYAxisValue;
}

void UQuestControllerAnimInstance::SetRightThumbstickYAxisValue(float InRightThumbstickYAxisValue)
{
  RightThumbstickYAxisValue = InRightThumbstickYAxisValue;
}

bool UQuestControllerAnimInstance::IsLeftPanelTouched() const
{
  return bLeftPanelTouched;
}

void UQuestControllerAnimInstance::SetLeftPanelTouched(bool InLeftPanelTouched)
{
  bLeftPanelTouched = InLeftPanelTouched;
}

bool UQuestControllerAnimInstance::IsRightPanelTouched() const
{
  return bRightPanelTouched;
}

void UQuestControllerAnimInstance::SetRightPanelTouched(bool InRightPanelTouched)
{
  bRightPanelTouched = InRightPanelTouched;
}
