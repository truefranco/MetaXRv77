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
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Interaction/IsdkPokeInteractable.h"
#include "Interaction/IsdkRayInteractable.h"
#include "Interaction/IsdkClippedPlaneSurface.h"
#include "Widget/IsdkPointableWidget.h"
#include "Audio/IsdkWidgetEventAudioPlayer.h"
#include "IsdkInteractableWidgetComponent.generated.h"

UENUM(BlueprintType)
enum class EIsdkWidgetBlendMode : uint8
{
  Masked,
  Transparent
};

UCLASS(
    ClassGroup = (Custom),
    meta = (BlueprintSpawnableComponent, DisplayName = "ISDK Interactable Widget"))
class OCULUSINTERACTIONPREBUILTS_API UIsdkInteractableWidgetComponent : public USceneComponent
{
  GENERATED_BODY()

 public:
  UIsdkInteractableWidgetComponent();

  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UWidgetComponent* GetWidget()
  {
    return Widget;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UAudioComponent* GetSelectedHoveredAudio()
  {
    return SelectedHoveredAudio;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UAudioComponent* GetSelectedEmptyAudio()
  {
    return SelectedEmptyAudio;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UIsdkPokeInteractable* GetPokeInteractable()
  {
    return PokeInteractable;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UIsdkRayInteractable* GetRayInteractable()
  {
    return RayInteractable;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UIsdkPointableWidget* GetPointableWidget()
  {
    return PointableWidget;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UIsdkWidgetEventAudioPlayer* GetWidgetEventAudioPlayer()
  {
    return WidgetEventAudioPlayer;
  }
  virtual void DestroyComponent(bool bPromoteChildren = false) override;

 private:
  // Instanced (created in constructor)
  UPROPERTY(BlueprintGetter = GetWidget, VisibleDefaultsOnly, Category = InteractionSDK)
  UWidgetComponent* Widget;
  void CreateWidgetInstance();
  UPROPERTY()
  UUserWidget* WidgetInstance{};

  UPROPERTY(
      BlueprintGetter = GetSelectedHoveredAudio,
      VisibleDefaultsOnly,
      Category = InteractionSDK)
  UAudioComponent* SelectedHoveredAudio;
  UPROPERTY(BlueprintGetter = GetSelectedEmptyAudio, VisibleDefaultsOnly, Category = InteractionSDK)
  UAudioComponent* SelectedEmptyAudio;
  UPROPERTY()
  UIsdkClippedPlaneSurface* ClippedPlaneSurface;
  UPROPERTY(BlueprintGetter = GetPointableWidget, VisibleDefaultsOnly, Category = InteractionSDK)
  UIsdkPointableWidget* PointableWidget;
  UPROPERTY(BlueprintGetter = GetWidgetEventAudioPlayer, Category = InteractionSDK)
  UIsdkWidgetEventAudioPlayer* WidgetEventAudioPlayer;

  UPROPERTY(BlueprintGetter = GetPokeInteractable, Category = InteractionSDK)
  UIsdkPokeInteractable* PokeInteractable{};
  UPROPERTY(BlueprintGetter = GetRayInteractable, Category = InteractionSDK)
  UIsdkRayInteractable* RayInteractable{};
  UPROPERTY()
  UIsdkPointablePlane* PointablePlane{};
  UPROPERTY()
  UIsdkPointablePlane* RayPointablePlane{};
  UPROPERTY()
  UIsdkPointablePlane* RaySelectPlane{};

  void Initialize();

 protected:
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  void SetWidgetProperties();

  UPROPERTY()
  UMaterialInterface* MaskedMaterial;
  UPROPERTY()
  UMaterialInterface* MaskedMaterialOneSided;
  UPROPERTY()
  UMaterialInterface* TransparentMaterial;
  UPROPERTY()
  UMaterialInterface* TransparentMaterialOneSided;

  bool EditingSize;
  FVector2D PreviousDrawSize;
  virtual void TickComponent(
      float DeltaTime,
      ELevelTick TickType,
      FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
  virtual bool CanEditChange(const FProperty* InProperty) const override;
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

 public:
  virtual void BeginPlay() override;
  virtual void OnRegister() override;
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void SetRoundedMaterialProperties();

  UPROPERTY(
      Category = "InteractionSDK|Widget Component|Rounded Box Material",
      EditAnywhere,
      BlueprintReadWrite,
      Meta = (ExposeOnSpawn = true))
  bool bUseRoundedBoxMaterial;
  UPROPERTY(
      Category = "InteractionSDK|Widget Component|Rounded Box Material",
      EditAnywhere,
      BlueprintReadWrite,
      Meta = (ExposeOnSpawn = true))
  FVector4 CornerRadius = FVector4(0.0, 0.0, 0.0, 0.0);
  UPROPERTY(
      Category = "InteractionSDK|Widget Component|Rounded Box Material",
      EditAnywhere,
      BlueprintReadWrite,
      Meta = (ExposeOnSpawn = true))
  FLinearColor BackgroundColor;
  UPROPERTY(
      Category = "InteractionSDK|Widget Component|Rounded Box Material",
      EditAnywhere,
      BlueprintReadWrite,
      Meta = (ExposeOnSpawn = true))
  EIsdkWidgetBlendMode BlendMode;
  UPROPERTY(
      Category = "InteractionSDK|Widget Component|Rounded Box Material",
      EditAnywhere,
      BlueprintReadWrite,
      Meta = (ExposeOnSpawn = true))
  bool bTwoSided;

  UPROPERTY(
      Category = "InteractionSDK|Interactable|Poke Interactable",
      EditAnywhere,
      BlueprintReadOnly,
      Meta = (ExposeOnSpawn = true))
  bool bCreatePokeInteractable;
  UPROPERTY(
      Category = "InteractionSDK|Interactable|Poke Interactable",
      EditAnywhere,
      BlueprintReadOnly,
      Meta = (ExposeOnSpawn = true))
  UIsdkPokeInteractableConfigDataAsset* DefaultPokeInteractableConfigAsset{};
  UPROPERTY(
      Category = "InteractionSDK|Interactable|Poke Interactable",
      EditAnywhere,
      BlueprintReadOnly,
      Meta = (ExposeOnSpawn = true))
  FIsdkPokeInteractableConfigOffsets DefaultPokeInteractableConfigOffsets{};

  UPROPERTY(
      Category = "InteractionSDK|Interactable|Ray Interactable",
      EditAnywhere,
      BlueprintReadOnly,
      Meta = (ExposeOnSpawn = true))
  bool bCreateRayInteractable;

  UPROPERTY(
      Category = "InteractionSDK|Widget Component",
      EditAnywhere,
      BlueprintReadOnly,
      Meta = (ExposeOnSpawn = true))
  FVector2D DrawSize;
  UPROPERTY(
      Category = "InteractionSDK|Widget Component",
      EditAnywhere,
      BlueprintReadOnly,
      Meta = (ExposeOnSpawn = true))
  float WidgetScale;
  UPROPERTY(
      Category = "InteractionSDK|Widget Component",
      EditAnywhere,
      BlueprintReadOnly,
      Meta = (ExposeOnSpawn = true))
  TSubclassOf<UUserWidget> WidgetClass;
};
