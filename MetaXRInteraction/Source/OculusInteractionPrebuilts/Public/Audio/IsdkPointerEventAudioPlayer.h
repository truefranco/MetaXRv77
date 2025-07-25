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
#include "Interaction/Pointable/IsdkIPointable.h"
#include "Components/ActorComponent.h"
#include "IsdkPointerEventAudioPlayer.generated.h"

UCLASS(ClassGroup = (InteractionSDK), meta = (BlueprintSpawnableComponent))
class OCULUSINTERACTIONPREBUILTS_API UIsdkPointerEventAudioPlayer : public UActorComponent
{
  GENERATED_BODY()

 public:
  // Sets default values for this component's properties
  UIsdkPointerEventAudioPlayer();

  // Getters
  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  TScriptInterface<IIsdkIPointable> GetPointable() const
  {
    return Pointable;
  }
  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetHoverAudio() const
  {
    return HoverAudio;
  }
  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetUnhoverAudio() const
  {
    return UnhoverAudio;
  }
  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetSelectAudio() const
  {
    return SelectAudio;
  }
  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetUnselectAudio() const
  {
    return UnselectAudio;
  }
  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetMoveAudio() const
  {
    return MoveAudio;
  }
  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetCancelAudio() const
  {
    return CancelAudio;
  }

  // Setters
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetPointable(const TScriptInterface<IIsdkIPointable>& Pointable);
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetHoverAudio(UAudioComponent* Audio)
  {
    HoverAudio = Audio;
  }
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetUnhoverAudio(UAudioComponent* Audio)
  {
    UnhoverAudio = Audio;
  }
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetSelectAudio(UAudioComponent* Audio)
  {
    SelectAudio = Audio;
  }
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetUnselectAudio(UAudioComponent* Audio)
  {
    UnselectAudio = Audio;
  }
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetMoveAudio(UAudioComponent* Audio)
  {
    MoveAudio = Audio;
  }
  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetCancelAudio(UAudioComponent* Audio)
  {
    CancelAudio = Audio;
  }

 protected:
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

 private:
  UPROPERTY(
      BlueprintGetter = GetPointable,
      BlueprintSetter = SetPointable,
      Category = InteractionSDK)
  TScriptInterface<IIsdkIPointable> Pointable;
  UPROPERTY(
      BlueprintGetter = GetHoverAudio,
      BlueprintSetter = SetHoverAudio,
      Category = InteractionSDK)
  UAudioComponent* HoverAudio{};
  UPROPERTY(
      BlueprintGetter = GetUnhoverAudio,
      BlueprintSetter = SetUnhoverAudio,
      Category = InteractionSDK)
  UAudioComponent* UnhoverAudio{};
  UPROPERTY(
      BlueprintGetter = GetSelectAudio,
      BlueprintSetter = SetSelectAudio,
      Category = InteractionSDK)
  UAudioComponent* SelectAudio{};
  UPROPERTY(
      BlueprintGetter = GetUnselectAudio,
      BlueprintSetter = SetUnselectAudio,
      Category = InteractionSDK)
  UAudioComponent* UnselectAudio{};
  UPROPERTY(
      BlueprintGetter = GetMoveAudio,
      BlueprintSetter = SetMoveAudio,
      Category = InteractionSDK)
  UAudioComponent* MoveAudio{};
  UPROPERTY(
      BlueprintGetter = GetCancelAudio,
      BlueprintSetter = SetCancelAudio,
      Category = InteractionSDK)
  UAudioComponent* CancelAudio{};

  UFUNCTION()
  void HandlePointerEvent(const FIsdkInteractionPointerEvent& PointerEvent);
};
