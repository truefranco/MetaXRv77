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
#include "Components/ActorComponent.h"
#include "Subsystem/IsdkITrackingDataSubsystem.h"
#include "DataSources/IsdkFromOpenXRControllerDataSource.h"
#include "DataSources/IsdkFromOpenXRHandDataSource.h"
#include "DataSources/IsdkFromOpenXRHmdDataSource.h"
#include "Subsystems/WorldSubsystem.h"
#include "IsdkDataSourcesOpenXRSubsystem.generated.h"

/**
 * @class UIsdkDataSourcesOpenXRSubsystem
 * @brief Manages the setup and lifecycle of OpenXR data sources, such as hands, controllers, and
 * HMDs, within the Interaction SDK.
 *
 * This subsystem automates the creation and management of OpenXR data sources which are created
 * through the UIsdkTrackedDataSourceRigComponent, making it easier to integrate OpenXR tracking
 * into your project without needing to manually handle the setup.
 *
 * @see UIsdkFromOpenXRHandDataSource
 * @see UIsdkFromOpenXRControllerDataSource
 * @see UIsdkFromOpenXRHmdDataSource
 * @addtogroup InteractionSDK
 */
UCLASS(ClassGroup = (InteractionSDK))
class ISDKDATASOURCESOPENXR_API UIsdkDataSourcesOpenXRSubsystem : public UWorldSubsystem,
                                                                  public IIsdkITrackingDataSubsystem
{
  GENERATED_BODY()

 public:
  virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

  /**
   * @brief Creates a hand data source component.
   * @param SourceMotionController The motion controller to which the hand data source will be
   * attached.
   * @param Handedness The handedness to set in the created component
   * @return FIsdkTrackingDataSources Struct containing references to the created hand data sources.
   *
   * This method handles the creation and initialization of hand tracking data sources. It’s
   * managed internally and is called by UIsdkTrackedDataSourceRigComponent.
   */
  virtual FIsdkTrackingDataSources GetOrCreateHandDataSourceComponent_Implementation(
      UMotionControllerComponent* SourceMotionController,
      EIsdkHandedness Handedness) override;
  /**
   * @brief Creates a controller data source component.
   * @param SourceMotionController The motion controller to which the controller data source will
   * be attached.
   * @param Handedness The handedness to set in the created component
   * @return FIsdkTrackingDataSources Struct containing references to the created controller data
   * sources.
   *
   * This method manages the setup of controller tracking data sources. It’s
   * managed internally and is called by UIsdkTrackedDataSourceRigComponent.
   */
  virtual FIsdkTrackingDataSources GetOrCreateControllerDataSourceComponent_Implementation(
      UMotionControllerComponent* SourceMotionController,
      EIsdkHandedness Handedness) override;
  /**
   * @brief Creates an HMD data source component.
   * @param TrackingSpaceRoot The actor representing the root of the HMD tracking space.
   * @return TScriptInterface<IIsdkIHmdDataSource> Interface for the created HMD data source.
   *
   * This method is responsible for setting up head tracking in your VR environment. Like other
   * methods in this subsystem, it’s handled automatically and is called by UIsdkRigComponent.
   */
  virtual TScriptInterface<IIsdkIHmdDataSource> GetOrCreateHmdDataSourceComponent_Implementation(
      AActor* TrackingSpaceRoot) override;
  /**
   * @brief Checks whether the OpenXR subsystem is enabled.
   * @return bool True if the subsystem is active, false otherwise.
   *
   * This is an internal check used to ensure that OpenXR is properly set up before creating data
   * sources.
   */
  virtual bool IsEnabled_Implementation() override;
  virtual EControllerHandBehavior GetControllerHandBehavior() override;
  virtual void SetControllerHandBehavior(EControllerHandBehavior ControllerHandBehavior) override;
  virtual FIsdkTrackedDataSubsystem_ControllerHandsBehaviorChangedDelegate*
  GetControllerHandBehaviorChangedDelegate() override;

 private:
  EControllerHandBehavior CurrentControllerHandBehavior;
  FIsdkTrackedDataSubsystem_ControllerHandsBehaviorChangedDelegate
      ControllerHandsBehaviorChangedDelegate;

  bool bIsHoldingAController = true;
};
