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

#include "IsdkGrabbableMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "IsdkContentAssetPaths.h"

UIsdkGrabbableMeshComponent::UIsdkGrabbableMeshComponent()
{
  PrimaryComponentTick.bCanEverTick = true;

  Material = Cast<UMaterial>(StaticLoadObject(
      UMaterial::StaticClass(), nullptr, IsdkContentAssetPaths::Materials::GrabbableDefault));

  if (!ensureMsgf(
          Material != nullptr, TEXT("Grabbable Prebuilt: Cube Material could not be loaded.")))
  {
    return;
  }

  if (MeshComponent == nullptr)
  {
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    if (!ensureMsgf(IsValid(MeshComponent), TEXT("Grabbable Prebuilt: Mesh Component is invalid.")))
    {
      return;
    }

    MeshComponent->SetupAttachment(this);
  }

  Grabbable = CreateDefaultSubobject<UIsdkGrabbableComponent>(FName("Grabbable"));
  if (!ensureMsgf(IsValid(Grabbable), TEXT("Grabbable Prebuilt: Grabbable could not be created.")))
  {
    return;
  }
  Grabbable->SetupAttachment(this);
}

void UIsdkGrabbableMeshComponent::BeginDestroy()
{
  if (CreatedCollider)
  {
    const auto CurCollider = Grabbable->GetGrabCollider();
    if (IsValid(CurCollider))
    {
      Grabbable->GetGrabCollider()->DestroyComponent();
    }
  }
  Super::BeginDestroy();
}

void UIsdkGrabbableMeshComponent::SetMesh(UStaticMesh* NewMesh)
{
  Mesh = NewMesh;
  MeshComponent->SetStaticMesh(NewMesh);
}

void UIsdkGrabbableMeshComponent::SetCollision(UShapeComponent* NewCollision)
{
  Grabbable->SetGrabCollider(NewCollision);
}

void UIsdkGrabbableMeshComponent::BeginPlay()
{
  Super::BeginPlay();

  // create mesh if not provided
  if (!Mesh)
  {
    Mesh = Cast<UStaticMesh>(StaticLoadObject(
        UStaticMesh::StaticClass(), nullptr, IsdkContentAssetPaths::Engine::BasicShapes::Cube));
  }
  if (!ensureMsgf(Mesh != nullptr, TEXT("Grabbable Prebuilt: mesh could not be created.")))
  {
    return;
  }
  SetMesh(Mesh);

  // create and register collision if not provided
  if (!IsValid(Collision))
  {
    const FBox BoxSize = Mesh->GetBoundingBox();
    const auto collision = NewObject<UBoxComponent>(this, TEXT("Collision Box"));
    collision->SetBoxExtent(BoxSize.GetSize() * .5f, false);
    collision->SetupAttachment(MeshComponent);
    Collision = collision;
    if (IsValid(Collision))
    {
      Collision->RegisterComponent();
    }
  }
  if (!ensureMsgf(
          Collision != nullptr, TEXT("Grabbable Prebuilt: collision could not be created.")))
  {
    return;
  }

  MaterialInstance = UMaterialInstanceDynamic::Create(Material, this, FName("Cube Material"));
  if (!ensureMsgf(
          MaterialInstance != nullptr,
          TEXT("Grabbable Prebuilt: material instance could not be created.")))
  {
    return;
  }
  MaterialInstance->SetVectorParameterValue(TEXT("ColorParam"), FLinearColor::Green);

  if (IsValid(MeshComponent))
  {
    MeshComponent->SetMaterial(0, MaterialInstance);
  }
}
