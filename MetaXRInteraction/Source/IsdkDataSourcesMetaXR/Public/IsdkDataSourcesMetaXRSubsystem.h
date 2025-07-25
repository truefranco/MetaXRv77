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
#include "DataSources/IsdkIHmdDataSource.h"
#include "DataSources/IsdkFromMetaXRControllerDataSource.h"
#include "DataSources/IsdkFromMetaXRHandDataSource.h"
#include "Subsystem/IsdkITrackingDataSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "IsdkDataSourcesMetaXRSubsystem.generated.h"

/**
 * @class UIsdkDataSourcesMetaXRSubsystem
 * @brief Manages the setup and lifecycle of data sources from the MetaXR plugin, such as hands,
 * controllers, and HMDs, within the Interaction SDK.
 *
 * This subsystem automates the creation and management of data sources originated from the MetaXR
 * plugin which are created through the UIsdkTrackedDataSourceRigComponent, making it easier to
 * integrate tracking into your project without needing to manually handle the setup.
 *
 * @see UIsdkFromMetaXRHandDataSource
 * @see UIsdkFromMetaXRControllerDataSource
 * @see UIsdkFromMetaXRHmdDataSource
 * @addtogroup InteractionSDK
 */
UCLASS()
class ISDKDATASOURCESMETAXR_API UIsdkDataSourcesMetaXRSubsystem : public UTickableWorldSubsystem,
                                                                  public IIsdkITrackingDataSubsystem
{
  GENERATED_BODY()

  UIsdkDataSourcesMetaXRSubsystem();
  virtual void Initialize(FSubsystemCollectionBase& Collection) override;
  virtual void Tick(float DeltaTime) override;
  virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

 public:
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
   * @brief Checks whether the MetaXR subsystem is enabled.
   * @return bool True if the subsystem is active, false otherwise.
   *
   * This is an internal check used to ensure that the MetaXR plugin is properly set up before
   * creating data sources.
   */
  virtual bool IsEnabled_Implementation() override;

  virtual EControllerHandBehavior GetControllerHandBehavior() override;
  virtual void SetControllerHandBehavior(EControllerHandBehavior ControllerHandBehavior) override;
  virtual FIsdkTrackedDataSubsystem_ControllerHandsBehaviorChangedDelegate*
  GetControllerHandBehaviorChangedDelegate() override;

  virtual TStatId GetStatId() const override;

  virtual bool IsHoldingAController();

 private:
  EControllerHandBehavior CurrentControllerHandBehavior;
  FIsdkTrackedDataSubsystem_ControllerHandsBehaviorChangedDelegate
      ControllerHandsBehaviorChangedDelegate;

  /*
   * A cached value indicating whether a controller is held in at least one hand.  We cache this
   * to avoid expensive calls into the meta xr module instance.
   */
  bool bIsHoldingAController = true;
};
