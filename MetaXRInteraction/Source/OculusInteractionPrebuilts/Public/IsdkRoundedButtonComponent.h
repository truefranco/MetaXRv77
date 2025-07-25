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
#include "Interaction/IsdkPokeInteractable.h"
#include "InteractableVisuals/IsdkInteractableColorVisual.h"
#include "Interaction/IsdkPokeButtonVisual.h"
#include "Interaction/Surfaces/IsdkPointablePlane.h"
#include "Interaction/IsdkClippedPlaneSurface.h"
#include "Audio/IsdkPointerEventAudioPlayer.h"
#include "Components/TextRenderComponent.h"
#include "IsdkRoundedButtonComponent.generated.h"

UCLASS(
    ClassGroup = (InteractionSDK),
    meta = (BlueprintSpawnableComponent, DisplayName = "ISDK Rounded Button"))
class OCULUSINTERACTIONPREBUILTS_API UIsdkRoundedButtonComponent : public USceneComponent
{
  GENERATED_BODY()

 public:
  // Sets default values for this component's properties
  UIsdkRoundedButtonComponent();

  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UAudioComponent* GetInteractionButtonPress() const
  {
    return InteractionButtonPress;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UAudioComponent* GetInteractionButtonRelease() const
  {
    return InteractionButtonRelease;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UStaticMeshComponent* GetButtonMesh() const
  {
    return ButtonMesh;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UIsdkInteractableColorVisual* GetInteractableColorVisual() const
  {
    return InteractableColorVisual;
  }

  virtual void DestroyComponent(bool bPromoteChildren = false) override;

#if WITH_EDITOR
  void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

 private:
  UPROPERTY(
      BlueprintGetter = GetInteractionButtonPress,
      Category = "InteractionSDK",
      VisibleDefaultsOnly)
  UAudioComponent* InteractionButtonPress;
  UPROPERTY(
      BlueprintGetter = GetInteractionButtonRelease,
      Category = "InteractionSDK",
      VisibleDefaultsOnly)
  UAudioComponent* InteractionButtonRelease;
  UPROPERTY(
      BlueprintGetter = GetInteractableColorVisual,
      Category = "InteractionSDK|Material Props",
      VisibleDefaultsOnly)
  UIsdkInteractableColorVisual* InteractableColorVisual;

  UPROPERTY()
  UIsdkPointerEventAudioPlayer* PointerEventAudioPlayer;
  UPROPERTY()
  UTextRenderComponent* ButtonLabel{};
  UPROPERTY(BlueprintGetter = GetButtonMesh, Category = InteractionSDK)
  UStaticMeshComponent* ButtonMesh;
  UPROPERTY()
  UStaticMeshComponent* BackplaneMesh;

  void SetDefaultValues();
  void SetupTextComponent();
  void UpdateProperties();
  void Initialize();
  virtual void OnRegister() override;
  virtual void BeginPlay() override;
  virtual void TickComponent(
      float DeltaTime,
      enum ELevelTick TickType,
      FActorComponentTickFunction* ThisTickFunction) override;

 public:
  UPROPERTY(
      Category = "InteractionSDK|Poke Visual",
      EditAnywhere,
      BlueprintReadOnly,
      meta = (ToolTip = "Does nothing when changed after begin play.", ExposeOnSpawn = true))
  FVector2D ButtonSize;
  UPROPERTY(
      Category = "InteractionSDK|Poke Visual",
      EditAnywhere,
      BlueprintReadOnly,
      meta = (ToolTip = "Does nothing when changed after begin play.", ExposeOnSpawn = true))
  float VisualOffset;
  UPROPERTY(
      Category = "InteractionSDK|Poke Visual",
      EditAnywhere,
      BlueprintReadWrite,
      meta = (ExposeOnSpawn = true))
  bool bTwoSidedMaterial = true;

  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetCornerRadius(float Radius);
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  float GetCornerRadius() const
  {
    return CornerRadius;
  }
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetBackplaneFillColor(FLinearColor Color);
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  FLinearColor GetBackplaneFillColor() const
  {
    return BackplaneFillColor;
  }
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetCreateBackplane(bool Value);
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  bool GetCreateBackplane() const
  {
    return CreateBackplane;
  }

  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetLabelSize(float Value)
  {
    LabelSize = Value;
    ButtonLabel->SetWorldSize(LabelSize);
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  float GetLabelSize() const
  {
    return LabelSize;
  }

  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetLabelText(FString Value)
  {
    LabelText = Value;
    ButtonLabel->SetText(FText::FromString(LabelText));
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  FString GetLabelText() const
  {
    return LabelText;
  }

  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetLabelColor(FLinearColor Value)
  {
    LabelColor = Value;
    ButtonLabel->SetTextRenderColor(LabelColor.ToFColor(false));
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  FLinearColor GetLabelColor() const
  {
    return LabelColor;
  }

 private:
  UPROPERTY(
      Category = "InteractionSDK|Poke Visual|Label",
      EditAnywhere,
      BlueprintGetter = GetLabelSize,
      BlueprintSetter = SetLabelSize,
      meta = (ExposeOnSpawn = true))
  float LabelSize;
  UPROPERTY(
      Category = "InteractionSDK|Poke Visual|Label",
      EditAnywhere,
      BlueprintGetter = GetLabelText,
      BlueprintSetter = SetLabelText,
      meta = (MultiLine = true, ExposeOnSpawn = true))
  FString LabelText;
  UPROPERTY(
      Category = "InteractionSDK|Poke Visual|Label",
      EditAnywhere,
      BlueprintGetter = GetLabelColor,
      BlueprintSetter = SetLabelColor,
      meta = (ExposeOnSpawn = true))
  FLinearColor LabelColor;

  UPROPERTY(
      Category = InteractionSDK,
      EditAnywhere,
      BlueprintGetter = GetCreateBackplane,
      BlueprintSetter = SetCreateBackplane,
      Meta = (ExposeOnSpawn = true))
  bool CreateBackplane;
  UPROPERTY(
      Category = "InteractionSDK|Material Props",
      EditAnywhere,
      BlueprintGetter = GetBackplaneFillColor,
      BlueprintSetter = SetBackplaneFillColor,
      Meta = (ExposeOnSpawn = true))
  FLinearColor BackplaneFillColor;
  UPROPERTY(
      Category = "InteractionSDK|Material Props",
      EditAnywhere,
      BlueprintGetter = GetCornerRadius,
      BlueprintSetter = SetCornerRadius,
      Meta = (ExposeOnSpawn = true))
  float CornerRadius;
  bool bInitialized;

  // Interactable things
 public:
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UIsdkPokeInteractable* GetPokeInteractable() const
  {
    return PokeInteractable;
  }
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UIsdkPokeButtonVisual* GetPokeButtonVisual() const
  {
    return PokeButtonVisual;
  }

 private:
  UPROPERTY(VisibleDefaultsOnly, BlueprintGetter = GetPokeInteractable, Category = InteractionSDK)
  UIsdkPokeInteractable* PokeInteractable;
  UPROPERTY(BlueprintGetter = GetPokeButtonVisual, Category = InteractionSDK)
  UIsdkPokeButtonVisual* PokeButtonVisual;
  UPROPERTY()
  UIsdkPointablePlane* PointablePlane;
  UPROPERTY()
  UIsdkClippedPlaneSurface* ClippedPlaneSurface;
};
