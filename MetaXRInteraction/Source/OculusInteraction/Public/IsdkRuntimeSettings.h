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
#include "Subsystem/IsdkITrackingDataSubsystem.h"
#include "NativeGameplayTags.h"
#include "IsdkRuntimeSettings.generated.h"

// Declaring our native Gameplay Tags
namespace IsdkGameplayTags
{
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Interactable_Ray);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Interactable_Poke);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Interactable_Grab);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Interactor_Ray);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Interactor_Poke);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Interactor_Grab);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Device_Hand);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Device_TrackedController);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Device_Left);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Type_Device_Right);
OCULUSINTERACTION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Isdk_Flag_Interactable_HideHandOnSelect);
} // namespace IsdkGameplayTags

/**
 * Implements the settings for the ISDK MetaXR plugin.
 */
UCLASS(Config = Engine, DefaultConfig, DisplayName = "Meta XR - Interaction")
class OCULUSINTERACTION_API UIsdkRuntimeSettings : public UObject
{
  GENERATED_BODY()

 public:
  UPROPERTY(
      config,
      EditAnywhere,
      Category = "Tracking Data",
      meta = (MustImplement = "/Script/OculusInteraction.IsdkITrackingDataSubsystem"))
  TArray<TSubclassOf<USubsystem>> DisabledTrackingDataSubsystems;

  /**
   * What color to display when an interactor is in a "Normal" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Interactors", DisplayName = "Normal Color")
  FColor InteractorColorNormal = FColor::Yellow;

  /**
   * What color to display when an interactor is in a "Hover" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Interactors", DisplayName = "Hover Color")
  FColor InteractorColorHover = FColor::White;

  /**
   * What color to display when an interactor is in a "Select" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Interactors", DisplayName = "Select Color")
  FColor InteractorColorSelect = FColor::Cyan;

  /**
   * What color to display when an interactor is in a "Disabled" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Interactors", DisplayName = "Disabled Color")
  FColor InteractorColorDisabled = FColor::Black;

  /**
   * What color to display when an interactor is in a "Normal" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Interactables", DisplayName = "Normal Color")
  FColor InteractableColorNormal = FColor::Yellow;

  /**
   * What color to display when an interactor is in a "Hover" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Interactables", DisplayName = "Hover Color")
  FColor InteractableColorHover = FColor::White;

  /**
   * What color to display when an interactor is in a "Select" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Interactables", DisplayName = "Select Color")
  FColor InteractableColorSelect = FColor::Cyan;

  /**
   * What color to display when an interactor is in a "Disabled" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(
      Config,
      EditAnywhere,
      Category = "Debug | Interactables",
      DisplayName = "Disabled Color")
  FColor InteractableColorDisabled = FColor::Black;

  /**
   * What color to display when an interactor is in a "Hover" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Pointer Events", DisplayName = "Hover Color")
  FColor PointerEventColorHover = FColor::White;

  /**
   * What color to display when an interactor is in a "Select" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Pointer Events", DisplayName = "Select Color")
  FColor PointerEventColorSelect = FColor::Cyan;

  /**
   * What color to display when an interactor is in a "Unselect" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(
      Config,
      EditAnywhere,
      Category = "Debug | Pointer Events",
      DisplayName = "Unselect Color")
  FColor PointerEventColorUnselect = FColor::Purple;

  /**
   * What color to display when an interactor is in a "Cancel" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Pointer Events", DisplayName = "Cancel Color")
  FColor PointerEventColorCancel = FColor::Orange;

  /**
   * What color to display when an interactor is in a "Unhover" state.
   * Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(
      Config,
      EditAnywhere,
      Category = "Debug | Pointer Events",
      DisplayName = "Unhover Color")
  FColor PointerEventColorUnhover = FColor::Black;

  /**
   * Pointer events render as spheres when debug drawn.  This settings drives the radius of these
   * spheres.  Used when Meta.InteractionSDK.DebugInteractionVisuals console variable is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Pointer Events", DisplayName = "Debug Radius")
  float PointerEventDebugRadius = 2.f;

  /**
   * How long to display debug pointer events
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Pointer Events", DisplayName = "Duration")
  float PointerEventDebugDuration = 0.3f;

  /**
   * The color to use for drawing hand bones when Meta.InteractionSDK.DebugHandVisuals is enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Hand", DisplayName = "Hand Bone Color")
  FColor HandDebugColor = FColor::Magenta;

  /**
   * The thickness to use for drawing hand bones when Meta.InteractionSDK.DebugHandVisuals is
   * enabled.
   */
  UPROPERTY(Config, EditAnywhere, Category = "Debug | Hand", DisplayName = "Hand Bone Thickness")
  float HandDebugThickness = 0.2f;

  static UIsdkRuntimeSettings& Get()
  {
    return *GetMutableDefault<UIsdkRuntimeSettings>();
  }
};
