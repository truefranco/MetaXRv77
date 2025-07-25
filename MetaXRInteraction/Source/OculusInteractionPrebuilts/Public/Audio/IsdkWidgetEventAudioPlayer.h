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
#include "Widget/IsdkPointableWidget.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "IsdkWidgetEventAudioPlayer.generated.h"

UCLASS(ClassGroup = (InteractionSDK), meta = (BlueprintSpawnableComponent))
class OCULUSINTERACTIONPREBUILTS_API UIsdkWidgetEventAudioPlayer : public UActorComponent
{
  GENERATED_BODY()

 public:
  // Sets default values for this component's properties
  UIsdkWidgetEventAudioPlayer();

  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetSelectedHoveredAudio() const
  {
    return SelectedHoveredAudio;
  }

  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetSelectedHoveredAudio(UAudioComponent* Audio)
  {
    SelectedHoveredAudio = Audio;
  }

  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetSelectedEmptyAudio() const
  {
    return SelectedEmptyAudio;
  }

  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetSelectedEmptyAudio(UAudioComponent* Audio)
  {
    SelectedEmptyAudio = Audio;
  }

  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetUnselectedHoveredAudio() const
  {
    return UnselectedHoveredAudio;
  }

  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetUnselectedHoveredAudio(UAudioComponent* Audio)
  {
    UnselectedHoveredAudio = Audio;
  }

  UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = InteractionSDK)
  UAudioComponent* GetUnselectedEmptyAudio() const
  {
    return UnselectedEmptyAudio;
  }

  UFUNCTION(BlueprintSetter, Category = InteractionSDK)
  void SetUnselectedEmptyAudio(UAudioComponent* Audio)
  {
    UnselectedEmptyAudio = Audio;
  }

  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  void HandleWidgetStateChanged(FIsdkWidgetEvent WidgetEvent);

 private:
  UPROPERTY(
      BlueprintGetter = GetSelectedHoveredAudio,
      BlueprintSetter = SetSelectedHoveredAudio,
      Category = InteractionSDK)
  UAudioComponent* SelectedHoveredAudio;
  UPROPERTY(
      BlueprintGetter = GetSelectedEmptyAudio,
      BlueprintSetter = SetSelectedEmptyAudio,
      Category = InteractionSDK)
  UAudioComponent* SelectedEmptyAudio;
  UPROPERTY(
      BlueprintGetter = GetUnselectedHoveredAudio,
      BlueprintSetter = SetUnselectedHoveredAudio,
      Category = InteractionSDK)
  UAudioComponent* UnselectedHoveredAudio;
  UPROPERTY(
      BlueprintGetter = GetUnselectedEmptyAudio,
      BlueprintSetter = SetUnselectedEmptyAudio,
      Category = InteractionSDK)
  UAudioComponent* UnselectedEmptyAudio;
};
