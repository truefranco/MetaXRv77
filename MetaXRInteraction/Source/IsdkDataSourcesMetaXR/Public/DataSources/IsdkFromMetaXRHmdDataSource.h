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
#include "UObject/Object.h"
#include "Components/ActorComponent.h"
#include "IsdkFromMetaXRHmdDataSource.generated.h"

/**
 * @class UIsdkFromMetaXRHmdDataSource
 * @brief Manages HMD tracking data from the MetaXR plugin and helps integrate into your VR project.
 *
 * This class facilitates the integration of HMD tracking data from the MetaXR plugin by providing
 * access to essential information such as the headset's poses and tracking state. It ensures that
 * this data is updated each frame, maintaining its relevance throughout the application's
 * lifecycle. An instance of this data source is created in the Interaction SDK's prebuilt rig
 * component via UIsdkTrackedDataSourceRigComponent.
 *
 * @see IIsdkIHmdDataSource
 * @addtogroup InteractionSDK
 */
UCLASS(
    ClassGroup = (InteractionSDK),
    meta = (BlueprintSpawnableComponent, DisplayName = "ISDK From MetaXR HMD Data Source"))
class ISDKDATASOURCESMETAXR_API UIsdkFromMetaXRHmdDataSource : public UActorComponent,
                                                               public IIsdkIHmdDataSource
{
  GENERATED_BODY()

  /**
   * @brief Returns the current tracking status of the MetaXR HMD data source
   * @return bool Whether or not the MetaXR HMD data source is being actively tracked
   */
  virtual bool IsHmdTracked_Implementation() override;

  /**
   * @brief Retrieves the current HMD pose.
   * @param HmdPose Returned transform of the HMD, passed as reference, in worldspace
   * @param IsTracked Returned status of the HMD, passed as reference true if being tracked
   *
   * This is how to get both the tracking state of the OpenXR HMD data source as well as its current
   * transform in worldspace
   */
  virtual void GetHmdPose_Implementation(FTransform& HmdPose, bool& IsTracked) override;

  /**
   * @brief Retrieves the current HMD pose.
   * @param HmdRelativePose Returned transform of the HMD, passed as reference, in relative space
   * @param IsTracked Returned status of the HMD, passed as reference true if being tracked
   *
   * This is how to get both the tracking state of the OpenXR HMD data source as well as its current
   * transform in relative space
   */
  virtual void GetRelativeHmdPose_Implementation(FTransform& HmdRelativePose, bool& IsTracked)
      override;
};
