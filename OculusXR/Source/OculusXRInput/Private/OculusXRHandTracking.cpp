// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRHandTracking.h"

#include "Animation/Skeleton.h"
#include "BoneWeights.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "IOculusXRInputModule.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/CoreDelegates.h"
#include "Model.h"
#include "OculusXRHMD.h"
#include "OpenXR/OculusXROpenXRUtilities.h"
#include "OculusXRInputModule.h"
#include "OculusXRInputState.h"

#define OCULUS_TO_UE4_SCALE 100.0f

namespace OculusXRInput
{

	static FInputDeviceId GetDeviceID(int32 ControllerId)
	{
		IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
		FPlatformUserId InPlatformUser = FGenericPlatformMisc::GetPlatformUserForUserIndex(ControllerId);
		FInputDeviceId InDeviceId = INPUTDEVICEID_NONE;
		DeviceMapper.RemapControllerIdToPlatformUserAndDevice(ControllerId, InPlatformUser, InDeviceId);
		return InDeviceId;
	}

	static bool IsPluginWrapperAvailible()
	{
		return ((IOculusXRHMDModule::IsAvailable() && FOculusXRHMDModule::GetPluginWrapper().IsInitialized()));
	}

	FQuat FOculusHandTracking::GetBoneRotation(const int32 ControllerIndex, const EOculusXRHandType DeviceHand, const EOculusXRBone BoneId)
	{
		FQuat Rotation = FQuat::Identity;
		if (BoneId <= EOculusXRBone::Invalid && BoneId >= EOculusXRBone::Bone_Max)
		{
			return Rotation;
		}

#if OCULUS_INPUT_SUPPORTED_PLATFORMS
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			TSharedPtr<FOculusXRInput> OculusXRInputModule = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
			if (OculusXRInputModule.IsValid())
			{
				const FInputDeviceId InDeviceId = GetDeviceID(ControllerIndex);
				TArray<FOculusControllerPair> ControllerPairs = OculusXRInputModule.Get()->ControllerPairs;
				for (const FOculusControllerPair& HandPair : ControllerPairs)
				{
					if (HandPair.DeviceId == InDeviceId)
					{
						if (DeviceHand != EOculusXRHandType::None)
						{
							ovrpHand Hand = DeviceHand == EOculusXRHandType::HandLeft ? ovrpHand_Left : ovrpHand_Right;
							const FOculusHandControllerState& HandState = HandPair.HandControllerStates[Hand];
							int32 OvrBoneId = ToOvrBone(BoneId);
							Rotation = HandState.BoneRotations[OvrBoneId];
							break;
						}
					}
				}
			}
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			if (DeviceHand != EOculusXRHandType::None)
			{
				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				const FOculusHandControllerState& HandState = InputModule->GetHandTrackingOpenXRExtension()->HandControllerStates[DeviceHand];
				const EHandBoneId HandBoneId = ToHandBone(BoneId);
				Rotation = HandState.BoneRotations[static_cast<int32>(HandBoneId)];
			}
		}
#endif

		return Rotation;
	}

	float FOculusHandTracking::GetHandScale(const int32 ControllerIndex, const EOculusXRHandType DeviceHand)
	{
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			TSharedPtr<FOculusXRInput> OculusXRInputModule = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
			if (OculusXRInputModule.IsValid())
			{
				const FInputDeviceId InDeviceId = GetDeviceID(ControllerIndex);
				TArray<FOculusControllerPair> ControllerPairs = OculusXRInputModule.Get()->ControllerPairs;
				for (const FOculusControllerPair& HandPair : ControllerPairs)
				{
					if (HandPair.DeviceId == InDeviceId)
					{
						if (DeviceHand != EOculusXRHandType::None)
						{
							ovrpHand Hand = DeviceHand == EOculusXRHandType::HandLeft ? ovrpHand_Left : ovrpHand_Right;
							return HandPair.HandControllerStates[Hand].HandScale;
						}
					}
				}
			}
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			if (DeviceHand != EOculusXRHandType::None)
			{
				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				const FOculusHandControllerState& HandState = InputModule->GetHandTrackingOpenXRExtension()->HandControllerStates[DeviceHand];
				return HandState.HandScale;
			}
		}
#endif
		return 1.0f;
	}

	EOculusXRTrackingConfidence FOculusHandTracking::GetTrackingConfidence(const int32 ControllerIndex, const EOculusXRHandType DeviceHand)
	{
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			TSharedPtr<FOculusXRInput> OculusXRInputModule = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
			if (OculusXRInputModule.IsValid())
			{
				const FInputDeviceId InDeviceId = GetDeviceID(ControllerIndex);
				TArray<FOculusControllerPair> ControllerPairs = OculusXRInputModule.Get()->ControllerPairs;
				for (const FOculusControllerPair& HandPair : ControllerPairs)
				{
					if (HandPair.DeviceId == InDeviceId)
					{
						if (DeviceHand != EOculusXRHandType::None)
						{
							ovrpHand Hand = DeviceHand == EOculusXRHandType::HandLeft ? ovrpHand_Left : ovrpHand_Right;
							return HandPair.HandControllerStates[Hand].TrackingConfidence;
						}
					}
				}
			}
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			if (DeviceHand != EOculusXRHandType::None)
			{
				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				const FOculusHandControllerState& HandState = InputModule->GetHandTrackingOpenXRExtension()->HandControllerStates[DeviceHand];
				return HandState.TrackingConfidence;
			}
		}
