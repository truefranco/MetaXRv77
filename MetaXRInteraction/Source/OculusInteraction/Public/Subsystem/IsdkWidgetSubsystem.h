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
#include "Subsystems/WorldSubsystem.h"
#include "Widget/IsdkWidget.h"
#include "IsdkWidgetSubsystem.generated.h"

UCLASS(Abstract)
class OCULUSINTERACTION_API UIsdkWidgetSubsystemBase : public UWorldSubsystem
{
  GENERATED_BODY()
};

class FIsdkWidgetSubsystemCollection : public FSubsystemCollection<UIsdkWidgetSubsystemBase>
{
};

USTRUCT(BlueprintType)
struct OCULUSINTERACTION_API FIsdkVirtualUserInfo
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  int32 VirtualUserIndex = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InteractionSDK)
  int32 PointerIndex = 0;
};

/**
 * Holds information specific to interacting with Slate
 */
UCLASS()
class OCULUSINTERACTION_API UIsdkWidgetSubsystem : public UIsdkWidgetSubsystemBase
{
  GENERATED_BODY()

 public:
  UIsdkWidgetSubsystem();
  virtual void BeginDestroy() override;

  // USubsystem implementation Begin
  virtual void Initialize(FSubsystemCollectionBase& Collection) override;
  // USubsystem implementation End

  /**
   * Static helper function to get an Interaction SDK subsystem from a world.
   * Will fail (via check) if the subsystem does not exist on the given world.
   */
  static UIsdkWidgetSubsystem& Get(const UWorld* InWorld)
  {
    checkf(IsValid(InWorld), TEXT("World passed to UIsdkWidgetSubsystem::Get was nullptr"));
    UIsdkWidgetSubsystem* Instance = InWorld->GetSubsystem<UIsdkWidgetSubsystem>();
    checkf(
        IsValid(Instance),
        TEXT("Failed to find a UIsdkWidgetSubsystem for the world %s"),
        *InWorld->GetDebugDisplayName());

    return *Instance;
  }

  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void RegisterVirtualUserInfo(UObject* Interactor, FIsdkVirtualUserInfo VirtualUserInfo);
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void UnregisterVirtualUserInfo(UObject* Interactor);
  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  const FIsdkVirtualUserInfo& GetVirtualUserInfo(UObject* Interactor);

  FIsdkWidgetVirtualUserState* FindOrCreateInteractorVirtualUserState(
      const FIsdkVirtualUserPointerEvent& PointerEvent);
  void DestroyInteractorVirtualUserState(UObject* Interactor);

#pragma region Default Collision Info
  static ECollisionChannel GetUninitializedChannel();
  ECollisionChannel GetDefaultCollisionChannel();
  ECollisionChannel GetDefaultHoverChannel();
#pragma endregion Default Collision Info

#pragma region Debug Drawing
  bool IsDebugDrawEnabled();
  bool ShouldDebugDrawMoveEvents();
#pragma endregion Debug Drawing

 private:
  TArray<FIsdkWidgetVirtualUserState> VirtualUserIndexInteractors;
  TMap<UObject*, FIsdkVirtualUserInfo>
      VirtualUserInfoMap; // TODO: turn this into a ticketed collection
};
