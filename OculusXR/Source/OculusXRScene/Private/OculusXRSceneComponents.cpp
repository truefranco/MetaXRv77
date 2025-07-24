// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSceneComponents.h"
#include "OculusXRScene.h"
#include "OculusXRSceneModule.h"
#include "OculusXRAnchorBPFunctionLibrary.h"

bool UOculusXRPlaneAnchorComponent::GetPositionAndSize(FVector& outPosition, FVector& outSize) const
{
	ensure(IsComponentEnabled());

	if (!UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(OculusXRScene::FOculusXRScene::GetScenePlane(Space, outPosition, outSize)))
	{
		UE_LOG(LogOculusXRScene, Warning, TEXT("Fetching scene plane failed."));
		return false;
	}

	return true;
}

bool UOculusXRVolumeAnchorComponent::GetPositionAndSize(FVector& outPosition, FVector& outSize) const
{
	ensure(IsComponentEnabled());

	if (!UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(OculusXRScene::FOculusXRScene::GetSceneVolume(Space, outPosition, outSize)))
	{
		UE_LOG(LogOculusXRScene, Warning, TEXT("Fetching scene plane failed."));
		return false;
	}

	return true;
}

bool UOculusXRSemanticClassificationAnchorComponent::GetSemanticClassifications(TArray<FString>& outClassifications) const
{
	ensure(IsComponentEnabled());

	if (!UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(OculusXRScene::FOculusXRScene::GetSemanticClassification(Space, outClassifications)))
	{
		UE_LOG(LogOculusXRScene, Warning, TEXT("Fetching scene volume failed."));
		return false;
	}

	return true;
}

bool UOculusXRRoomLayoutAnchorComponent::GetRoomLayout(FOculusXRUUID& outFloorUUID, FOculusXRUUID& outCeilingUUID, TArray<FOculusXRUUID>& outWallsUUIDs) const
{
	ensure(IsComponentEnabled());

	auto result = OculusXRScene::FOculusXRScene::GetRoomLayout(Space, 64, outCeilingUUID, outFloorUUID, outWallsUUIDs);
	if (!UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(result))
	{
		UE_LOG(LogOculusXRScene, Warning, TEXT("Fetching room layout failed."));
		return false;
	}

	return true;
}
