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

#include "Rig/IsdkInputActionsRigComponent.h"

#include "InputAction.h"
#include "IsdkHandData.h"
#include "IsdkContentAssetPaths.h"
#include "Misc/UObjectToken.h"
#include "UObject/ConstructorHelpers.h"
#include "Logging/MessageLog.h"

UIsdkInputActionsRigComponent::UIsdkInputActionsRigComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UIsdkInputActionsRigComponent::SetSubobjectPropertyDefaults(EIsdkHandedness InHandedness)
{
  struct FStaticObjectFinders
  {
    // Hand Actions
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftHandPinchSelectAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightHandPinchSelectAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftHandPinchSelectStrengthAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightHandPinchSelectStrengthAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftHandPinchGrabAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightHandPinchGrabAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftHandPalmGrabAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightHandPalmGrabAction;

    // Controller Animation Actions
    ConstructorHelpers::FObjectFinderOptional<UInputAction> AButtonDownAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> BButtonDownAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> XButtonDownAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> YButtonDownAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> AButtonTouchAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> BButtonTouchAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> XButtonTouchAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> YButtonTouchAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftMenuButtonDownAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftFrontTriggerTouchedAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftFrontTriggerAxisAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightFrontTriggerTouchedAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightFrontTriggerAxisAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftGripTriggerAxisAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightGripTriggerAxisAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftThumbstickTouchedAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftThumbstickXAxisAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftThumbstickYAxisAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightThumbstickTouchedAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightThumbstickXAxisAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightThumbstickYAxisAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> LeftPanelTouchedAction;
    ConstructorHelpers::FObjectFinderOptional<UInputAction> RightPanelTouchedAction;

    FStaticObjectFinders()
        : LeftHandPinchSelectAction(IsdkContentAssetPaths::Inputs::IA_Left_HandPinchSelect),
          RightHandPinchSelectAction(IsdkContentAssetPaths::Inputs::IA_Right_HandPinchSelect),
          LeftHandPinchSelectStrengthAction(
              IsdkContentAssetPaths::Inputs::IA_Left_HandPinchSelectStrength),
          RightHandPinchSelectStrengthAction(
              IsdkContentAssetPaths::Inputs::IA_Right_HandPinchSelectStrength),
          LeftHandPinchGrabAction(IsdkContentAssetPaths::Inputs::IA_Left_HandPinchGrab),
          RightHandPinchGrabAction(IsdkContentAssetPaths::Inputs::IA_Right_HandPinchGrab),
          LeftHandPalmGrabAction(IsdkContentAssetPaths::Inputs::IA_Left_HandPalmGrab),
          RightHandPalmGrabAction(IsdkContentAssetPaths::Inputs::IA_Right_HandPalmGrab),
          AButtonDownAction(IsdkContentAssetPaths::Inputs::IA_AButton_Down),
          BButtonDownAction(IsdkContentAssetPaths::Inputs::IA_BButton_Down),
          XButtonDownAction(IsdkContentAssetPaths::Inputs::IA_XButton_Down),
          YButtonDownAction(IsdkContentAssetPaths::Inputs::IA_YButton_Down),
          AButtonTouchAction(IsdkContentAssetPaths::Inputs::IA_AButton_Touched),
          BButtonTouchAction(IsdkContentAssetPaths::Inputs::IA_BButton_Touched),
          XButtonTouchAction(IsdkContentAssetPaths::Inputs::IA_XButton_Touched),
          YButtonTouchAction(IsdkContentAssetPaths::Inputs::IA_YButton_Touched),
          LeftMenuButtonDownAction(IsdkContentAssetPaths::Inputs::IA_Left_MenuButton_Down),
          LeftFrontTriggerTouchedAction(
              IsdkContentAssetPaths::Inputs::IA_Left_FrontTrigger_Touched),
          LeftFrontTriggerAxisAction(IsdkContentAssetPaths::Inputs::IA_Left_FrontTrigger_Axis),
          RightFrontTriggerTouchedAction(
              IsdkContentAssetPaths::Inputs::IA_Right_FrontTrigger_Touched),
          RightFrontTriggerAxisAction(IsdkContentAssetPaths::Inputs::IA_Right_FrontTrigger_Axis),
          LeftGripTriggerAxisAction(IsdkContentAssetPaths::Inputs::IA_Left_GripTrigger_Axis),
          RightGripTriggerAxisAction(IsdkContentAssetPaths::Inputs::IA_Right_GripTrigger_Axis),
          LeftThumbstickTouchedAction(IsdkContentAssetPaths::Inputs::IA_Left_Thumbstick_Touched),
          LeftThumbstickXAxisAction(IsdkContentAssetPaths::Inputs::IA_Left_Thumbstick_XAxis),
          LeftThumbstickYAxisAction(IsdkContentAssetPaths::Inputs::IA_Left_Thumbstick_YAxis),
          RightThumbstickTouchedAction(IsdkContentAssetPaths::Inputs::IA_Right_Thumbstick_Touched),
          RightThumbstickXAxisAction(IsdkContentAssetPaths::Inputs::IA_Right_Thumbstick_XAxis),
          RightThumbstickYAxisAction(IsdkContentAssetPaths::Inputs::IA_Right_Thumbstick_YAxis),
          LeftPanelTouchedAction(IsdkContentAssetPaths::Inputs::IA_Left_Panel_Touched),
          RightPanelTouchedAction(IsdkContentAssetPaths::Inputs::IA_Right_Panel_Touched)
    {
    }
  };
  static FStaticObjectFinders StaticObjectFinders;
  if (InHandedness == EIsdkHandedness::Left)
  {
    SelectAction = StaticObjectFinders.LeftHandPinchSelectAction.Get();
    SelectStrengthAction = StaticObjectFinders.LeftHandPinchSelectStrengthAction.Get();
    PinchGrabAction = StaticObjectFinders.LeftHandPinchGrabAction.Get();
    PalmGrabAction = StaticObjectFinders.LeftHandPalmGrabAction.Get();
  }
  else
  {
    SelectAction = StaticObjectFinders.RightHandPinchSelectAction.Get();
    SelectStrengthAction = StaticObjectFinders.RightHandPinchSelectStrengthAction.Get();
    PinchGrabAction = StaticObjectFinders.RightHandPinchGrabAction.Get();
    PalmGrabAction = StaticObjectFinders.RightHandPalmGrabAction.Get();
  }

  AButtonDownAction = StaticObjectFinders.AButtonDownAction.Get();
  BButtonDownAction = StaticObjectFinders.BButtonDownAction.Get();
  XButtonDownAction = StaticObjectFinders.XButtonDownAction.Get();
  YButtonDownAction = StaticObjectFinders.YButtonDownAction.Get();
  AButtonTouchedAction = StaticObjectFinders.AButtonTouchAction.Get();
  BButtonTouchedAction = StaticObjectFinders.BButtonTouchAction.Get();
  XButtonTouchedAction = StaticObjectFinders.XButtonTouchAction.Get();
  YButtonTouchedAction = StaticObjectFinders.YButtonTouchAction.Get();
  LeftMenuButtonDownAction = StaticObjectFinders.LeftMenuButtonDownAction.Get();
  LeftFrontTriggerTouchedAction = StaticObjectFinders.LeftFrontTriggerTouchedAction.Get();
  LeftFrontTriggerAxisAction = StaticObjectFinders.LeftFrontTriggerAxisAction.Get();
  RightFrontTriggerTouchedAction = StaticObjectFinders.RightFrontTriggerTouchedAction.Get();
  RightFrontTriggerAxisAction = StaticObjectFinders.RightFrontTriggerAxisAction.Get();
  LeftGripTriggerAxisAction = StaticObjectFinders.LeftGripTriggerAxisAction.Get();
  RightGripTriggerAxisAction = StaticObjectFinders.RightGripTriggerAxisAction.Get();
  LeftThumbstickTouchedAction = StaticObjectFinders.LeftThumbstickTouchedAction.Get();
  LeftThumbstickXAxisAction = StaticObjectFinders.LeftThumbstickXAxisAction.Get();
  LeftThumbstickYAxisAction = StaticObjectFinders.LeftThumbstickYAxisAction.Get();
  RightThumbstickTouchedAction = StaticObjectFinders.RightThumbstickTouchedAction.Get();
  RightThumbstickXAxisAction = StaticObjectFinders.RightThumbstickXAxisAction.Get();
  RightThumbstickYAxisAction = StaticObjectFinders.RightThumbstickYAxisAction.Get();
  LeftPanelTouchedAction = StaticObjectFinders.LeftPanelTouchedAction.Get();
  RightPanelTouchedAction = StaticObjectFinders.RightPanelTouchedAction.Get();
}

#if WITH_EDITOR
void UIsdkInputActionsRigComponent::CheckForErrors()
{
  Super::CheckForErrors();

#define LOCTEXT_NAMESPACE "InteractionSDK"
  if (!IsValid(SelectStrengthAction))
  {
    FMessageLog("MapCheck")
        .Warning()
        ->AddToken(FUObjectToken::Create(this))
        ->AddToken(FTextToken::Create(LOCTEXT(
            "MapCheck_Message_IsdkInputActionsRigComponent",
            "SelectStrengthAction field must be set.")));
  }
#undef LOCTEXT_NAMESPACE
}
#endif
