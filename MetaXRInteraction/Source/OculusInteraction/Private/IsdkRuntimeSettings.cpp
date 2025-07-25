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

#include "IsdkRuntimeSettings.h"

// Defining our native Gameplay Tags

namespace IsdkGameplayTags
{
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Interactable_Ray,
    "Isdk.Type.Interactable.Ray",
    "Identifies this Component as a Ray Interactable");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Interactable_Grab,
    "Isdk.Type.Interactable.Grab",
    "Identifies this Component as a Grab Interactable");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Interactable_Poke,
    "Isdk.Type.Interactable.Poke",
    "Identifies this Component as a Poke Interactable");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Interactor_Ray,
    "Isdk.Type.Interactor.Ray",
    "Identifies this Component as a Ray Interactor");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Interactor_Grab,
    "Isdk.Type.Interactor.Grab",
    "Identifies this Component as a Grab Interactor");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Interactor_Poke,
    "Isdk.Type.Interactor.Poke",
    "Identifies this Component as a Poke Interactor");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Device_Hand,
    "Isdk.Type.Device.Hand",
    "Interaction was invoked by a user input device that has articulated hand joint data.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Device_TrackedController,
    "Isdk.Type.Device.TrackedController",
    "Interaction was invoked by a user input device that is held by a hand.");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Device_Left,
    "Isdk.Flag.Handedness.Left",
    "Interaction was invoked from a device with handedness: Left");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Type_Device_Right,
    "Isdk.Flag.Handedness.Right",
    "Interaction was invoked from a device with handedness: Right");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(
    TAG_Isdk_Flag_Interactable_HideHandOnSelect,
    "Isdk.Flag.Interactable.HideHandOnSelect",
    "Flags this Interactable to Hide Hands when selected");
} // namespace IsdkGameplayTags
