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

#include "DataSources/IsdkExternalHandDataModifier.h"
#include "DataSources/IsdkIRootPose.h"

void UIsdkExternalHandDataModifier::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (IsValid(InputDataSource) && IsValid(HandData))
  {
    const UIsdkHandData* HandJointsUE = IIsdkIHandJoints::Execute_GetHandData(InputDataSource);
    // Set the joints on the HandData object
    HandData->SetJoints(HandJointsUE->GetJointPoses());
    // Modify the hand data
    OnModify(HandData);
    // Convert the root pose and set the hand data
    const FTransform RootPoseUE = IIsdkIRootPose::Execute_GetRootPose(InputDataSource);
    SetHandData(HandData, RootPoseUE);
  }
}

void UIsdkExternalHandDataModifier::SetInputDataSource(UIsdkHandDataSource* InInputDataSource)
{
  if (IsValid(InputDataSource))
  {
    RemoveTickPrerequisiteComponent(InputDataSource);
  }

  InputDataSource = InInputDataSource;
  // Ensure that this component ticks after the input component
  if (IsValid(InputDataSource))
  {
    AddTickPrerequisiteComponent(InputDataSource);
  }
}

void UIsdkExternalHandDataModifier::OnModify_Implementation(UIsdkHandData* InputHandData)
{
  OnHandDataModified.Broadcast(this);
}
