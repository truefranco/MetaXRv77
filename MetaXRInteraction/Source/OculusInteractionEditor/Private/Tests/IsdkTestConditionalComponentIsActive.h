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

#include "Editor.h"

#include "Core/IsdkConditionalComponentIsActive.h"
#include "Kismet/GameplayStatics.h"
#include "IsdkTestConditionalComponentIsActive.generated.h"

UCLASS()
class UIsdkTestConditionalComponentIsActive_Component : public UActorComponent
{
  GENERATED_BODY()

 public:
  UIsdkTestConditionalComponentIsActive_Component()
  {
    bAutoActivate = true;
  }
};

UCLASS()
class AIsdkTestConditionalComponentIsActive_Actor : public AActor
{
  GENERATED_BODY()

 public:
  AIsdkTestConditionalComponentIsActive_Actor()
  {
    OtherComponent =
        CreateDefaultSubobject<UIsdkTestConditionalComponentIsActive_Component>("OtherComponent");
    Conditional = CreateDefaultSubobject<UIsdkConditionalComponentIsActive>("Conditional");
  }

  virtual void PostInitializeComponents() override
  {
    Super::PostInitializeComponents();
    Conditional->SetComponent(OtherComponent);
    OtherComponent->SetActive(true);
  }

  static AIsdkTestConditionalComponentIsActive_Actor* Get()
  {
    const UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();
    check(TestWorld);
    AIsdkTestConditionalComponentIsActive_Actor* Instance =
        Cast<AIsdkTestConditionalComponentIsActive_Actor, AActor>(UGameplayStatics::GetActorOfClass(
            TestWorld, AIsdkTestConditionalComponentIsActive_Actor::StaticClass()));
    check(Instance);
    return Instance;
  }

  UPROPERTY()
  UIsdkConditionalComponentIsActive* Conditional{};
  UPROPERTY()
  UActorComponent* OtherComponent{};
};
