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

#include "Interaction/Grabbable/IsdkGrabPoseCollection.h"

int FIsdkGrabPoseCollection::FindPose(const TArray<FIsdkGrabPose>& InPoses, int Identifier)
{
  for (size_t i = 0; i < InPoses.Num(); i++)
  {
    if (InPoses[i].Identifier == Identifier)
    {
      return i;
    }
  }
  return INDEX_NONE;
}

void FIsdkGrabPoseCollection::Hover(const FIsdkGrabPose& Pose)
{
  if (FindPose(HoverPoses, Pose.Identifier) == INDEX_NONE)
  {
    HoverPoses.Add(Pose);
  }
}

void FIsdkGrabPoseCollection::Unhover(int Identifier)
{
  int PoseIndex = FindPose(HoverPoses, Identifier);
  if (PoseIndex != INDEX_NONE)
  {
    HoverPoses.RemoveAt(PoseIndex);
  }
}

void FIsdkGrabPoseCollection::Select(const FIsdkGrabPose& Pose)
{
  if (FindPose(SelectPoses, Pose.Identifier) == INDEX_NONE)
  {
    SelectPoses.Add(Pose);
  }
}

void FIsdkGrabPoseCollection::Unselect(int Identifier)
{
  int SelectIndex = FindPose(SelectPoses, Identifier);
  if (SelectIndex != INDEX_NONE)
  {
    SelectPoses.RemoveAt(SelectIndex);
  }
}

void FIsdkGrabPoseCollection::RemovePose(int Identifier)
{
  Unselect(Identifier);
  Unhover(Identifier);
}

void FIsdkGrabPoseCollection::Move(const FIsdkGrabPose& GrabPose)
{
  int PoseIndex = FindPose(HoverPoses, GrabPose.Identifier);
  if (PoseIndex != INDEX_NONE)
  {
    HoverPoses[PoseIndex].Pose = GrabPose.Pose;
  }

  int SelectIndex = FindPose(SelectPoses, GrabPose.Identifier);
  if (SelectIndex != INDEX_NONE)
  {
    SelectPoses[SelectIndex].Pose = GrabPose.Pose;
  }
}
