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
#include "IsdkHandData.h"
#include "Components/ActorComponent.h"
#include "StructTypes.h"
#include "IsdkInputActionsRigComponent.generated.h"

class UInputAction;

/**
 *
 */
UCLASS(ClassGroup = (InteractionSDK))
class OCULUSINTERACTIONPREBUILTS_API UIsdkInputActionsRigComponent : public UActorComponent
{
  GENERATED_BODY()
 public:
  UIsdkInputActionsRigComponent();

  // must be called from Target's constructor.
  void SetSubobjectPropertyDefaults(EIsdkHandedness InHandedness);

#if WITH_EDITOR
  virtual void CheckForErrors() override;
#endif

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> SelectAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> SelectStrengthAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> GrabSelectAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> GrabSelectStrengthAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> PinchGrabAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> PalmGrabAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> AButtonDownAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> BButtonDownAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> XButtonDownAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> YButtonDownAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> AButtonTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> BButtonTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> XButtonTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> YButtonTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> LeftMenuButtonDownAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> LeftFrontTriggerTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> LeftFrontTriggerAxisAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> RightFrontTriggerTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> RightFrontTriggerAxisAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> LeftGripTriggerAxisAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> RightGripTriggerAxisAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> LeftThumbstickTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> LeftThumbstickXAxisAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> LeftThumbstickYAxisAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> RightThumbstickTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> RightThumbstickXAxisAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> RightThumbstickYAxisAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> LeftPanelTouchedAction;

  UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = InteractionSDK)
  TObjectPtr<UInputAction> RightPanelTouchedAction;
};
