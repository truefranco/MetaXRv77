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
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "IsdkInformationFrame.generated.h"

class UMaterial;
class USceneComponent;
class UStaticMeshComponent;

USTRUCT()
struct FIsdkInformationFrameLabelProps
{
  GENERATED_BODY()
  UPROPERTY(Category = InteractionSDK, EditAnywhere, meta = (MultiLine = true))
  FText Text;
  UPROPERTY(Category = InteractionSDK, EditAnywhere)
  float Size = 2.0;
  UPROPERTY(Category = InteractionSDK, EditAnywhere)
  FLinearColor Color = FLinearColor(FVector4f(1.0, 1.0, 1.0, 1.0));
  UPROPERTY(Category = InteractionSDK, EditAnywhere)
  TEnumAsByte<EHorizTextAligment> Alignment = EHorizTextAligment::EHTA_Center;
};

UCLASS(ClassGroup = (InteractionSDK), meta = (DisplayName = "ISDK Information Frame"))
class OCULUSINTERACTIONPREBUILTS_API AIsdkInformationFrame : public AActor
{
  GENERATED_BODY()
 protected:
  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  USceneComponent* InformationFrameRoot;
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  USceneComponent* LabelGroup;
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  UTextRenderComponent* LabelTextRender;
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  UTextRenderComponent* SecondaryLabelTextRender = nullptr;
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  UStaticMeshComponent* LabelBackground;
  UPROPERTY(BlueprintReadOnly, Category = InteractionSDK)
  UStaticMeshComponent* FloorOutline;

 private:
  void Initialize();
  void SetTextRenderProperties(
      UTextRenderComponent* TextRender,
      FIsdkInformationFrameLabelProps Properties,
      UMaterial* Material);
  void SetTextPosition(
      UTextRenderComponent* TextRender,
      FIsdkInformationFrameLabelProps Properties,
      FVector ContainerSize,
      float VerticalMultiplier);

 public:
  AIsdkInformationFrame();
  virtual void OnConstruction(const FTransform& Transform) override;
  virtual void PostInitializeComponents() override;

#if WITH_EDITOR
  virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

  UPROPERTY(Category = "InteractionSDK|Outline", BlueprintReadOnly, EditAnywhere)
  bool bShowOutline = true;
  UPROPERTY(Category = "InteractionSDK|Outline", BlueprintReadOnly, EditAnywhere)
  FLinearColor OutlineColor = FLinearColor(FVector4f(1.0, 1.0, 1.0, 1.0));
  UPROPERTY(Category = "InteractionSDK|Outline", BlueprintReadOnly, EditAnywhere)
  float OutlineCornerRadius = 1.0;
  UPROPERTY(Category = "InteractionSDK|Outline", BlueprintReadOnly, EditAnywhere)
  float OutlineWidth = 0.3;
  UPROPERTY(Category = "InteractionSDK|Outline", BlueprintReadOnly, EditAnywhere)
  FVector2D OutlineSize = FVector2D(10.0, 10.0);

  UPROPERTY(Category = "InteractionSDK|Label", BlueprintReadOnly, EditAnywhere)
  FVector LabelOffset = FVector(-1.0, 1.0, 0.5);
  UPROPERTY(Category = "InteractionSDK|Label", BlueprintReadOnly, EditAnywhere)
  FVector2D LabelFrameAttachment = FVector2D(1.0, 1.0);
  UPROPERTY(Category = "InteractionSDK|Label", BlueprintReadOnly, EditAnywhere)
  float LabelMargin = 0.0;

  UPROPERTY(Category = "InteractionSDK|Label|Background", BlueprintReadOnly, EditAnywhere)
  FLinearColor BackgroundColor = FLinearColor(FVector4f(0.0, 0.0, 0.0, 1.0));
  UPROPERTY(Category = "InteractionSDK|Label|Background", BlueprintReadOnly, EditAnywhere)
  float BackgroundCornerRadius = 0.3;
  UPROPERTY(Category = "InteractionSDK|Label|Background", BlueprintReadOnly, EditAnywhere)
  FVector2D BackgroundPadding = FVector2D(0.5, 0.5);
  UPROPERTY(
      Category = "InteractionSDK|Label|Background",
      BlueprintReadOnly,
      EditAnywhere,
      meta = (ClampMin = 0.0, ClampMax = 360.0, UIMin = 0.0, UIMax = 360.0))
  float BackgroundAngle = 60.0;

  UPROPERTY(Category = "InteractionSDK|Label|Text", EditAnywhere)
  FIsdkInformationFrameLabelProps PrimaryLabelProperties{};
  UPROPERTY(Category = "InteractionSDK|Label|Text", EditAnywhere)
  FIsdkInformationFrameLabelProps SecondaryLabelProperties{};
};