#endif
		return EOculusXRTrackingConfidence::Low;
	}

	EOculusXRTrackingConfidence FOculusHandTracking::GetFingerTrackingConfidence(const int32 ControllerIndex, const EOculusXRHandType DeviceHand, const EOculusHandAxes Finger)
	{
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			TSharedPtr<FOculusXRInput> OculusXRInputModule = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
			if (OculusXRInputModule.IsValid())
			{
				const FInputDeviceId InDeviceId = GetDeviceID(ControllerIndex);
				TArray<FOculusControllerPair> ControllerPairs = OculusXRInputModule.Get()->ControllerPairs;
				for (const FOculusControllerPair& HandPair : ControllerPairs)
				{
					if (HandPair.DeviceId == InDeviceId)
					{
						if (DeviceHand != EOculusXRHandType::None)
						{
							ovrpHand Hand = DeviceHand == EOculusXRHandType::HandLeft ? ovrpHand_Left : ovrpHand_Right;
							return HandPair.HandControllerStates[Hand].FingerConfidences[(int)Finger];
						}
					}
				}
			}
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			if (DeviceHand != EOculusXRHandType::None)
			{
				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				const FOculusHandControllerState& HandState = InputModule->GetHandTrackingOpenXRExtension()->HandControllerStates[DeviceHand];
				return HandState.FingerConfidences[(int)Finger];
			}
		}
		return EOculusXRTrackingConfidence::Low;
	}

	FTransform FOculusHandTracking::GetPointerPose(const int32 ControllerIndex, const EOculusXRHandType DeviceHand, const float WorldToMeters)
	{
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			TSharedPtr<FOculusXRInput> OculusXRInputModule = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
			if (OculusXRInputModule.IsValid())
			{
				const FInputDeviceId InDeviceId = GetDeviceID(ControllerIndex);
				TArray<FOculusControllerPair> ControllerPairs = OculusXRInputModule.Get()->ControllerPairs;
				for (const FOculusControllerPair& HandPair : ControllerPairs)
				{
					if (HandPair.DeviceId == InDeviceId)
					{
						if (DeviceHand != EOculusXRHandType::None)
						{
							ovrpHand Hand = DeviceHand == EOculusXRHandType::HandLeft ? ovrpHand_Left : ovrpHand_Right;
							FTransform PoseTransform = HandPair.HandControllerStates[Hand].PointerPose;
							PoseTransform.SetLocation(PoseTransform.GetLocation() * WorldToMeters);
							return PoseTransform;
						}
					}
				}
			}
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			if (DeviceHand != EOculusXRHandType::None)
			{
				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				const FOculusHandControllerState& HandState = InputModule->GetHandTrackingOpenXRExtension()->HandControllerStates[DeviceHand];
				FTransform PoseTransform = HandState.PointerPose;
				PoseTransform.SetLocation(PoseTransform.GetLocation() * WorldToMeters);
				return PoseTransform;
			}
		}
#endif

		return FTransform();
	}

	bool FOculusHandTracking::IsPointerPoseValid(const int32 ControllerIndex, const EOculusXRHandType DeviceHand)
	{
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			TSharedPtr<FOculusXRInput> OculusXRInputModule = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
			if (OculusXRInputModule.IsValid())
			{
				const FInputDeviceId InDeviceId = GetDeviceID(ControllerIndex);
				TArray<FOculusControllerPair> ControllerPairs = OculusXRInputModule.Get()->ControllerPairs;
				for (const FOculusControllerPair& HandPair : ControllerPairs)
				{
					if (HandPair.DeviceId == InDeviceId)
					{
						if (DeviceHand != EOculusXRHandType::None)
						{
							ovrpHand Hand = DeviceHand == EOculusXRHandType::HandLeft ? ovrpHand_Left : ovrpHand_Right;
							return HandPair.HandControllerStates[Hand].bIsPointerPoseValid;
						}
					}
				}
			}
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			if (DeviceHand != EOculusXRHandType::None)
			{
				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				const FOculusHandControllerState& HandState = InputModule->GetHandTrackingOpenXRExtension()->HandControllerStates[DeviceHand];
				return HandState.bIsPointerPoseValid;
			}
		}
#endif

		return false;
	}

	bool FOculusHandTracking::IsHandTrackingEnabled()
	{
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
			if (IOculusXRHMDModule::IsAvailable() && FOculusXRHMDModule::GetPluginWrapper().IsInitialized())
			{
				ovrpBool result;
				FOculusXRHMDModule::GetPluginWrapper().GetHandTrackingEnabled(&result);
				return result == ovrpBool_True;
			}
#endif // OCULUS_INPUT_SUPPORTED_PLATFORMS
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
			return InputModule->GetHandTrackingOpenXRExtension()->IsHandTrackingActive();
		}

		return false;
	}

	bool FOculusHandTracking::IsHandDominant(const int32 ControllerIndex, const EOculusXRHandType DeviceHand)
	{
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			TSharedPtr<FOculusXRInput> OculusXRInputModule = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
			if (OculusXRInputModule.IsValid())
			{
				const FInputDeviceId InDeviceId = GetDeviceID(ControllerIndex);
				TArray<FOculusControllerPair> ControllerPairs = OculusXRInputModule.Get()->ControllerPairs;
				for (const FOculusControllerPair& HandPair : ControllerPairs)
				{
					if (HandPair.DeviceId == InDeviceId)
					{
						if (DeviceHand != EOculusXRHandType::None)
						{
							ovrpHand Hand = DeviceHand == EOculusXRHandType::HandLeft ? ovrpHand_Left : ovrpHand_Right;
							return HandPair.HandControllerStates[Hand].bIsDominantHand;
						}
					}
				}
			}
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			if (DeviceHand != EOculusXRHandType::None)
			{
				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				const FOculusHandControllerState& HandState = InputModule->GetHandTrackingOpenXRExtension()->HandControllerStates[DeviceHand];
				return HandState.bIsDominantHand;
			}
		}
