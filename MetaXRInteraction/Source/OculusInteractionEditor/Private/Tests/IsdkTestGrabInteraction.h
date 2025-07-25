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
#include "Engine/World.h"
#include "Editor.h"
#include "Kismet/GameplayStatics.h"

#include "Interaction/IsdkGrabbableComponent.h"
#include "Interaction/IsdkGrabberComponent.h"
#include "Interaction/Grabbable/IsdkGrabTransformerComponent.h"
#include "Interaction/Grabbable/IsdkHandGrabPose.h"
#include "Interaction/GrabDetectors/IsdkHandGrabDetector.h"
#include "IsdkTestFakes.h"

#include "IsdkTestGrabInteraction.generated.h"

UCLASS()
class OCULUSINTERACTIONEDITOR_API AIsdkTestGrabInteractorActor : public AActor
{
  GENERATED_BODY()
 public:
  AIsdkTestGrabInteractorActor()
  {
    const auto Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
    SetRootComponent(Root);

    TestGrabberComponent =
        CreateDefaultSubobject<UIsdkGrabberComponent>(TEXT("TestGrabberComponent"));
    TestGrabberComponent->SetupAttachment(RootComponent);
    TestGrabberComponent->SetGrabInputMethodAllowed(EIsdkGrabInputMethod::Pinch, true);
    TestGrabberComponent->SetGrabInputMethodAllowed(EIsdkGrabInputMethod::Palm, true);
    TestGrabberComponent->SetGrabDetectionTypeAllowed(EIsdkGrabDetectorType::HandGrab, true);
    TestGrabberComponent->SetGrabDetectionTypeAllowed(EIsdkGrabDetectorType::DistanceGrab, false);
    TestGrabberComponent->SetGrabDetectionTypeAllowed(EIsdkGrabDetectorType::RayGrab, false);
    TestGrabberComponent->Activate();
  }

  static bool Setup()
  {
    // No getting the level dirty
    FActorSpawnParameters ActorParameters{};
    ActorParameters.bNoFail = true;
    UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();

    const auto TestActor = TestWorld->SpawnActor<AIsdkTestGrabInteractorActor>(
        AIsdkTestGrabInteractorActor::StaticClass(), ActorParameters);
    return ensureMsgf(TestActor, TEXT("Failed to spawn test actor: AIsdkTestGrabInteractorActor"));
  }

  static AIsdkTestGrabInteractorActor& Get()
  {
    const UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();
    check(TestWorld);
    AIsdkTestGrabInteractorActor* Instance = Cast<AIsdkTestGrabInteractorActor, AActor>(
        UGameplayStatics::GetActorOfClass(TestWorld, AIsdkTestGrabInteractorActor::StaticClass()));
    checkf(Instance, TEXT("Failed to cast actor to test object"));
    return *Instance;
  }

  void PinchGrab()
  {
    checkf(TestGrabberComponent, TEXT("GrabberComponent invalid"));
    TestGrabberComponent->PinchGrab();
  }

  void PalmGrab()
  {
    checkf(TestGrabberComponent, TEXT("GrabberComponent invalid"));
    TestGrabberComponent->PalmGrab();
  }

  void PinchRelease()
  {
    checkf(TestGrabberComponent, TEXT("GrabberComponent invalid"));
    TestGrabberComponent->PinchRelease();
  }

  void PalmRelease()
  {
    checkf(TestGrabberComponent, TEXT("GrabberComponent invalid"));
    TestGrabberComponent->PalmRelease();
  }

  UIsdkGrabberComponent* GetGrabberComponent()
  {
    checkf(TestGrabberComponent, TEXT("GrabberComponent invalid"));
    return TestGrabberComponent;
  }

  UIsdkHandGrabDetector* GetHandGrabDetector()
  {
    checkf(TestGrabberComponent, TEXT("GrabberComponent invalid"));
    UIsdkHandGrabDetector* TestHandGrabDetector = TestGrabberComponent->GetHandGrabDetector();
    checkf(TestHandGrabDetector, TEXT("HandGrabDetector invalid"));
    return TestHandGrabDetector;
  }

 private:
  UPROPERTY()
  UIsdkGrabberComponent* TestGrabberComponent{};
};

UCLASS()
class OCULUSINTERACTIONEDITOR_API AIsdkTestGrabInteractableActor : public AActor
{
  GENERATED_BODY()
 public:
  AIsdkTestGrabInteractableActor()
  {
    const auto Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
    SetRootComponent(Root);

    TestGrabbableComponent =
        CreateDefaultSubobject<UIsdkGrabbableComponent>(TEXT("TestGrabbableComponent"));
    TestGrabbableComponent->SetupAttachment(Root);
    TestGrabbableComponent->SetGrabInputMethodAllowed(EIsdkGrabInputMethod::Pinch, true);
    TestGrabbableComponent->SetGrabInputMethodAllowed(EIsdkGrabInputMethod::Palm, true);

    TestGrabTransformerComponent =
        CreateDefaultSubobject<UIsdkGrabTransformerComponent>(TEXT("TestGrabTransformerComponent"));
    TestGrabbableComponent->SetGrabTransformer(TestGrabTransformerComponent);
  }

  static bool Setup()
  {
    // No getting the level dirty
    FActorSpawnParameters ActorParameters{};
    ActorParameters.bNoFail = true;
    UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();

    const auto TestActor = TestWorld->SpawnActor<AIsdkTestGrabInteractableActor>(
        AIsdkTestGrabInteractableActor::StaticClass(), ActorParameters);

    return ensureMsgf(
        TestActor, TEXT("Failed to spawn test actor: AIsdkTestGrabInteractableActor"));
  }

  static AIsdkTestGrabInteractableActor& Get()
  {
    const UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();
    check(TestWorld);
    AIsdkTestGrabInteractableActor* Instance =
        Cast<AIsdkTestGrabInteractableActor, AActor>(UGameplayStatics::GetActorOfClass(
            TestWorld, AIsdkTestGrabInteractableActor::StaticClass()));
    checkf(Instance, TEXT("Failed to cast actor to test object"));
    return *Instance;
  }

  void SetColliderMode(EIsdkGrabbableColliderMode NewMode, float NewSize)
  {
    checkf(TestGrabbableComponent, TEXT("TestGrabbableComponent invalid"));
    TestGrabbableComponent->SetColliderMode(NewMode, NewSize);
    TestGrabbableComponent->SetupCollider();
  }

  void SetGrabInput(EIsdkGrabInputMethod NewGrabMethod, bool bNewValue)
  {
    checkf(TestGrabbableComponent, TEXT("TestGrabbableComponent invalid"));
    TestGrabbableComponent->SetGrabInputMethodAllowed(NewGrabMethod, bNewValue);
  }

  UIsdkGrabbableComponent* GetGrabbableComponent()
  {
    checkf(TestGrabbableComponent, TEXT("TestGrabberComponent invalid"));
    return TestGrabbableComponent;
  }

 private:
  UPROPERTY()
  UIsdkGrabbableComponent* TestGrabbableComponent{};

  UPROPERTY()
  UIsdkGrabTransformerComponent* TestGrabTransformerComponent{};
};
