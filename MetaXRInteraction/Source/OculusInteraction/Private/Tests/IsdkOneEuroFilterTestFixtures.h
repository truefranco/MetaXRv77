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
#include "DataSources/IsdkExternalHandDataSource.h"
#include "DataSources/IsdkOneEuroFilterDataModifier.h"
#include "isdk_api/isdk_api.hpp"
#include "Subsystem/IsdkITrackingDataSubsystem.h"
#include "IsdkOneEuroFilterTestFixtures.generated.h"

UCLASS()
class OCULUSINTERACTION_API UIsdkOneEuroFilterTestFixtures : public UObject
{
  GENERATED_BODY()
 public:
  void SetUp()
  {
    ExternalHandDataSource = NewObject<UIsdkExternalHandDataSource>();
    UIsdkHandData* HandData = NewObject<UIsdkHandData>();

    // Set the joint rotations on the HandData object to Identity
    HandData->SetJointsToIdentity();

    ExternalHandDataSource->SetHandData(HandData, FTransform::Identity);
    // Update the API instance with the hand data
    ExternalHandDataSource->GetApiIHandDataSource()->updateData();

    OneEuroFilterDataModifier = NewObject<UIsdkOneEuroFilterDataModifier>();
    // Set tracking data source on OneEuroFilter
    OneEuroFilterDataModifier->SetInputDataSource(ExternalHandDataSource);
  }

 public:
  UPROPERTY()
  UIsdkOneEuroFilterDataModifier* OneEuroFilterDataModifier;
  UPROPERTY()
  UIsdkExternalHandDataSource* ExternalHandDataSource;
};
