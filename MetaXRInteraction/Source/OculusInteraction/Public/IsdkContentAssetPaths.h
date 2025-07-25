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

class IsdkContentAssetPaths
{
 public:
  struct Configurations
  {
    static constexpr auto PokeInteractablePanelConfig =
        TEXT("/OculusInteraction/Configurations/IsdkPokeInteractablePanelConfig");
  };
  struct Engine
  {
    struct BasicShapes
    {
      static constexpr auto Plane = TEXT("/Engine/BasicShapes/Plane");
      static constexpr auto Cube = TEXT("/Engine/BasicShapes/Cube");
    };
  };
  struct Audio
  {
    static constexpr auto Interaction_BasicRay_Press =
        TEXT("/OculusInteraction/Audio/Interaction_BasicRay_Press");
    static constexpr auto Interaction_BasicPoke_ButtonPress =
        TEXT("/OculusInteraction/Audio/Interaction_BasicPoke_ButtonPress");
  };
  struct Inputs
  {
    static constexpr auto IA_Right_HandPinchSelectStrength =
        TEXT("/OculusInteraction/Inputs/IA_Right_HandPinchSelectStrength");
    static constexpr auto IA_Right_HandPinchSelect =
        TEXT("/OculusInteraction/Inputs/IA_Right_HandPinchSelect");
    static constexpr auto IA_Left_HandPinchSelectStrength =
        TEXT("/OculusInteraction/Inputs/IA_Left_HandPinchSelectStrength");
    static constexpr auto IA_Left_HandPinchSelect =
        TEXT("/OculusInteraction/Inputs/IA_Left_HandPinchSelect");
    static constexpr auto IA_Right_ControllerSelectStrength =
        TEXT("/OculusInteraction/Inputs/IA_Right_ControllerSelectStrength");
    static constexpr auto IA_Right_ControllerSelect =
        TEXT("/OculusInteraction/Inputs/IA_Right_ControllerSelect");
    static constexpr auto IA_Left_ControllerSelectStrength =
        TEXT("/OculusInteraction/Inputs/IA_Left_ControllerSelectStrength");
    static constexpr auto IA_Left_ControllerSelect =
        TEXT("/OculusInteraction/Inputs/IA_Left_ControllerSelect");
    static constexpr auto IA_Left_HandPinchGrab =
        TEXT("/OculusInteraction/Inputs/IA_Left_HandPinchGrab");
    static constexpr auto IA_Right_HandPinchGrab =
        TEXT("/OculusInteraction/Inputs/IA_Right_HandPinchGrab");
    static constexpr auto IA_Left_HandPalmGrab =
        TEXT("/OculusInteraction/Inputs/IA_Left_HandPalmGrab");
    static constexpr auto IA_Right_HandPalmGrab =
        TEXT("/OculusInteraction/Inputs/IA_Right_HandPalmGrab");
    static constexpr auto IA_AButton_Down = TEXT("/OculusInteraction/Inputs/IA_AButton_Down");
    static constexpr auto IA_BButton_Down = TEXT("/OculusInteraction/Inputs/IA_BButton_Down");
    static constexpr auto IA_XButton_Down = TEXT("/OculusInteraction/Inputs/IA_XButton_Down");
    static constexpr auto IA_YButton_Down = TEXT("/OculusInteraction/Inputs/IA_YButton_Down");
    static constexpr auto IA_AButton_Touched = TEXT("/OculusInteraction/Inputs/IA_AButton_Touched");
    static constexpr auto IA_BButton_Touched = TEXT("/OculusInteraction/Inputs/IA_BButton_Touched");
    static constexpr auto IA_XButton_Touched = TEXT("/OculusInteraction/Inputs/IA_XButton_Touched");
    static constexpr auto IA_YButton_Touched = TEXT("/OculusInteraction/Inputs/IA_YButton_Touched");
    static constexpr auto IA_Left_MenuButton_Down =
        TEXT("/OculusInteraction/Inputs/IA_Left_MenuButton_Down");
    static constexpr auto IA_Left_FrontTrigger_Touched =
        TEXT("/OculusInteraction/Inputs/IA_Left_FrontTrigger_Touched");
    static constexpr auto IA_Left_FrontTrigger_Axis =
        TEXT("/OculusInteraction/Inputs/IA_Left_FrontTrigger_Axis");
    static constexpr auto IA_Right_FrontTrigger_Touched =
        TEXT("/OculusInteraction/Inputs/IA_Right_FrontTrigger_Touched");
    static constexpr auto IA_Right_FrontTrigger_Axis =
        TEXT("/OculusInteraction/Inputs/IA_Right_FrontTrigger_Axis");
    static constexpr auto IA_Left_GripTrigger_Axis =
        TEXT("/OculusInteraction/Inputs/IA_Left_GripTrigger_Axis");
    static constexpr auto IA_Right_GripTrigger_Axis =
        TEXT("/OculusInteraction/Inputs/IA_Right_GripTrigger_Axis");
    static constexpr auto IA_Left_Thumbstick_Touched =
        TEXT("/OculusInteraction/Inputs/IA_Left_Thumbstick_Touched");
    static constexpr auto IA_Left_Thumbstick_XAxis =
        TEXT("/OculusInteraction/Inputs/IA_Left_Thumbstick_XAxis");
    static constexpr auto IA_Left_Thumbstick_YAxis =
        TEXT("/OculusInteraction/Inputs/IA_Left_Thumbstick_YAxis");
    static constexpr auto IA_Right_Thumbstick_Touched =
        TEXT("/OculusInteraction/Inputs/IA_Right_Thumbstick_Touched");
    static constexpr auto IA_Right_Thumbstick_XAxis =
        TEXT("/OculusInteraction/Inputs/IA_Right_Thumbstick_XAxis");
    static constexpr auto IA_Right_Thumbstick_YAxis =
        TEXT("/OculusInteraction/Inputs/IA_Right_Thumbstick_YAxis");
    static constexpr auto IA_Left_Panel_Touched =
        TEXT("/OculusInteraction/Inputs/IA_Left_Panel_Touched");
    static constexpr auto IA_Right_Panel_Touched =
        TEXT("/OculusInteraction/Inputs/IA_Right_Panel_Touched");
  };
  struct Materials
  {
    static constexpr auto TextUnlit = TEXT("/OculusInteraction/Materials/TextUnlit");
    static constexpr auto RoundedBoxUnlit = TEXT("/OculusInteraction/Materials/RoundedBoxUnlit");
    static constexpr auto RoundedBoxUnlit_TwoSided =
        TEXT("/OculusInteraction/Materials/RoundedBoxUnlit_TwoSided");
    static constexpr auto PinchArrow = TEXT("/OculusInteraction/Materials/PinchArrowMaterial");
    static constexpr auto Cursor = TEXT("/OculusInteraction/Materials/CursorMaterial");
    static constexpr auto Widget3DRoundedBox_Masked =
        TEXT("/OculusInteraction/Materials/Widget3DRoundedBox");
    static constexpr auto Widget3DRoundedBox_Masked_OneSided =
        TEXT("/OculusInteraction/Materials/Widget3DRoundedBox_Masked_OneSided");
    static constexpr auto Widget3DRoundedBox_Transparent =
        TEXT("/OculusInteraction/Materials/Widget3DRoundedBox_Transparent");
    static constexpr auto Widget3DRoundedBox_Transparent_OneSided =
        TEXT("/OculusInteraction/Materials/Widget3DRoundedBox_Transparent_OneSided");
    static constexpr auto GrabbableDefault = TEXT("/OculusInteraction/Materials/GrabbableLit");
  };
  struct Models
  {
    struct RoundedButton
    {
      static constexpr auto DefaultTransitionCurve =
          TEXT("/OculusInteraction/Models/RoundedButton/DefaultTransitionCurve");
    };
    struct RayVisual
    {
      static constexpr auto OculusHandPinchArrowBlended =
          TEXT("/OculusInteraction/Models/RayVisual/OculusHandPinchArrowBlended");
      static constexpr auto PinchStrengthCurve =
          TEXT("/OculusInteraction/Models/RayVisual/PinchStrengthCurve");
    };
    struct Hand
    {
      static constexpr auto OpenXRLeftHand =
          TEXT("/OculusInteraction/Models/Hands/SK_OpenXRHand_Left");
      static constexpr auto OpenXRRightHand =
          TEXT("/OculusInteraction/Models/Hands/SK_OpenXRHand_Right");
      static constexpr auto OculusHandMaterial =
          TEXT("/OculusInteraction/Materials/Hands/M_Hand_Translucent");
      static constexpr auto OculusHandTestMaterial =
          TEXT("/OculusInteraction/Tools/TestHandMaterial");
    };
    struct Controller
    {
      static constexpr auto OculusControllerSkeletalMeshL =
          TEXT("/OculusInteraction/Models/Controllers/MetaQuestPro/SK_MetaQuestPro_Left");
      static constexpr auto OculusControllerSkeletalMeshR =
          TEXT("/OculusInteraction/Models/Controllers/MetaQuestPro/SK_MetaQuestPro_Right");
      static constexpr auto OculusControllerAnimBlueprintL = TEXT(
          "/OculusInteraction/Models/Controllers/MetaQuestPro/Animations/ABP_MetaQuestPro_Left");
      static constexpr auto OculusControllerAnimBlueprintR = TEXT(
          "/OculusInteraction/Models/Controllers/MetaQuestPro/Animations/ABP_MetaQuestPro_Right");
    };
  };
  struct ControllerDrivenHands
  {
    static constexpr auto ControllerDrivenHandAnimBlueprintL =
        TEXT("/OculusInteraction/Models/Hands/Animations/ABP_ControllerDrivenHand_Left");
    static constexpr auto ControllerDrivenHandAnimBlueprintR =
        TEXT("/OculusInteraction/Models/Hands/Animations/ABP_ControllerDrivenHand_Right");
  };
};