#endif

		return false;
	}

	bool FOculusHandTracking::IsHandPositionValid(int32 ControllerIndex, EOculusXRHandType DeviceHand)
	{
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			TSharedPtr<FOculusXRInput> OculusXRInputModule = StaticCastSharedPtr<FOculusXRInput>(IOculusXRInputModule::Get().GetInputDevice());
			if (OculusXRInputModule.IsValid())
			{
				const FInputDeviceId InDeviceId = GetDeviceID(ControllerIndex);
				TArray<FOculusControllerPair> ControllerPairs = OculusXRInputModule.Get()->ControllerPairs;
				for (const FOculusControllerPair& HandPair : ControllerPairs)
				{
					if (HandPair.DeviceId == InDeviceId)
					{
						if (DeviceHand != EOculusXRHandType::None)
						{
							ovrpHand Hand = DeviceHand == EOculusXRHandType::HandLeft ? ovrpHand_Left : ovrpHand_Right;
							return HandPair.HandControllerStates[Hand].bIsPositionValid;
						}
					}
				}
			}
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			if (DeviceHand != EOculusXRHandType::None)
			{
				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				const FOculusHandControllerState& HandState = InputModule->GetHandTrackingOpenXRExtension()->HandControllerStates[DeviceHand];
				return HandState.bIsPositionValid;
			}
		}

		return false;
	}

	bool FOculusHandTracking::GetHandSkeletalMesh(USkeletalMesh* HandSkeletalMesh, const EOculusXRHandType SkeletonType, const EOculusXRHandType MeshType, const float WorldToMeters)
	{
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
		if (HandSkeletalMesh)
		{
			if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
			{
				if (!IsPluginWrapperAvailible())
				{
					return false;
				}

				ovrpMesh* OvrMesh = new ovrpMesh();
				ovrpSkeleton2* OvrSkeleton = new ovrpSkeleton2();

				ovrpSkeletonType OvrSkeletonType = (ovrpSkeletonType)((int32)SkeletonType - 1);
				ovrpMeshType OvrMeshType = (ovrpMeshType)((int32)MeshType - 1);

				ovrpResult SkelResult = FOculusXRHMDModule::GetPluginWrapper().GetSkeleton2(OvrSkeletonType, OvrSkeleton);
				ovrpResult MeshResult = FOculusXRHMDModule::GetPluginWrapper().GetMesh(OvrMeshType, OvrMesh);

				if (SkelResult != ovrpSuccess || MeshResult != ovrpSuccess)
				{
#if !WITH_EDITOR
					UE_LOG(LogOcHandTracking, Error, TEXT("Failed to get mesh or skeleton data from Oculus runtime."));
#endif
					delete OvrMesh;
					delete OvrSkeleton;

					return false;
				}

				// Create Skeletal Mesh LOD Render Data
#if WITH_EDITOR
				FSkeletalMeshLODModel* LodRenderData = new FSkeletalMeshLODModel();
				HandSkeletalMesh->GetImportedModel()->LODModels.Add(LodRenderData);
#else
				FSkeletalMeshLODRenderData* LodRenderData = new FSkeletalMeshLODRenderData();
				HandSkeletalMesh->AllocateResourceForRendering();
				HandSkeletalMesh->GetResourceForRendering()->LODRenderData.Add(LodRenderData);
#endif

				// Set default LOD Info
				FSkeletalMeshLODInfo& LodInfo = HandSkeletalMesh->AddLODInfo();
				LodInfo.ScreenSize = 0.3f;
				LodInfo.LODHysteresis = 0.2f;
				LodInfo.BuildSettings.bUseFullPrecisionUVs = true;
				LodInfo.bHasBeenSimplified = false;

				InitializeHandSkeleton(HandSkeletalMesh, OvrSkeleton, WorldToMeters);

				// Add default material as backup
				LodInfo.LODMaterialMap.Add(0);
				UMaterialInterface* DefaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
				HandSkeletalMesh->GetMaterials().Add(DefaultMaterial);
				HandSkeletalMesh->GetMaterials()[0].UVChannelData.bInitialized = true;

				// Set skeletal mesh properties
				HandSkeletalMesh->SetHasVertexColors(true);
				HandSkeletalMesh->SetEnablePerPolyCollision(false);

				InitializeHandMesh(HandSkeletalMesh, OvrMesh, WorldToMeters);

#if WITH_EDITOR
				HandSkeletalMesh->InvalidateDeriveDataCacheGUID();
				HandSkeletalMesh->PostEditChange();
#endif

				// Create Skeleton object and merge all bones
				HandSkeletalMesh->SetSkeleton(NewObject<USkeleton>());
				HandSkeletalMesh->GetSkeleton()->MergeAllBonesToBoneTree(HandSkeletalMesh);
				HandSkeletalMesh->PostLoad();

				delete OvrMesh;
				delete OvrSkeleton;

				return true;
			}
			else if (OculusXR::IsOpenXRSystem())
			{
				TSharedPtr<FHandMesh> HandMesh = MakeShareable(new FHandMesh);
				TSharedPtr<FHandSkeleton> HandSkeleton = MakeShareable(new FHandSkeleton);

				const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
				bool SkelResult = InputModule->GetHandTrackingOpenXRExtension()->GetSkeleton(SkeletonType, HandSkeleton);
				bool MeshResult = InputModule->GetHandTrackingOpenXRExtension()->GetMesh(MeshType, HandMesh);

				if (!SkelResult || !MeshResult)
				{
#if !WITH_EDITOR
					UE_LOG(LogOcHandTracking, Error, TEXT("Failed to get mesh or skeleton data from Oculus runtime."));
#endif
					return false;
				}

				// Create Skeletal Mesh LOD Render Data
#if WITH_EDITOR
				FSkeletalMeshLODModel* LodRenderData = new FSkeletalMeshLODModel();
				HandSkeletalMesh->GetImportedModel()->LODModels.Add(LodRenderData);
#else
				FSkeletalMeshLODRenderData* LodRenderData = new FSkeletalMeshLODRenderData();
				HandSkeletalMesh->AllocateResourceForRendering();
				HandSkeletalMesh->GetResourceForRendering()->LODRenderData.Add(LodRenderData);
#endif

				// Set default LOD Info
				FSkeletalMeshLODInfo& LodInfo = HandSkeletalMesh->AddLODInfo();
				LodInfo.ScreenSize = 0.3f;
				LodInfo.LODHysteresis = 0.2f;
				LodInfo.BuildSettings.bUseFullPrecisionUVs = true;
				LodInfo.bHasBeenSimplified = false;

				InitializeHandSkeletonOpenXR(HandSkeletalMesh, HandSkeleton, WorldToMeters);

				// Add default material as backup
				LodInfo.LODMaterialMap.Add(0);
				UMaterialInterface* DefaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
				HandSkeletalMesh->GetMaterials().Add(DefaultMaterial);
				HandSkeletalMesh->GetMaterials()[0].UVChannelData.bInitialized = true;

				// Set skeletal mesh properties
				HandSkeletalMesh->SetHasVertexColors(true);
				HandSkeletalMesh->SetEnablePerPolyCollision(false);

				InitializeHandMeshOpenXR(HandSkeletalMesh, HandMesh, WorldToMeters);

#if WITH_EDITOR
				HandSkeletalMesh->InvalidateDeriveDataCacheGUID();
				HandSkeletalMesh->PostEditChange();
#endif

				// Create Skeleton object and merge all bones
				HandSkeletalMesh->SetSkeleton(NewObject<USkeleton>());
				HandSkeletalMesh->GetSkeleton()->MergeAllBonesToBoneTree(HandSkeletalMesh);
				HandSkeletalMesh->PostLoad();

				return true;
			}
		}
#endif
		return false;
	}

	void FOculusHandTracking::InitializeHandMesh(USkeletalMesh* SkeletalMesh, const ovrpMesh* OvrMesh, const float WorldToMeters)
	{
#if WITH_EDITOR
		FSkeletalMeshLODModel* LodRenderData = &SkeletalMesh->GetImportedModel()->LODModels[0];

		// Initialize mesh section
		LodRenderData->Sections.SetNumUninitialized(1);
		new (&LodRenderData->Sections[0]) FSkelMeshSection();
		auto& MeshSection = LodRenderData->Sections[0];

		// Set default mesh section properties
		MeshSection.MaterialIndex = 0;
		MeshSection.BaseIndex = 0;
		MeshSection.NumTriangles = OvrMesh->NumIndices / 3;
		MeshSection.BaseVertexIndex = 0;
		MeshSection.MaxBoneInfluences = 4;
		MeshSection.NumVertices = OvrMesh->NumVertices;

		float MaxDistSq = MIN_flt;
		for (uint32_t VertexIndex = 0; VertexIndex < OvrMesh->NumVertices; VertexIndex++)
		{
			FSoftSkinVertex SoftVertex;
			FMemory::Memzero(SoftVertex.InfluenceWeights);
			FMemory::Memzero(SoftVertex.InfluenceBones);

			// Update vertex data
			SoftVertex.Color = FColor::White;
			ovrpVector3f VertexPosition = OvrMesh->VertexPositions[VertexIndex];
			ovrpVector3f Normal = OvrMesh->VertexNormals[VertexIndex];
			SoftVertex.Position = FVector3f(VertexPosition.x, VertexPosition.z, VertexPosition.y) * WorldToMeters;
			SoftVertex.TangentZ = FVector3f(Normal.x, Normal.z, Normal.y);
			SoftVertex.TangentX = FVector3f(1.0f, 0.0f, 0.0f);
			SoftVertex.TangentY = FVector3f(0.0f, 1.0f, 0.0f); // SoftVertex.TangentZ^ SoftVertex.TangentX* SoftVertex.TangentZ.W;
			SoftVertex.UVs[0] = FVector2f(OvrMesh->VertexUV0[VertexIndex].x, OvrMesh->VertexUV0[VertexIndex].y);

			// Update the Bounds
			float VertexDistSq = SoftVertex.Position.SizeSquared();
			if (VertexDistSq > MaxDistSq)
				MaxDistSq = VertexDistSq;

			// Update blend weights and indices
			ovrpVector4f BlendWeights = OvrMesh->BlendWeights[VertexIndex];
			ovrpVector4s BlendIndices = OvrMesh->BlendIndices[VertexIndex];

			SoftVertex.InfluenceWeights[0] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.x;
			SoftVertex.InfluenceBones[0] = BlendIndices.x;
			SoftVertex.InfluenceWeights[1] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.y;
			SoftVertex.InfluenceBones[1] = BlendIndices.y;
			SoftVertex.InfluenceWeights[2] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.z;
			SoftVertex.InfluenceBones[2] = BlendIndices.z;
			SoftVertex.InfluenceWeights[3] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.w;
			SoftVertex.InfluenceBones[3] = BlendIndices.w;

			MeshSection.SoftVertices.Add(SoftVertex);
		}

		// Update bone map
		for (uint32 BoneIndex = 0; BoneIndex < (uint32)SkeletalMesh->GetRefSkeleton().GetNum(); BoneIndex++)
		{
			MeshSection.BoneMap.Add(BoneIndex);
		}

		// Update LOD render data
		LodRenderData->NumVertices = OvrMesh->NumVertices;
		LodRenderData->NumTexCoords = 1;

		// Create index buffer
		for (uint32_t Index = 0; Index < OvrMesh->NumIndices; Index++)
		{
			LodRenderData->IndexBuffer.Add(OvrMesh->Indices[Index]);
		}

		// Finalize Bounds
		float MaxDist = FMath::Sqrt(MaxDistSq);
		FBoxSphereBounds Bounds;
		Bounds.Origin = FVector::ZeroVector;
		Bounds.BoxExtent = FVector(MaxDist);
		Bounds.SphereRadius = MaxDist;
		SkeletalMesh->SetImportedBounds(Bounds);

#else
		FSkeletalMeshLODRenderData* LodRenderData = &SkeletalMesh->GetResourceForRendering()->LODRenderData[0];

		// Initialize Mesh Section
		LodRenderData->RenderSections.SetNumUninitialized(1);
		new (&LodRenderData->RenderSections[0]) FSkelMeshRenderSection();
		auto& MeshSection = LodRenderData->RenderSections[0];

		// Initialize render section properties
		MeshSection.MaterialIndex = 0;
		MeshSection.BaseIndex = 0;
		MeshSection.NumTriangles = OvrMesh->NumIndices / 3;
		MeshSection.BaseVertexIndex = 0;
		MeshSection.MaxBoneInfluences = 4;
		MeshSection.NumVertices = OvrMesh->NumVertices;
		MeshSection.bCastShadow = true;
		MeshSection.bDisabled = false;
		MeshSection.bRecomputeTangent = false;

		// Initialize Vertex Buffers
		LodRenderData->StaticVertexBuffers.PositionVertexBuffer.Init(OvrMesh->NumVertices);
		LodRenderData->StaticVertexBuffers.StaticMeshVertexBuffer.Init(OvrMesh->NumVertices, 1);
		LodRenderData->StaticVertexBuffers.ColorVertexBuffer.Init(OvrMesh->NumVertices);

		// Initialize Skin Weights
		TArray<FSkinWeightInfo> InWeights;
		InWeights.AddUninitialized(OvrMesh->NumVertices);

		float MaxDistSq = MIN_flt;
		TMap<int32, TArray<int32>> OverlappingVertices;
		for (uint32_t VertexIndex = 0; VertexIndex < OvrMesh->NumVertices; VertexIndex++)
		{
			FMemory::Memzero(InWeights[VertexIndex].InfluenceWeights);
			FMemory::Memzero(InWeights[VertexIndex].InfluenceBones);
			// Initialize vertex data
			FModelVertex ModelVertex;

			// Update Model Vertex
			ovrpVector3f VertexPosition = OvrMesh->VertexPositions[VertexIndex];
			ovrpVector3f Normal = OvrMesh->VertexNormals[VertexIndex];
			ModelVertex.Position = FVector3f(VertexPosition.x, VertexPosition.z, VertexPosition.y) * WorldToMeters;
			ModelVertex.TangentZ = FVector3f(Normal.x, Normal.z, Normal.y);
			ModelVertex.TangentX = FVector3f(1.0f, 0.0f, 0.0f);
			ModelVertex.TexCoord = FVector2f(OvrMesh->VertexUV0[VertexIndex].x, OvrMesh->VertexUV0[VertexIndex].y);

			// Add Model Vertex data to vertex buffer
			LodRenderData->StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex) = ModelVertex.Position;
			LodRenderData->StaticVertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(VertexIndex, ModelVertex.TangentX, ModelVertex.GetTangentY(), ModelVertex.TangentZ);
			LodRenderData->StaticVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(VertexIndex, 0, ModelVertex.TexCoord);

			// Update the Bounds
			float VertexDistSq = ModelVertex.Position.SizeSquared();
			if (VertexDistSq > MaxDistSq)
				MaxDistSq = VertexDistSq;

			// Set vertex blend weights and indices
			TArray<int32> Vertices;
			ovrpVector4f BlendWeights = OvrMesh->BlendWeights[VertexIndex];
			ovrpVector4s BlendIndices = OvrMesh->BlendIndices[VertexIndex];

			InWeights[VertexIndex].InfluenceWeights[0] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.x;
			InWeights[VertexIndex].InfluenceBones[0] = BlendIndices.x;
			Vertices.Add(BlendIndices.x);
			InWeights[VertexIndex].InfluenceWeights[1] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.y;
			InWeights[VertexIndex].InfluenceBones[1] = BlendIndices.y;
			Vertices.Add(BlendIndices.y);
			InWeights[VertexIndex].InfluenceWeights[2] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.z;
			InWeights[VertexIndex].InfluenceBones[2] = BlendIndices.z;
			Vertices.Add(BlendIndices.z);
			InWeights[VertexIndex].InfluenceWeights[3] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.w;
			InWeights[VertexIndex].InfluenceBones[3] = BlendIndices.w;
			Vertices.Add(BlendIndices.w);

			OverlappingVertices.Add(VertexIndex, Vertices);
		}

		// Update bone map for mesh section
		for (uint32 BoneIndex = 0; BoneIndex < (uint32)SkeletalMesh->GetRefSkeleton().GetNum(); BoneIndex++)
		{
			MeshSection.BoneMap.Add(BoneIndex);
		}

		// Finalize Bounds
		float MaxDist = FMath::Sqrt(MaxDistSq);
		FBoxSphereBounds Bounds;
		Bounds.Origin = FVector::ZeroVector;
		Bounds.BoxExtent = FVector(MaxDist);
		Bounds.SphereRadius = MaxDist;
		SkeletalMesh->SetImportedBounds(Bounds);

		// Assign skin weights to vertex buffer
		LodRenderData->SkinWeightVertexBuffer = InWeights;
		MeshSection.DuplicatedVerticesBuffer.Init(OvrMesh->NumVertices, OverlappingVertices);

		// Set index buffer
		LodRenderData->MultiSizeIndexContainer.CreateIndexBuffer(sizeof(uint16_t));
		for (uint32_t Index = 0; Index < OvrMesh->NumIndices; Index++)
		{
			LodRenderData->MultiSizeIndexContainer.GetIndexBuffer()->AddItem(OvrMesh->Indices[Index]);
		}
