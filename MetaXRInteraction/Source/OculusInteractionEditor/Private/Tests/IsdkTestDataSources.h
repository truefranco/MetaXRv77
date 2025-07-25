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
#include "IsdkTestFakes.h"
#include "DataSources/IsdkExternalHandDataSource.h"
#include "DataSources/IsdkOneEuroFilterDataModifier.h"
#include "Kismet/GameplayStatics.h"
#include "Editor.h"

#include "IsdkTestDataSources.generated.h"

UCLASS()
class OCULUSINTERACTIONEDITOR_API AIsdkTestDataSourcesActor : public AActor
{
  GENERATED_BODY()
 public:
  UPROPERTY()
  UIsdkFakeHandDataSource* TestDataSourceExternal{};

  UPROPERTY()
  UIsdkOneEuroFilterDataModifier* TestDataModifierOneEuro{};

  AIsdkTestDataSourcesActor()
  {
    const auto Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
    SetRootComponent(Root);

    TestDataSourceExternal =
        CreateDefaultSubobject<UIsdkFakeHandDataSource>(FName("TestDataSourceExternal"));
    TestDataModifierOneEuro =
        CreateDefaultSubobject<UIsdkOneEuroFilterDataModifier>(FName("TestDataModifierOneEuro"));
  }

  static bool SetUp()
  {
    // No getting the level dirty
    FActorSpawnParameters ActorParameters{};
    ActorParameters.bNoFail = true;
    UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();

    const auto TestActor = TestWorld->SpawnActor<AIsdkTestDataSourcesActor>(
        AIsdkTestDataSourcesActor::StaticClass(), ActorParameters);
    return ensure(TestActor);
  }

  static AIsdkTestDataSourcesActor& Get()
  {
    const UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();
    check(TestWorld);
    AIsdkTestDataSourcesActor* Instance = Cast<AIsdkTestDataSourcesActor, AActor>(
        UGameplayStatics::GetActorOfClass(TestWorld, AIsdkTestDataSourcesActor::StaticClass()));
    check(Instance);
    return *Instance;
  }

  static bool TryGetChecked(AIsdkTestDataSourcesActor*& Instance)
  {
    const UWorld* TestWorld = GEditor->GetPIEWorldContext()->World();
    check(TestWorld);
    Instance = Cast<AIsdkTestDataSourcesActor, AActor>(
        UGameplayStatics::GetActorOfClass(TestWorld, AIsdkTestDataSourcesActor::StaticClass()));
    const bool bIsValid = IsValid(Instance);
    check(bIsValid);
    return bIsValid;
  }
};
