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

#include "InteractorVisuals/IsdkRayVisualComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"
#include "IsdkContentAssetPaths.h"
#include "IsdkFunctionLibrary.h"
#include "InteractorVisuals/IsdkRayVisualFunctionLibrary.h"

// Sets default values for this component's properties
UIsdkRayVisualComponent::UIsdkRayVisualComponent()
{
  PrimaryComponentTick.bCanEverTick = false;

  PinchArrow = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Pinch Arrow"));
  PinchArrow->SetupAttachment(this);
  PinchArrow->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
  PinchArrow->SetRelativeScale3D(FVector(0.01, 0.01, 0.01));
  static ConstructorHelpers::FObjectFinder<USkeletalMesh> PinchArrowMeshFinder(
      IsdkContentAssetPaths::Models::RayVisual::OculusHandPinchArrowBlended);
  if (ensureMsgf(PinchArrowMeshFinder.Succeeded(), TEXT("Could not load Pinch Arrow's Mesh.")))
  {
    SetPinchArrowMesh(PinchArrowMeshFinder.Object);
  }

  static ConstructorHelpers::FObjectFinder<UMaterial> PinchArrowMaterialFinder(
      IsdkContentAssetPaths::Materials::PinchArrow);
  if (ensureMsgf(
          PinchArrowMaterialFinder.Succeeded(), TEXT("Could not load Pinch Arrow's Material.")))
  {
    SetPinchArrowMaterial(PinchArrowMaterialFinder.Object);
  }
  PinchArrowColor = FLinearColor(1.0, 1.0, 1.0);

  Cursor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cursor Plane"));
  Cursor->SetupAttachment(this);
  Cursor->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
  Cursor->SetRelativeScale3D(FVector(0.06, 0.06, 0.06));
  static ConstructorHelpers::FObjectFinder<UStaticMesh> CursorMeshFinder(
      IsdkContentAssetPaths::Engine::BasicShapes::Plane);
  if (ensureMsgf(CursorMeshFinder.Succeeded(), TEXT("Could not load Engine's Plane Basic Shape.")))
  {
    SetCursorMesh(CursorMeshFinder.Object);
  }
  TranslucentSortPriority = 1000;

  static ConstructorHelpers::FObjectFinder<UMaterial> CursorMaterialFinder(
      IsdkContentAssetPaths::Materials::Cursor);
  if (ensureMsgf(CursorMaterialFinder.Succeeded(), TEXT("Could not load Cursor's Material.")))
  {
    SetCursorMaterial(CursorMaterialFinder.Object);
  }
  CursorAccentColor = FLinearColor(0.031, 0.376, 0.965);
  CursorColor = FLinearColor(1.0, 1.0, 1.0);
  CursorShadowColor = FLinearColor(0.0, 0.0, 0.0);

  static ConstructorHelpers::FObjectFinder<UCurveFloat> PinchStrengthCurve(
      IsdkContentAssetPaths::Models::RayVisual::PinchStrengthCurve);
  if (ensureMsgf(PinchStrengthCurve.Succeeded(), TEXT("Could not load Pinch's Strength Curve.")))
  {
    PinchArrowStrengthCurve = PinchStrengthCurve.Object;
  }

  MorphTargetNames.Empty();
  MorphTargetNames.Add(FName("ClosingCone"));
  MorphTargetNames.Add(FName("PopCone"));

  ReferenceDistance = 50.0;
  CursorSizeMinMax = FVector2D(0.02, 0.015);
}

void UIsdkRayVisualComponent::BeginPlay()
{
  Super::BeginPlay();
  Initialize();
}

void UIsdkRayVisualComponent::OnRegister()
{
  Super::OnRegister();
  Initialize();
}

void UIsdkRayVisualComponent::OnVisibilityChanged()
{
  Super::OnVisibilityChanged();
  if (IsValid(PinchArrow))
  {
    PinchArrow->SetVisibility(IsVisible(), true);
  }
  if (IsValid(Cursor))
  {
    Cursor->SetVisibility(IsVisible(), true);
  }
}

void UIsdkRayVisualComponent::DestroyComponent(bool bPromoteChildren /*= false*/)
{
  if (IsValid(PinchArrow))
  {
    PinchArrow->DestroyComponent(bPromoteChildren);
  }
  if (IsValid(Cursor))
  {
    Cursor->DestroyComponent(bPromoteChildren);
  }
  Super::DestroyComponent(bPromoteChildren);
}

void UIsdkRayVisualComponent::SetCursorMaterial(UMaterialInterface* InMaterial)
{
  CursorMaterial = InMaterial;
  CursorMaterialInstance =
      UMaterialInstanceDynamic::Create(InMaterial, this, FName("Cursor Material"));
  UpdateMaterialInstanceParameters();
  if (Cursor)
  {
    Cursor->SetMaterial(0, CursorMaterialInstance);
  }
}