#endif
	}

	void FOculusHandTracking::InitializeHandMeshOpenXR(USkeletalMesh* SkeletalMesh, const TSharedPtr<FHandMesh> XrMesh, const float WorldToMeters)
	{
#if WITH_EDITOR
		FSkeletalMeshLODModel* LodRenderData = &SkeletalMesh->GetImportedModel()->LODModels[0];

		// Initialize mesh section
		LodRenderData->Sections.SetNumUninitialized(1);
		new (&LodRenderData->Sections[0]) FSkelMeshSection();
		auto& MeshSection = LodRenderData->Sections[0];

		// Set default mesh section properties
		MeshSection.MaterialIndex = 0;
		MeshSection.BaseIndex = 0;
		MeshSection.NumTriangles = XrMesh->NumIndices / 3;
		MeshSection.BaseVertexIndex = 0;
		MeshSection.MaxBoneInfluences = 4;
		MeshSection.NumVertices = XrMesh->NumVertices;

		float MaxDistSq = MIN_flt;
		for (uint32_t VertexIndex = 0; VertexIndex < XrMesh->NumVertices; VertexIndex++)
		{
			FSoftSkinVertex SoftVertex;
			FMemory::Memzero(SoftVertex.InfluenceWeights);
			FMemory::Memzero(SoftVertex.InfluenceBones);

			// Update vertex data
			SoftVertex.Color = FColor::White;
			XrVector3f VertexPosition = XrMesh->VertexPositions[VertexIndex];
			XrVector3f Normal = XrMesh->VertexNormals[VertexIndex];
			SoftVertex.Position = FVector3f(VertexPosition.x, VertexPosition.z, VertexPosition.y) * WorldToMeters;
			SoftVertex.TangentZ = FVector3f(Normal.x, Normal.z, Normal.y);
			SoftVertex.TangentX = FVector3f(1.0f, 0.0f, 0.0f);
			SoftVertex.TangentY = FVector3f(0.0f, 1.0f, 0.0f); // SoftVertex.TangentZ^ SoftVertex.TangentX* SoftVertex.TangentZ.W;
			SoftVertex.UVs[0] = FVector2f(XrMesh->VertexUV0[VertexIndex].x, XrMesh->VertexUV0[VertexIndex].y);

			// Update the Bounds
			float VertexDistSq = SoftVertex.Position.SizeSquared();
			if (VertexDistSq > MaxDistSq)
				MaxDistSq = VertexDistSq;

			// Update blend weights and indices
			XrVector4f BlendWeights = XrMesh->BlendWeights[VertexIndex];
			XrVector4sFB BlendIndices = XrMesh->BlendIndices[VertexIndex];

			SoftVertex.InfluenceWeights[0] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.x;
			SoftVertex.InfluenceBones[0] = BlendIndices.x;
			SoftVertex.InfluenceWeights[1] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.y;
			SoftVertex.InfluenceBones[1] = BlendIndices.y;
			SoftVertex.InfluenceWeights[2] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.z;
			SoftVertex.InfluenceBones[2] = BlendIndices.z;
			SoftVertex.InfluenceWeights[3] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.w;
			SoftVertex.InfluenceBones[3] = BlendIndices.w;

			MeshSection.SoftVertices.Add(SoftVertex);
		}

		// Update bone map
		for (uint32 BoneIndex = 0; BoneIndex < (uint32)SkeletalMesh->GetRefSkeleton().GetNum(); BoneIndex++)
		{
			MeshSection.BoneMap.Add(BoneIndex);
		}

		// Update LOD render data
		LodRenderData->NumVertices = XrMesh->NumVertices;
		LodRenderData->NumTexCoords = 1;

		// Create index buffer
		for (uint32_t Index = 0; Index < XrMesh->NumIndices; Index++)
		{
			LodRenderData->IndexBuffer.Add(XrMesh->Indices[Index]);
		}

		// Finalize Bounds
		float MaxDist = FMath::Sqrt(MaxDistSq);
		FBoxSphereBounds Bounds;
		Bounds.Origin = FVector::ZeroVector;
		Bounds.BoxExtent = FVector(MaxDist);
		Bounds.SphereRadius = MaxDist;
		SkeletalMesh->SetImportedBounds(Bounds);

#else
		FSkeletalMeshLODRenderData* LodRenderData = &SkeletalMesh->GetResourceForRendering()->LODRenderData[0];

		// Initialize Mesh Section
		LodRenderData->RenderSections.SetNumUninitialized(1);
		new (&LodRenderData->RenderSections[0]) FSkelMeshRenderSection();
		auto& MeshSection = LodRenderData->RenderSections[0];

		// Initialize render section properties
		MeshSection.MaterialIndex = 0;
		MeshSection.BaseIndex = 0;
		MeshSection.NumTriangles = XrMesh->NumIndices / 3;
		MeshSection.BaseVertexIndex = 0;
		MeshSection.MaxBoneInfluences = 4;
		MeshSection.NumVertices = XrMesh->NumVertices;
		MeshSection.bCastShadow = true;
		MeshSection.bDisabled = false;
		MeshSection.bRecomputeTangent = false;

		// Initialize Vertex Buffers
		LodRenderData->StaticVertexBuffers.PositionVertexBuffer.Init(XrMesh->NumVertices);
		LodRenderData->StaticVertexBuffers.StaticMeshVertexBuffer.Init(XrMesh->NumVertices, 1);
		LodRenderData->StaticVertexBuffers.ColorVertexBuffer.Init(XrMesh->NumVertices);

		// Initialize Skin Weights
		TArray<FSkinWeightInfo> InWeights;
		InWeights.AddUninitialized(XrMesh->NumVertices);

		float MaxDistSq = MIN_flt;
		TMap<int32, TArray<int32>> OverlappingVertices;
		for (uint32_t VertexIndex = 0; VertexIndex < XrMesh->NumVertices; VertexIndex++)
		{
			FMemory::Memzero(InWeights[VertexIndex].InfluenceWeights);
			FMemory::Memzero(InWeights[VertexIndex].InfluenceBones);
			// Initialize vertex data
			FModelVertex ModelVertex;

			// Update Model Vertex
			XrVector3f VertexPosition = XrMesh->VertexPositions[VertexIndex];
			XrVector3f Normal = XrMesh->VertexNormals[VertexIndex];
			ModelVertex.Position = FVector3f(VertexPosition.x, VertexPosition.z, VertexPosition.y) * WorldToMeters;
			ModelVertex.TangentZ = FVector3f(Normal.x, Normal.z, Normal.y);
			ModelVertex.TangentX = FVector3f(1.0f, 0.0f, 0.0f);
			ModelVertex.TexCoord = FVector2f(XrMesh->VertexUV0[VertexIndex].x, XrMesh->VertexUV0[VertexIndex].y);

			// Add Model Vertex data to vertex buffer
			LodRenderData->StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex) = ModelVertex.Position;
			LodRenderData->StaticVertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(VertexIndex, ModelVertex.TangentX, ModelVertex.GetTangentY(), ModelVertex.TangentZ);
			LodRenderData->StaticVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(VertexIndex, 0, ModelVertex.TexCoord);

			// Update the Bounds
			float VertexDistSq = ModelVertex.Position.SizeSquared();
			if (VertexDistSq > MaxDistSq)
				MaxDistSq = VertexDistSq;

			// Set vertex blend weights and indices
			TArray<int32> Vertices;
			XrVector4f BlendWeights = XrMesh->BlendWeights[VertexIndex];
			XrVector4sFB BlendIndices = XrMesh->BlendIndices[VertexIndex];

			InWeights[VertexIndex].InfluenceWeights[0] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.x;
			InWeights[VertexIndex].InfluenceBones[0] = BlendIndices.x;
			Vertices.Add(BlendIndices.x);
			InWeights[VertexIndex].InfluenceWeights[1] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.y;
			InWeights[VertexIndex].InfluenceBones[1] = BlendIndices.y;
			Vertices.Add(BlendIndices.y);
			InWeights[VertexIndex].InfluenceWeights[2] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.z;
			InWeights[VertexIndex].InfluenceBones[2] = BlendIndices.z;
			Vertices.Add(BlendIndices.z);
			InWeights[VertexIndex].InfluenceWeights[3] = UE::AnimationCore::MaxRawBoneWeightFloat * BlendWeights.w;
			InWeights[VertexIndex].InfluenceBones[3] = BlendIndices.w;
			Vertices.Add(BlendIndices.w);

			OverlappingVertices.Add(VertexIndex, Vertices);
		}

		// Update bone map for mesh section
		for (uint32 BoneIndex = 0; BoneIndex < (uint32)SkeletalMesh->GetRefSkeleton().GetNum(); BoneIndex++)
		{
			MeshSection.BoneMap.Add(BoneIndex);
		}

		// Finalize Bounds
		float MaxDist = FMath::Sqrt(MaxDistSq);
		FBoxSphereBounds Bounds;
		Bounds.Origin = FVector::ZeroVector;
		Bounds.BoxExtent = FVector(MaxDist);
		Bounds.SphereRadius = MaxDist;
		SkeletalMesh->SetImportedBounds(Bounds);

		// Assign skin weights to vertex buffer
		LodRenderData->SkinWeightVertexBuffer = InWeights;
		MeshSection.DuplicatedVerticesBuffer.Init(XrMesh->NumVertices, OverlappingVertices);

		// Set index buffer
		LodRenderData->MultiSizeIndexContainer.CreateIndexBuffer(sizeof(uint16_t));
		for (uint32_t Index = 0; Index < XrMesh->NumIndices; Index++)
		{
			LodRenderData->MultiSizeIndexContainer.GetIndexBuffer()->AddItem(XrMesh->Indices[Index]);
		}
