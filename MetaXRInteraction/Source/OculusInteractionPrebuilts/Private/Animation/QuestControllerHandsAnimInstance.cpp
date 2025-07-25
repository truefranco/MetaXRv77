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

#include "Animation/QuestControllerHandsAnimInstance.h"

void UQuestControllerHandsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
  Super::NativeUpdateAnimation(DeltaSeconds);

  // The main idea here is that we interpolate a bunch of Alpha values from 0-1 based on some
  // conditional.  The value generally represents whether the corresponding finger (thumb or index)
  // is touching or pressing the button.  A value of 0 means not touching, 1 means touching, and
  // anything in between means that it is transitioning to / away from touching the button.
  // Note that there's some nuance in the way the conditionals are set up and later used in
  // the animation blueprint, mostly surrounding where the thumb should be if multiple inputs are
  // pressed.  I recommend taking a look at the animation blueprints first if you are thinking of
  // modifying this code.

  UpdateAlpha(
      DeltaSeconds, AButtonThumbAlpha, (bAButtonTouched || bAButtonDown) && !bBButtonTouched);
  UpdateAlpha(DeltaSeconds, BButtonThumbAlpha, bBButtonTouched || bBButtonDown);
  UpdateAlpha(
      DeltaSeconds, XButtonThumbAlpha, (bXButtonTouched || bXButtonDown) && !bYButtonTouched);
  UpdateAlpha(DeltaSeconds, YButtonThumbAlpha, bYButtonTouched || bYButtonDown);
  UpdateAlpha(
      DeltaSeconds,
      LeftFrontTriggerAlpha,
      bLeftFrontTriggerTouched || LeftFrontTriggerAxisValue > TriggerDeadZone);
  UpdateAlpha(
      DeltaSeconds,
      RightFrontTriggerAlpha,
      bRightFrontTriggerTouched || RightFrontTriggerAxisValue > TriggerDeadZone);
  const bool bLeftThumbstickNonZero =
      FVector2D(LeftThumbstickXAxisValue, LeftThumbstickYAxisValue).Length() > TriggerDeadZone;
  UpdateAlpha(DeltaSeconds, LeftThumbstickAlpha, bLeftThumbstickTouched || bLeftThumbstickNonZero);
  const bool bRightThumbstickNonZero =
      FVector2D(RightThumbstickXAxisValue, RightThumbstickYAxisValue).Length() > TriggerDeadZone;
  UpdateAlpha(
      DeltaSeconds, RightThumbstickAlpha, bRightThumbstickTouched || bRightThumbstickNonZero);
  UpdateAlpha(DeltaSeconds, LeftPanelThumbAlpha, bLeftPanelTouched);
  UpdateAlpha(DeltaSeconds, RightPanelThumbAlpha, bRightPanelTouched);
  UpdateAlpha(DeltaSeconds, LeftMenuThumbAlpha, bLeftMenuButtonDown);
}

void UQuestControllerHandsAnimInstance::UpdateAlpha(
    float DeltaSeconds,
    float& InOutAlpha,
    bool bInterpolateTarget) const
{
  const float Target = bInterpolateTarget ? 1.f : 0.f;
  InOutAlpha = FMath::FInterpTo(InOutAlpha, Target, DeltaSeconds, InterpolationSpeed);
}