void UIsdkRayVisualComponent::SetPinchArrowMaterial(UMaterialInterface* InMaterial)
{
  PinchArrowMaterial = InMaterial;
  PinchArrowMaterialInstance =
      UMaterialInstanceDynamic::Create(InMaterial, this, FName("Pinch Arrow Material"));
  UpdateMaterialInstanceParameters();
  if (PinchArrow)
  {
    PinchArrow->SetMaterial(0, PinchArrowMaterialInstance);
  }
}

void UIsdkRayVisualComponent::SetTranslucentSortPriority(int32 InTranslucentSortPriority)
{
  TranslucentSortPriority = InTranslucentSortPriority;
  Cursor->SetTranslucentSortPriority(TranslucentSortPriority);
}

void UIsdkRayVisualComponent::UpdateMaterialInstanceParameters()
{
  if (CursorMaterialInstance)
  {
    CursorMaterialInstance->SetVectorParameterValue(FName("AccentColor"), CursorAccentColor);
    CursorMaterialInstance->SetVectorParameterValue(FName("Color"), CursorColor);
    CursorMaterialInstance->SetVectorParameterValue(FName("ShadowColor"), CursorShadowColor);
  }
  if (PinchArrowMaterialInstance)
  {
    PinchArrowMaterialInstance->SetVectorParameterValue(FName("Color"), PinchArrowColor);
  }
}

void UIsdkRayVisualComponent::UpdatePinchArrow(float PinchStrength, FTransform PointerPoseTransform)
{
  if (!ShowArrow)
  {
    PinchArrow->SetVisibility(false);
    return;
  }
  PinchArrow->SetVisibility(true);

  for (int i = 0; i < MorphTargetNames.Num(); i++)
  {
    auto MorphName = MorphTargetNames[i];
    PinchArrow->SetMorphTarget(MorphName, PinchStrength);
  }
  PointerPoseTransform.SetScale3D(PinchArrow->GetComponentScale());
  PinchArrow->SetWorldTransform(PointerPoseTransform);

  if (PinchArrowMaterialInstance)
  {
    PinchArrowMaterialInstance->SetScalarParameterValue(FName("SelectStrength"), PinchStrength);
  }
}

void UIsdkRayVisualComponent::UpdateCursor(
    float PinchStrength,
    FTransform CursorTransform,
    bool CollisionHitValid)
{
  float CursorSizeByStrength =
      UKismetMathLibrary::Lerp(CursorSizeMinMax.X, CursorSizeMinMax.Y, PinchStrength);
  float CursorScale = UIsdkRayVisualFunctionLibrary::GetCursorScaleFromDistanceToHmd(
      CursorTransform.GetLocation(), ReferenceDistance, HmdDataSource);
  if (!CollisionHitValid || !ShowCursor)
  {
    Cursor->SetVisibility(false);
    return;
  }
  Cursor->SetVisibility(true);

  Cursor->SetWorldScale3D(FVector(CursorSizeByStrength * CursorScale));
  Cursor->SetWorldLocation(CursorTransform.GetLocation());
  Cursor->SetWorldRotation(CursorTransform.GetRotation());

  if (CursorMaterialInstance)
  {
    CursorMaterialInstance->SetScalarParameterValue(FName("SelectStrength"), PinchStrength);
  }
}

void UIsdkRayVisualComponent::UpdateVisual(
    FTransform PointerPoseTransform,
    FTransform CursorTransform,
    bool CollisionHitValid,
    float RaySelectStrength)
{
  if (!IsVisible())
  {
    return;
  }

  auto PinchStrength = RaySelectStrength;
  if (IsValid(PinchArrowStrengthCurve))
  {
    PinchStrength = PinchArrowStrengthCurve->GetFloatValue(PinchStrength);
  }

  UpdatePinchArrow(PinchStrength, PointerPoseTransform);
  UpdateCursor(PinchStrength, CursorTransform, CollisionHitValid);
}

void UIsdkRayVisualComponent::Initialize()
{
  SetCursorMaterial(CursorMaterial);
  SetCursorMesh(CursorMesh);
  SetTranslucentSortPriority(TranslucentSortPriority);
  SetPinchArrowMaterial(PinchArrowMaterial);
  SetPinchArrowMesh(PinchArrowMesh);
}

void UIsdkRayVisualComponent::SetCursorMesh(UStaticMesh* Mesh)
{
  CursorMesh = Mesh;
  Cursor->SetStaticMesh(CursorMesh);
}

void UIsdkRayVisualComponent::SetPinchArrowMesh(USkeletalMesh* Mesh)
{
  PinchArrowMesh = Mesh;
  PinchArrow->SetSkeletalMesh(PinchArrowMesh);
}