#endif
	}

	void FOculusHandTracking::InitializeHandSkeleton(USkeletalMesh* SkeletalMesh, const ovrpSkeleton2* OvrSkeleton, const float WorldToMeters)
	{
		SkeletalMesh->GetRefSkeleton().Empty(OvrSkeleton->NumBones);

#if WITH_EDITOR
		FSkeletalMeshLODModel* LodRenderData = &SkeletalMesh->GetImportedModel()->LODModels[0];
#else
		FSkeletalMeshLODRenderData* LodRenderData = &SkeletalMesh->GetResourceForRendering()->LODRenderData[0];
#endif
		SkeletalMesh->SetHasVertexColors(true);

		checkf(OvrSkeleton->NumBones <= static_cast<unsigned int>(TNumericLimits<uint8>::Max()), TEXT("Bone indices are stored as uint8 type."));
		for (uint8 BoneIndex = 0; BoneIndex < static_cast<uint8>(OvrSkeleton->NumBones); BoneIndex++)
		{
			LodRenderData->ActiveBoneIndices.Add(BoneIndex);
			LodRenderData->RequiredBones.Add(BoneIndex);

			FText BoneDisplayName;
			if (!FindBoneDisplayName(BoneDisplayName, BoneIndex))
			{
				UE_LOG(LogOcHandTracking, Error, TEXT("Cannot find bone display name for bone index: %d."), BoneIndex)
				continue;
			}
			FString BoneString = BoneDisplayName.ToString();
			FName BoneName = FName(*BoneString);

			FTransform Transform = FTransform::Identity;
			FVector BonePosition = OvrBoneVectorToFVector(OvrSkeleton->Bones[BoneIndex].Pose.Position, WorldToMeters);
			FQuat BoneRotation = BoneIndex == 0 ? FQuat(-1.0f, 0.0f, 0.0f, 1.0f) : OvrBoneQuatToFQuat(OvrSkeleton->Bones[BoneIndex].Pose.Orientation);
			Transform.SetLocation(BonePosition);
			Transform.SetRotation(BoneRotation);

			FReferenceSkeletonModifier Modifier = FReferenceSkeletonModifier(SkeletalMesh->GetRefSkeleton(), nullptr);
			int32 ParentIndex = -1;
			if (BoneIndex > 0)
			{
				if (OvrSkeleton->Bones[BoneIndex].ParentBoneIndex == ovrpBoneId::ovrpBoneId_Invalid)
				{
					ParentIndex = 0;
				}
				else
				{
					ParentIndex = OvrSkeleton->Bones[BoneIndex].ParentBoneIndex;
				}
			}
			Modifier.Add(FMeshBoneInfo(BoneName, BoneString, ParentIndex), Transform);
		}
		SkeletalMesh->CalculateInvRefMatrices();
	}

	void FOculusHandTracking::InitializeHandSkeletonOpenXR(USkeletalMesh* SkeletalMesh, const TSharedPtr<FHandSkeleton> Skeleton, const float WorldToMeters)
	{
		SkeletalMesh->GetRefSkeleton().Empty(Skeleton->NumBones);

#if WITH_EDITOR
		FSkeletalMeshLODModel* LodRenderData = &SkeletalMesh->GetImportedModel()->LODModels[0];
#else
		FSkeletalMeshLODRenderData* LodRenderData = &SkeletalMesh->GetResourceForRendering()->LODRenderData[0];
#endif
		SkeletalMesh->SetHasVertexColors(true);

		checkf(Skeleton->NumBones <= static_cast<unsigned int>(TNumericLimits<uint8>::Max()), TEXT("Bone indices are stored as uint8 type."));
		for (uint8 BoneIndex = 0; BoneIndex < static_cast<uint8>(Skeleton->NumBones); BoneIndex++)
		{
			LodRenderData->ActiveBoneIndices.Add(BoneIndex);
			LodRenderData->RequiredBones.Add(BoneIndex);

			FText BoneDisplayName;
			if (!FindBoneDisplayName(BoneDisplayName, BoneIndex))
			{
				UE_LOG(LogOcHandTracking, Error, TEXT("Cannot find bone display name for bone index: %d."), BoneIndex)
				continue;
			}
			FString BoneString = BoneDisplayName.ToString();
			FName BoneName = FName(*BoneString);

			FTransform Transform = FTransform::Identity;
			FVector BonePosition = HandBoneVectorToFVector(Skeleton->Bones[BoneIndex].Pose.position, WorldToMeters);
			FQuat BoneRotation = BoneIndex == 0 ? FQuat(-1.0f, 0.0f, 0.0f, 1.0f) : HandBoneQuatToFQuat(Skeleton->Bones[BoneIndex].Pose.orientation);

			Transform.SetLocation(BonePosition);
			Transform.SetRotation(BoneRotation);

			FReferenceSkeletonModifier Modifier = FReferenceSkeletonModifier(SkeletalMesh->GetRefSkeleton(), nullptr);
			int32 ParentIndex = -1;
			if (BoneIndex > 0)
			{
				if (Skeleton->Bones[BoneIndex].ParentBoneIndex == static_cast<int32>(EHandBoneId::Invalid))
				{
					ParentIndex = 0;
				}
				else
				{
					ParentIndex = Skeleton->Bones[BoneIndex].ParentBoneIndex;
				}
			}
			Modifier.Add(FMeshBoneInfo(BoneName, BoneString, ParentIndex), Transform);
		}
		SkeletalMesh->CalculateInvRefMatrices();
	}

	TArray<FOculusXRCapsuleCollider> FOculusHandTracking::InitializeHandPhysics(const EOculusXRHandType SkeletonType, USkinnedMeshComponent* HandComponent, const float WorldToMeters)
	{
		TArray<FOculusXRCapsuleCollider> CollisionCapsules;
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			if (!IsPluginWrapperAvailible())
			{
				return TArray<FOculusXRCapsuleCollider>();
			}
			ovrpSkeleton2* OvrSkeleton = new ovrpSkeleton2();
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
			ovrpSkeletonType OvrSkeletonType = (ovrpSkeletonType)((int32)SkeletonType - 1);
			if (FOculusXRHMDModule::GetPluginWrapper().GetSkeleton2(OvrSkeletonType, OvrSkeleton) != ovrpSuccess)
			{
#if !WITH_EDITOR
				UE_LOG(LogOcHandTracking, Error, TEXT("Failed to get skeleton data from Oculus runtime."));
#endif
				delete OvrSkeleton;
				return CollisionCapsules;
			}
#endif
			TArray<UPrimitiveComponent*> IgnoreCapsules;
			CollisionCapsules.AddDefaulted(OvrSkeleton->NumBoneCapsules);
			for (uint32 CapsuleIndex = 0; CapsuleIndex < OvrSkeleton->NumBoneCapsules; CapsuleIndex++)
			{
				ovrpBoneCapsule OvrBoneCapsule = OvrSkeleton->BoneCapsules[CapsuleIndex];

				UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(HandComponent);

				FVector CapsulePointZero = OvrBoneVectorToFVector(OvrBoneCapsule.Points[0], WorldToMeters);
				FVector CapsulePointOne = OvrBoneVectorToFVector(OvrBoneCapsule.Points[1], WorldToMeters);
				FVector Delta = (CapsulePointOne - CapsulePointZero);

				FName BoneName = HandComponent->GetSkinnedAsset()->GetRefSkeleton().GetBoneName(OvrBoneCapsule.BoneIndex);

				float CapsuleHeight = Delta.Size();
				float CapsuleRadius = OvrBoneCapsule.Radius * WorldToMeters;

				Capsule->SetCapsuleRadius(CapsuleRadius);
				Capsule->SetCapsuleHalfHeight(Delta.Size() / 2 + CapsuleRadius);
				Capsule->SetupAttachment(HandComponent, BoneName);
				Capsule->SetCollisionProfileName(HandComponent->GetCollisionProfileName());
				Capsule->RegisterComponentWithWorld(HandComponent->GetWorld());
				Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				FRotator CapsuleRotation = FQuat::FindBetweenVectors(FVector::RightVector, Delta).Rotator() + FRotator(0, 0, 90);
				;

				Capsule->SetRelativeRotation(CapsuleRotation);
				Capsule->SetRelativeLocation(CapsulePointZero + (Delta / 2));

				CollisionCapsules[CapsuleIndex].Capsule = Capsule;
				CollisionCapsules[CapsuleIndex].BoneId = (EOculusXRBone)OvrBoneCapsule.BoneIndex;

				IgnoreCapsules.Add(Capsule);
			}

			for (int32 CapsuleIndex = 0; CapsuleIndex < CollisionCapsules.Num(); CapsuleIndex++)
			{
				CollisionCapsules[CapsuleIndex].Capsule->MoveIgnoreComponents = IgnoreCapsules;
			}

			return CollisionCapsules;
		}
		else if (OculusXR::IsOpenXRSystem())
		{
			TSharedPtr<FHandSkeleton> HandSkeleton = MakeShareable(new FHandSkeleton);
#if OCULUS_INPUT_SUPPORTED_PLATFORMS
			const FOculusXRInputModule* InputModule = static_cast<FOculusXRInputModule*>(&FOculusXRInputModule::Get());
			if (!InputModule->GetHandTrackingOpenXRExtension()->GetSkeleton(SkeletonType, HandSkeleton))
			{
#if !WITH_EDITOR
				UE_LOG(LogOcHandTracking, Error, TEXT("Failed to get skeleton data from Oculus runtime."));
#endif
				return CollisionCapsules;
			}
#endif
			TArray<UPrimitiveComponent*> IgnoreCapsules;
			CollisionCapsules.AddDefaulted(HandSkeleton->NumBoneCapsules);
			for (uint32 CapsuleIndex = 0; CapsuleIndex < HandSkeleton->NumBoneCapsules; CapsuleIndex++)
			{
				FHandBoneCapsule XrBoneCapsule = HandSkeleton->BoneCapsules[CapsuleIndex];

				UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(HandComponent);

				FVector CapsulePointZero = HandBoneVectorToFVector(XrBoneCapsule.Points[0], WorldToMeters);
				FVector CapsulePointOne = HandBoneVectorToFVector(XrBoneCapsule.Points[1], WorldToMeters);
				FVector Delta = (CapsulePointOne - CapsulePointZero);

				FName BoneName = HandComponent->GetSkinnedAsset()->GetRefSkeleton().GetBoneName(XrBoneCapsule.BoneIndex);

				float CapsuleHeight = Delta.Size();
				float CapsuleRadius = XrBoneCapsule.Radius * WorldToMeters;

				Capsule->SetCapsuleRadius(CapsuleRadius);
				Capsule->SetCapsuleHalfHeight(Delta.Size() / 2 + CapsuleRadius);
				Capsule->SetupAttachment(HandComponent, BoneName);
				Capsule->SetCollisionProfileName(HandComponent->GetCollisionProfileName());
				Capsule->RegisterComponentWithWorld(HandComponent->GetWorld());
				Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				FRotator CapsuleRotation = FQuat::FindBetweenVectors(FVector::RightVector, Delta).Rotator() + FRotator(0, 0, 90);
				;

				Capsule->SetRelativeRotation(CapsuleRotation);
				Capsule->SetRelativeLocation(CapsulePointZero + (Delta / 2));

				CollisionCapsules[CapsuleIndex].Capsule = Capsule;
				CollisionCapsules[CapsuleIndex].BoneId = (EOculusXRBone)XrBoneCapsule.BoneIndex;

				IgnoreCapsules.Add(Capsule);
			}

			for (int32 CapsuleIndex = 0; CapsuleIndex < CollisionCapsules.Num(); CapsuleIndex++)
			{
				CollisionCapsules[CapsuleIndex].Capsule->MoveIgnoreComponents = IgnoreCapsules;
			}

			return CollisionCapsules;
		}
		return CollisionCapsules;
	}

	ovrpBoneId FOculusHandTracking::ToOvrBone(EOculusXRBone Bone)
	{
		if (Bone > EOculusXRBone::Bone_Max)
			return ovrpBoneId_Invalid;

		return static_cast<ovrpBoneId>(Bone);
	}

	EHandBoneId FOculusHandTracking::ToHandBone(EOculusXRBone Bone)
	{
		if (Bone > EOculusXRBone::Bone_Max)
			return EHandBoneId::Invalid;

		return static_cast<EHandBoneId>(Bone);
	}

	FString FOculusHandTracking::GetBoneName(const uint8 Bone)
	{
		FText DisplayName;
		if (FindBoneDisplayName(DisplayName, Bone))
		{
			return DisplayName.ToString();
		}
		if (FindBoneDisplayName(DisplayName, static_cast<uint8>(EOculusXRBone::Invalid)))
		{
			return DisplayName.ToString();
		}
		return { "Invalid" };
	}

	bool FOculusHandTracking::FindBoneDisplayName(FText& DisplayName, uint8 Bone)
	{
		return StaticEnum<EOculusXRBone>()->FindDisplayNameTextByValue(DisplayName, Bone);
	}

	EOculusXRTrackingConfidence FOculusHandTracking::ToEOculusXRTrackingConfidence(ovrpTrackingConfidence Confidence)
	{
		EOculusXRTrackingConfidence TrackingConfidence = EOculusXRTrackingConfidence::Low;
		switch (Confidence)
		{
			case ovrpTrackingConfidence_Low:
				TrackingConfidence = EOculusXRTrackingConfidence::Low;
				break;
			case ovrpTrackingConfidence_High:
				TrackingConfidence = EOculusXRTrackingConfidence::High;
				break;
		}
		return TrackingConfidence;
	}

	FVector FOculusHandTracking::OvrBoneVectorToFVector(ovrpVector3f ovrpVector, float WorldToMeters)
	{
		return FVector(ovrpVector.x, -ovrpVector.y, ovrpVector.z) * WorldToMeters;
	}

	FQuat FOculusHandTracking::OvrBoneQuatToFQuat(ovrpQuatf ovrpQuat)
	{
		return FQuat(ovrpQuat.x, -ovrpQuat.y, ovrpQuat.z, -ovrpQuat.w);
	}

	FVector FOculusHandTracking::HandBoneVectorToFVector(XrVector3f XrVector, float WorldToMeters)
	{
		return FVector(XrVector.x, -XrVector.y, XrVector.z) * WorldToMeters;
	}

	FQuat FOculusHandTracking::HandBoneQuatToFQuat(XrQuaternionf XrQuat)
	{
		return FQuat(XrQuat.x, -XrQuat.y, XrQuat.z, -XrQuat.w);
	}

	EOculusXRControllerDrivenHandPoseTypes FOculusHandTracking::ControllerDrivenHandType = EOculusXRControllerDrivenHandPoseTypes::None;

	void FOculusHandTracking::SetControllerDrivenHandPoses(const EOculusXRControllerDrivenHandPoseTypes Type)
	{
		if (!IsPluginWrapperAvailible())
		{
			return;
		}

		FOculusHandTracking::ControllerDrivenHandType = Type;
		switch (Type)
		{
			case EOculusXRControllerDrivenHandPoseTypes::None:
				FOculusXRHMDModule::GetPluginWrapper().SetControllerDrivenHandPoses(false);
				FOculusXRHMDModule::GetPluginWrapper().SetControllerDrivenHandPosesAreNatural(false);
				break;
			case EOculusXRControllerDrivenHandPoseTypes::Natural:
				FOculusXRHMDModule::GetPluginWrapper().SetControllerDrivenHandPoses(true);
				FOculusXRHMDModule::GetPluginWrapper().SetControllerDrivenHandPosesAreNatural(true);
				break;
			case EOculusXRControllerDrivenHandPoseTypes::Controller:
				FOculusXRHMDModule::GetPluginWrapper().SetControllerDrivenHandPoses(true);
				FOculusXRHMDModule::GetPluginWrapper().SetControllerDrivenHandPosesAreNatural(false);
				break;
		}
	}
} // namespace OculusXRInput
