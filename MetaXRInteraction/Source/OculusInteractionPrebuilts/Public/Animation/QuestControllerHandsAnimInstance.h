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
#include "QuestControllerAnimInstance.h"
#include "QuestControllerHandsAnimInstance.generated.h"

/**
 * UQuestControllerHandsAnimInstance extends UQuestControllerAnimInstance to provide
 * some basic values intended to drive the animation of hands paired with a controller
 * mesh.
 */
UCLASS()
class OCULUSINTERACTIONPREBUILTS_API UQuestControllerHandsAnimInstance
    : public UQuestControllerAnimInstance
{
  GENERATED_BODY()

  virtual void NativeUpdateAnimation(float DeltaSeconds) override;

 protected:
  // The speed at which fingers and thumbs should interpolate toward the button
  // they are touching or pressing, toward a maximum of 1, and a minimum of 0.
  // A larger number interpolates faster.
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractionSDK)
  float InterpolationSpeed = 20.f;

  // The amount of time to blend between a "touched" and "down" state for most
  // button press animations.
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractionSDK)
  float ButtonPressBlendTime = 0.05f;

  // For axial inputs, the range after which we will consider the input "touched".
  // We use this value in conjunction with the corresponding "touched" boolean value.
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractionSDK)
  float TriggerDeadZone = 0.05f;

  // A 0-1 value indicating how much the thumb is positioned over the A button.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float AButtonThumbAlpha = 0.f;

  // A 0-1 value indicating how much the thumb is positioned over the B button.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float BButtonThumbAlpha = 0.f;

  // A 0-1 value indicating how much the thumb is positioned over the X button.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float XButtonThumbAlpha = 0.f;

  // A 0-1 value indicating how much the thumb is positioned over the Y button.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float YButtonThumbAlpha = 0.f;

  // A 0-1 value indicating how much the finger is positioned over the Left Front Trigger.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float LeftFrontTriggerAlpha = 0.f;

  // A 0-1 value indicating how much the finger is positioned over the Right Front Trigger.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float RightFrontTriggerAlpha = 0.f;

  // A 0-1 value indicating how much the finger is positioned over the Left Thumbstick.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float LeftThumbstickAlpha = 0.f;

  // A 0-1 value indicating how much the finger is positioned over the Right Thumbstick.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float RightThumbstickAlpha = 0.f;

  // A 0-1 value indicating how much the finger is positioned over the Left Panel.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float LeftPanelThumbAlpha = 0.f;

  // A 0-1 value indicating how much the finger is positioned over the Right Panel.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float RightPanelThumbAlpha = 0.f;

  // A 0-1 value indicating how much the finger is positioned over the Left Menu Button.
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  float LeftMenuThumbAlpha = 0.f;

 private:
  // A simple helper function for updating the value of an "Alpha" property.
  void UpdateAlpha(float DeltaSeconds, float& InOutAlpha, bool bInterpolateTarget) const;
};
