// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "UObject/Class.h"
#include "OculusXRAnchorComponents.h"
#include "OculusXRSceneComponents.generated.h"

UCLASS(Blueprintable)
class OCULUSXRSCENE_API UOculusXRPlaneAnchorComponent : public UOculusXRBaseAnchorComponent
{
	GENERATED_BODY()
public:
	UOculusXRPlaneAnchorComponent()
	{
		Type = EOculusXRSpaceComponentType::ScenePlane;
	}

	UFUNCTION(BlueprintCallable, Category = "OculusXR|SpatialAnchor")
	bool GetPositionAndSize(FVector& outPosition, FVector& outSize) const;
};

UCLASS(Blueprintable)
class OCULUSXRSCENE_API UOculusXRVolumeAnchorComponent : public UOculusXRBaseAnchorComponent
{
	GENERATED_BODY()
public:
	UOculusXRVolumeAnchorComponent()
	{
		Type = EOculusXRSpaceComponentType::SceneVolume;
	}

	UFUNCTION(BlueprintCallable, Category = "OculusXR|SpatialAnchor")
	bool GetPositionAndSize(FVector& outPosition, FVector& outSize) const;
};

UCLASS(Blueprintable)
class OCULUSXRSCENE_API UOculusXRSemanticClassificationAnchorComponent : public UOculusXRBaseAnchorComponent
{
	GENERATED_BODY()
public:
	UOculusXRSemanticClassificationAnchorComponent()
	{
		Type = EOculusXRSpaceComponentType::SemanticClassification;
	}

	UFUNCTION(BlueprintCallable, Category = "OculusXR|SpatialAnchor")
	bool GetSemanticClassifications(TArray<FString>& outClassifications) const;
};

UCLASS(Blueprintable)
class OCULUSXRSCENE_API UOculusXRRoomLayoutAnchorComponent : public UOculusXRBaseAnchorComponent
{
	GENERATED_BODY()
public:
	UOculusXRRoomLayoutAnchorComponent()
	{
		Type = EOculusXRSpaceComponentType::RoomLayout;
	}

	UFUNCTION(BlueprintCallable, Category = "OculusXR|SpatialAnchor")
	bool GetRoomLayout(FOculusXRUUID& outFloorUUID, FOculusXRUUID& outCeilingUUID, TArray<FOculusXRUUID>& outWallsUUIDs) const;
};

UCLASS(Blueprintable)
class OCULUSXRSCENE_API UOculusXRTriangleMeshAnchorComponent : public UOculusXRBaseAnchorComponent
{
	GENERATED_BODY()
public:
	UOculusXRTriangleMeshAnchorComponent()
	{
		Type = EOculusXRSpaceComponentType::TriangleMesh;
	}
};
