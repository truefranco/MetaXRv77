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

#include "DataSources/IsdkHandDataModifier.h"

#include "StructTypesPrivate.h"
#include "isdk_api/isdk_api.hpp"

UIsdkHandDataModifier::UIsdkHandDataModifier()
{
  // Tick: only becomes enabled once a source modifier is provided.
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UIsdkHandDataModifier::BeginPlay()
{
  Super::BeginPlay();

  // Always reset to identity, in case invalid data was persisted on the HandData instance.
  HandData->SetJointsToIdentity();
}

void UIsdkHandDataModifier::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (bUpdateInTick)
  {
    ReadHandJointsFromImpl();

    const auto ApiHandDataModifier = GetApiIHandDataModifier();
    if (ApiHandDataModifier)
    {
      ModifiedRootPose =
          StructTypesUtils::ConvertTransform(GetApiIHandDataModifier()->getRootPose());
    }
  }
}

void UIsdkHandDataModifier::SetInputDataSource(UIsdkHandDataSource* InInputDataSource)
{
  if (IsValid(InputDataSource))
  {
    RemoveTickPrerequisiteComponent(InputDataSource);
    SetComponentTickEnabled(false);
  }
  InputDataSource = InInputDataSource;

  // Ensure that this component ticks after the input component
  if (IsValid(InputDataSource))
  {
    AddTickPrerequisiteComponent(InputDataSource);
    SetComponentTickEnabled(true);
    // Push down BoneMappings from new data source
    InputDataSource->GetBoneMaps(OutboundBoneMap, InboundBoneMap);
    PropagateInboundBoneMap();
    PropagateOutboundBoneMap();

    // Copy HandData
    HandData = IIsdkIHandJoints::Execute_GetHandData(InInputDataSource);
  }

  if (IsApiInstanceValid())
  {
    isdk_IHandDataSource* FromDataSource{};
    TryGetApiFromDataSource(FromDataSource);
    GetApiIHandDataModifier()->setFromDataSource(FromDataSource);
  }
}

void UIsdkHandDataModifier::SetRecursiveUpdate(bool bInRecursiveUpdate)
{
  bRecursiveUpdate = bInRecursiveUpdate;
  if (IsApiInstanceValid())
  {
    GetApiIHandDataModifier()->setRecursiveUpdate(bInRecursiveUpdate ? 1 : 0);
  }
}

bool UIsdkHandDataModifier::IsRootPoseValid_Implementation()
{
  return IsValid(InputDataSource);
}

UIsdkConditional* UIsdkHandDataModifier::GetRootPoseConnectedConditional_Implementation()
{
  check(IsValid(InputDataSource));
  return Execute_GetRootPoseConnectedConditional(InputDataSource);
}

UIsdkConditional* UIsdkHandDataModifier::GetRootPoseHighConfidenceConditional_Implementation()
{
  check(IsValid(InputDataSource));
  return Execute_GetRootPoseHighConfidenceConditional(InputDataSource);
}

EIsdkHandedness UIsdkHandDataModifier::GetHandedness_Implementation()
{
  check(IsValid(InputDataSource));
  return Execute_GetHandedness(InputDataSource);
}

bool UIsdkHandDataModifier::IsHandJointDataValid_Implementation()
{
  return IsValid(InputDataSource) && Execute_IsHandJointDataValid(InputDataSource);
}

const UIsdkHandJointMappings* UIsdkHandDataModifier::GetHandJointMappings_Implementation()
{
  check(IsValid(InputDataSource));
  return Execute_GetHandJointMappings(InputDataSource);
}

bool UIsdkHandDataModifier::ShouldUpdateDataSource()
{
  // Only call the native update method if this modifier has been set up with a valid input.
  return Super::ShouldUpdateDataSource() && IsValid(InputDataSource);
}

bool UIsdkHandDataModifier::TryGetApiFromDataSource(isdk_IHandDataSource*& OutFromDataSource) const
{
  if (IsValid(InputDataSource))
  {
    const auto ApiFromDataSource = ValidateAndGetApiDataSource(
        GetInputDataSource(), TEXT("UIsdkHandDataModifier::InputDataSource"));
    OutFromDataSource = ApiFromDataSource->getIHandDataSourceHandle();
    return true;
  }
  OutFromDataSource = nullptr;
  return false;
}
