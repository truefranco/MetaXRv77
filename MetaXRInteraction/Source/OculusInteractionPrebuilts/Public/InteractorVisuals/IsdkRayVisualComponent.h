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
#include "Materials/MaterialInstanceDynamic.h"
#include "DataSources/IsdkIHmdDataSource.h"
#include "Curves/CurveFloat.h"
#include "IsdkRayVisualComponent.generated.h"

/**
 * @class UIsdkRayVisualComponent
 * @brief Scene Component containing all relevant data for a ray visual
 *
 * Includes configuration parameters, getters and setters for related meshes and materials for the
 * ray visual
 * @see AIsdkRayVisual
 * @addtogroup InteractionSDKPrebuilts
 */
UCLASS(
    ClassGroup = (Custom),
    meta = (BlueprintSpawnableComponent, DisplayName = "ISDK Ray Visual Component"))
class OCULUSINTERACTIONPREBUILTS_API UIsdkRayVisualComponent : public USceneComponent
{
  GENERATED_BODY()

 public:
  UIsdkRayVisualComponent();

  /**
   * @brief When true, during BeginPlay this actor will bind the configured input actions to the
   * PlayerController at index 0.
   * If false, a manual call to BindInputActionEvents must be made to bind the input actions.
   */
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = InteractionSDK)
  bool bAutoBindInputActions = true;

  /**
   * @brief Object implementing IIsdkIHmdDataSource to be used as the headset data source for this
   * component
   */
  UPROPERTY()
  TScriptInterface<IIsdkIHmdDataSource> HmdDataSource{};

 private:
  UPROPERTY(EditDefaultsOnly, Category = InteractionSDK)
  USkeletalMeshComponent* PinchArrow;
  UPROPERTY(EditDefaultsOnly, Category = InteractionSDK)
  UStaticMeshComponent* Cursor;
  UPROPERTY(EditInstanceOnly, Category = InteractionSDK)
  UCurveFloat* PinchArrowStrengthCurve;

  UPROPERTY()
  UMaterialInstanceDynamic* CursorMaterialInstance;
  UPROPERTY()
  UMaterialInstanceDynamic* PinchArrowMaterialInstance;

  UPROPERTY(
      BlueprintGetter = GetPinchArrowMaterial,
      BlueprintSetter = SetPinchArrowMaterial,
      EditInstanceOnly,
      Category = "InteractionSDK|Pinch Arrow|Material")
  UMaterialInterface* PinchArrowMaterial;
  UPROPERTY(
      BlueprintGetter = GetCursorMaterial,
      BlueprintSetter = SetCursorMaterial,
      EditInstanceOnly,
      Category = "InteractionSDK|Cursor|Material")
  UMaterialInterface* CursorMaterial;

  UPROPERTY(
      BlueprintGetter = GetPinchArrowMesh,
      BlueprintSetter = SetPinchArrowMesh,
      EditInstanceOnly,
      Category = "InteractionSDK|Pinch Arrow")
  USkeletalMesh* PinchArrowMesh;
  UPROPERTY(
      BlueprintGetter = GetCursorMesh,
      BlueprintSetter = SetCursorMesh,
      EditInstanceOnly,
      Category = "InteractionSDK|Cursor")
  UStaticMesh* CursorMesh;

  void UpdatePinchArrow(float InPinchStrength, FTransform PointerPoseTransform);
  void UpdateCursor(float InPinchStrength, FTransform CursorTransform, bool CollisionHitValid);

 protected:
  /**
   * @brief Overridden from USceneComponent, called on BeginPlay. Calls Initialize()
   */
  virtual void BeginPlay() override;
  /**
   * @brief Overridden from USceneComponent, called when component is registered. Calls Initialize()
   */
  virtual void OnRegister() override;

 public:
  /**
   * @brief Overridden from USceneComponent, called when visibility state of component is changed.
   * Sets new visibility of PinchArrow and Cursor components and propagates to their children
   */
  virtual void OnVisibilityChanged() override;

  /**
   * @brief Overridden from USceneComponent, called when component is destroyed. Recursively
   * destroys PinchArrow and Cursor components
   */
  virtual void DestroyComponent(bool bPromoteChildren = false) override;

  /**
   * @brief With passed in transforms and cursor state and ray strength, update the PinchArrow and
   * Cursor components
   * @param PointerPoseTransform The new transform of the pointer pose component
   * @param CursorTransform The new transform of the cursor component
   * @param CollisionHitValid If there is currently a collision hit from the Cursor
   * @param RaySelectStrength Strength to utilize for both cursor and pinch arrow
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void UpdateVisual(
      FTransform PointerPoseTransform,
      FTransform CursorTransform,
      bool CollisionHitValid,
      float RaySelectStrength);

  /**
   * @brief Sets the meshes and materials for the Cursor and PinchArrow components
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void Initialize();

  /**
   * @brief Sets the static mesh to be used by the Cursor component
   * @param Mesh New mesh to use for the Cursor component
   */
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetCursorMesh(UStaticMesh* Mesh);

  /**
   * @brief Gets the static mesh being used by the Cursor component
   * @return UStaticMesh* Mesh in use for the Cursor component
   */
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UStaticMesh* GetCursorMesh() const
  {
    return CursorMesh;
  }

  /**
   * @brief Sets the material to be used by the Cursor component
   * @param InMaterial New material to use for the Cursor component
   */
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetCursorMaterial(UMaterialInterface* InMaterial);

  /**
   * @brief Gets the material being used by the Cursor component
   * @return UMaterialInterface* Material in use for the Cursor component
   */
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UMaterialInterface* GetCursorMaterial() const
  {
    return CursorMaterial;
  }

  /**
   * @brief Gets the material instance being used by the Cursor component
   * @return UMaterialInterface* Material instance in use for the Cursor component
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  UMaterialInstanceDynamic* GetCursorMaterialInstance() const
  {
    return CursorMaterialInstance;
  }

  /**
   * @brief Sets the static mesh to be used by the PinchArrow component
   * @param Mesh New mesh to use for the PinchArrow component
   */
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetPinchArrowMesh(USkeletalMesh* Mesh);

  /**
   * @brief Gets the static mesh being used by the PinchArrow component
   * @return UStaticMesh* Mesh in use for the PinchArrow component
   */
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  USkeletalMesh* GetPinchArrowMesh() const
  {
    return PinchArrowMesh;
  }

  /**
   * @brief Sets the material to be used by the PinchArrow component
   * @param InMaterial New material to use for the PinchArrow component
   */
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetPinchArrowMaterial(UMaterialInterface* InMaterial);

  /**
   * @brief Gets the material being used by the PinchArrow component
   * @return UMaterialInterface* Material in use for the PinchArrow component
   */
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  UMaterialInterface* GetPinchArrowMaterial() const
  {
    return PinchArrowMaterial;
  }

  /**
   * @brief Gets the material instance being used by the PinchArrow component
   * @return UMaterialInterface* Material instance in use for the PinchArrow component
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  UMaterialInstanceDynamic* GetPinchArrowMaterialInstance() const
  {
    return PinchArrowMaterialInstance;
  }

  /**
   * @brief Sets the translucence sort priority, used by UPrimiveComponent for rendering
   * @param InTranslucentSortPriority The translucence sort priority to use for rendering
   */
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetTranslucentSortPriority(int32 InTranslucentSortPriority);

  /**
   * @brief Gets the translucence sort priority, used by UPrimiveComponent for rendering
   * @return int32 The translucence sort priority set for rendering
   */
  UFUNCTION(BlueprintGetter, Category = InteractionSDK)
  int32 GetTranslucentSortPriority()
  {
    return TranslucentSortPriority;
  }

  /**
   * @brief Updates Material Instances with new values for both the Cursor (AccentColor, Color,
   * ShadowColor) and PinchArrow (Color) components
   */
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void UpdateMaterialInstanceParameters();

  /**
   * @brief If the Arrow element of the cursor should be rendered
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Pinch Arrow")
  bool ShowArrow = true;

  /**
   * @brief Morph target names used for the PinchArrow
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Pinch Arrow")
  TArray<FName> MorphTargetNames;

  /**
   * @brief Linear color used when rendering the PinchArrow
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Pinch Arrow|Material")
  FLinearColor PinchArrowColor;

  /**
   * @brief If the cursor element of the cursor should be rendered
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Cursor")
  bool ShowCursor = true;

  /**
   * @brief Distance at which the scale of the cursor is 1.0
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Cursor")
  float ReferenceDistance = 50.0;

  /**
   * @brief Cursor size range used for the scale change based on the current pinch strength
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Cursor")
  FVector2D CursorSizeMinMax = FVector2D(0.02, 0.015);

  /**
   * @brief Linear accent color to be used when rendering the Cursor
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Cursor|Material")
  FLinearColor CursorAccentColor;

  /**
   * @brief Linear color to be used when rendering the Cursor
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Cursor|Material")
  FLinearColor CursorColor;

  /**
   * @brief Linear color to be used when rendering the Cursor's shadow
   */
  UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "InteractionSDK|Cursor|Material")
  FLinearColor CursorShadowColor;

 private:
  UPROPERTY(
      BlueprintGetter = GetTranslucentSortPriority,
      BlueprintSetter = SetTranslucentSortPriority,
      EditInstanceOnly,
      Category = "InteractionSDK|Cursor|Material")
  int32 TranslucentSortPriority;
};
