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

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QuestControllerAnimInstance.generated.h"

/**
 * UQuestControllerAnimInstance provides a set of properties corresponding to quest controller
 * inputs (buttons, triggers, etc), used to drive controller-based animations.
 *
 * The intent is to provide a c++ facing class to drive controller and hand animation behavior
 * in a reusable way.  This allows us to wire up all the inputs to the anim instance, and let
 * the blueprint decide what it will do with them.
 */
UCLASS(Blueprintable)
class OCULUSINTERACTIONPREBUILTS_API UQuestControllerAnimInstance : public UAnimInstance
{
  GENERATED_BODY()

 public:
  bool IsAButtonDown() const;
  void SetAButtonDown(bool bInAButtonDown);

  bool IsBButtonDown() const;
  void SetBButtonDown(bool bInBButtonDown);

  bool IsXButtonDown() const;
  void SetXButtonDown(bool bInXButtonDown);

  bool IsYButtonDown() const;
  void SetYButtonDown(bool bInYButtonDown);

  bool IsLeftMenuButtonDown() const;
  void SetLeftMenuButtonDown(bool bInLeftMenuButtonDown);

  bool IsAButtonTouched() const;
  void SetAButtonTouched(bool bInAButtonTouched);

  bool IsBButtonTouched() const;
  void SetBButtonTouched(bool bInBButtonTouched);

  bool IsXButtonTouched() const;
  void SetXButtonTouched(bool bInXButtonTouched);

  bool IsYButtonTouched() const;
  void SetYButtonTouched(bool bInYButtonTouched);

  bool IsLeftFrontTriggerTouched() const;
  void SetLeftFrontTriggerTouched(bool bInLeftFrontTriggerTouched);

  float GetLeftFrontTriggerAxisValue() const;
  void SetLeftFrontTriggerAxisValue(float InLeftFrontTriggerAxisValue);

  bool IsRightFrontTriggerTouched() const;
  void SetRightFrontTriggerTouched(bool bInRightFrontTriggerTouched);

  float GetRightFrontTriggerAxisValue() const;
  void SetRightFrontTriggerAxisValue(float InRightFrontTriggerAxisValue);

  float GetLeftGripTriggerAxisValue() const;
  void SetLeftGripTriggerAxisValue(float InLeftGripTriggerAxisValue);

  float GetRightGripTriggerAxisValue() const;
  void SetRightGripTriggerAxisValue(float InRightGripTriggerAxisValue);

  bool GetLeftThumbstickTouched() const;
  void SetLeftThumbstickTouched(bool bInLeftThumbstickTouched);

  float GetLeftThumbstickXAxisValue() const;
  void SetLeftThumbstickXAxisValue(float InLeftThumbstickXAxisValue);

  float GetLeftThumbstickYAxisValue() const;
  void SetLeftThumbstickYAxisValue(float InLeftThumbstickYAxisValue);

  bool GetRightThumbstickTouched() const;
  void SetRightThumbstickTouched(bool bInRightThumbstickTouched);

  float GetRightThumbstickXAxisValue() const;
  void SetRightThumbstickXAxisValue(float InRightThumbstickXAxisValue);

  float GetRightThumbstickYAxisValue() const;
  void SetRightThumbstickYAxisValue(float InRightThumbstickYAxisValue);

  bool IsLeftPanelTouched() const;
  void SetLeftPanelTouched(bool InLeftPanelTouched);

  bool IsRightPanelTouched() const;
  void SetRightPanelTouched(bool InRightPanelTouched);

 protected:
  // Is the A Button currently in a Down state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bAButtonDown = false;

  // Is the B Button currently in a Down state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bBButtonDown = false;

  // Is the X Button currently in a Down state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bXButtonDown = false;

  // Is the Y Button currently in a Down state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bYButtonDown = false;

  // Is the A Button currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bAButtonTouched = false;

  // Is the B Button currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bBButtonTouched = false;

  // Is the X Button currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bXButtonTouched = false;

  // Is the Y Button currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bYButtonTouched = false;

  // Is the Left Menu Button currently in a Down state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bLeftMenuButtonDown = false;

  // Is the Left Front Trigger currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bLeftFrontTriggerTouched = false;

  // A 0-1 value indicating how far down the Left Front Trigger is pressed.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  float LeftFrontTriggerAxisValue = 0.f;

  // Is the Right Front Trigger currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bRightFrontTriggerTouched = false;

  // A 0-1 value indicating how far down the Right Front Trigger is pressed.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  float RightFrontTriggerAxisValue = 0.f;

  // A 0-1 value indicating how far down the Left Grip Trigger is pressed.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  float LeftGripTriggerAxisValue = 0.f;

  // A 0-1 value indicating how far down the Right Grip Trigger is pressed.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  float RightGripTriggerAxisValue = 0.f;

  // Is the Left Thumbstick currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bLeftThumbstickTouched = false;

  // A -1 to 1 value indicating how far to the left (-1) or right (1)
  // the Left Thumbstick is pressed.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  float LeftThumbstickXAxisValue = 0.f;

  // A -1 to 1 value indicating how far up (-1) or down (1)
  // the Left Thumbstick is pressed.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  float LeftThumbstickYAxisValue = 0.f;

  // Is the Left Thumbstick currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bRightThumbstickTouched = false;

  // A -1 to 1 value indicating how far to the left (-1) or right (1)
  // the Right Thumbstick is pressed.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  float RightThumbstickXAxisValue = 0.f;

  // A -1 to 1 value indicating how far up (-1) or down (1)
  // the Right Thumbstick is pressed.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  float RightThumbstickYAxisValue = 0.f;

  // Is the Left Panel currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bLeftPanelTouched = false;

  // Is the Right Panel currently in a Touched state
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controller)
  bool bRightPanelTouched = false;
};
