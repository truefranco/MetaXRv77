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

#include "IsdkHandPoseData.h"
#include "IsdkFunctionLibrary.h"
#include "IsdkHandMeshComponent.h"

UIsdkHandPoseData::UIsdkHandPoseData()
{
  HandData = CreateDefaultSubobject<UIsdkHandData>(TEXT("Hand Data"));

  HandJointMapping = CreateDefaultSubobject<UIsdkHandJointMappings>(TEXT("Hand Joint Mapping"));
  HandJointMapping->ThumbJointMappings = UIsdkFunctionLibrary::GetDefaultOpenXRThumbMapping();
  HandJointMapping->FingerJointMappings = UIsdkFunctionLibrary::GetDefaultOpenXRFingerMapping();

  JointNames[0] = FName("XRHand_Palm");
  JointNames[1] = FName("XRHand_Wrist");
  // ----- Thumb
  JointNames[2] = FName("XRHand_ThumbMetacarpal");
  JointNames[3] = FName("XRHand_ThumbProximal");
  JointNames[4] = FName("XRHand_ThumbDistal");
  JointNames[5] = FName("XRHand_ThumbTip");
  // ----- Index
  JointNames[6] = FName("XRHand_IndexMetacarpal");
  JointNames[7] = FName("XRHand_IndexProximal");
  JointNames[8] = FName("XRHand_IndexIntermediate");
  JointNames[9] = FName("XRHand_IndexDistal");
  JointNames[10] = FName("XRHand_IndexTip");
  // ----- Middle
  JointNames[11] = FName("XRHand_MiddleMetacarpal");
  JointNames[12] = FName("XRHand_MiddleProximal");
  JointNames[13] = FName("XRHand_MiddleIntermediate");
  JointNames[14] = FName("XRHand_MiddleDistal");
  JointNames[15] = FName("XRHand_MiddleTip");
  // ----- Ring
  JointNames[16] = FName("XRHand_RingMetacarpal");
  JointNames[17] = FName("XRHand_RingProximal");
  JointNames[18] = FName("XRHand_RingIntermediate");
  JointNames[19] = FName("XRHand_RingDistal");
  JointNames[20] = FName("XRHand_RingTip");
  // ----- Pinky
  JointNames[21] = FName("XRHand_LittleMetacarpal");
  JointNames[22] = FName("XRHand_LittleProximal");
  JointNames[23] = FName("XRHand_LittleIntermediate");
  JointNames[24] = FName("XRHand_LittleDistal");
  JointNames[25] = FName("XRHand_LittleTip");
}

void UIsdkHandPoseData::SetRotationFromSkeleton(USkinnedAsset* SkinnedAsset)
{
  FReferenceSkeleton& RefSkeleton = SkinnedAsset->GetRefSkeleton();
  auto& RefBonePoses = RefSkeleton.GetRefBonePose();

  TArray<FTransform> ComponentSpacePoses = TArray<FTransform>();
  ComponentSpacePoses.SetNum(RefBonePoses.Num());
  TArray<uint16> RequiredBones = TArray<uint16>();
  RequiredBones.SetNum(RefBonePoses.Num());

  for (int BoneNameIndex = 0; BoneNameIndex < RefBonePoses.Num(); ++BoneNameIndex)
  {
    RequiredBones[BoneNameIndex] = BoneNameIndex;
  }

  SkinnedAsset->FillComponentSpaceTransforms(RefBonePoses, RequiredBones, ComponentSpacePoses);

  TArray<FTransform>& HandJoints = HandData->GetJointPoses();
  for (int BoneNameIndex = 0; BoneNameIndex < static_cast<int>(EIsdkHandBones::EHandBones_MAX);
       ++BoneNameIndex)
  {
    auto BoneIndex = RefSkeleton.FindBoneIndex(JointNames[BoneNameIndex]);
    auto BonePose = ComponentSpacePoses[BoneIndex];
    HandJoints[BoneNameIndex] = BonePose;
  }
  SetDirty();
}

void UIsdkHandPoseData::SetRotationFromVisual(UIsdkHandMeshComponent* HandMesh)
{
  if (!IsValid(HandMesh))
  {
    UE_LOG(LogOculusInteraction, Warning, TEXT("Hand Mesh is invalid"));
    return;
  }
  auto InputDataSource = HandMesh->GetJointsDataSource();
  if (!IsValid(InputDataSource.GetObject()))
  {
    UE_LOG(LogOculusInteraction, Warning, TEXT("Joint data source is invalid"));
    return;
  }
  const UIsdkHandData* SourceHandData =
      InputDataSource->Execute_GetHandData(InputDataSource.GetObject());
  TArray<FTransform>& HandJointPoses = HandData->GetJointPoses();
  const TArray<FTransform>& SourceJointPoses = SourceHandData->GetJointPoses();
  for (int BoneIndex = 0; BoneIndex < static_cast<int>(EIsdkHandBones::EHandBones_MAX); ++BoneIndex)
  {
    HandJointPoses[BoneIndex] = SourceJointPoses[BoneIndex];
  }

  EIsdkHandedness VisualHandedness;
  HandMesh->GetHandednessFromDataSource(VisualHandedness);
  Handedness = VisualHandedness;

  SetDirty();
}

void UIsdkHandPoseData::SetRotationFromPoseWithName(UPoseAsset* Pose, FName Name)
{
  auto LocalPoseTransforms = TArray<FTransform>();
#if WITH_EDITOR
  auto PoseIndex = Pose->GetPoseIndexByName(Name);
  Pose->GetFullPose(PoseIndex, LocalPoseTransforms);
  TArray<FTransform>& HandJointPoses = HandData->GetJointPoses();
  for (int BoneNameIndex = 1; BoneNameIndex < static_cast<int>(EIsdkHandBones::EHandBones_MAX);
       ++BoneNameIndex)
  {
    auto BoneName = JointNames[BoneNameIndex];
    if (BoneName == NAME_None)
    {
      auto JointName = JointNames[BoneNameIndex].ToString();
      UE_LOG(LogOculusInteraction, Warning, TEXT("Could not find node %s"), *JointName);
      return;
    }
    auto BonePose = Pose->GetComponentSpaceTransform(BoneName, LocalPoseTransforms);
    HandJointPoses[BoneNameIndex] = BonePose;
  }
  HandJointPoses[0] = FTransform::Identity;
  SetDirty();
#endif
}
