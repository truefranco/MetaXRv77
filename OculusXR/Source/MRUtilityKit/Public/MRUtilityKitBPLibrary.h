// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "OculusXRAnchorTypes.h"
#include "MRUtilityKitBPLibrary.generated.h"

USTRUCT(BlueprintType)
struct FMRUKMeshSegment
{
	GENERATED_BODY()

	TArray<FVector> Positions;
	TArray<int32> Indices;
};

/**
 * Load the scene async from device.
 */
UCLASS()
class MRUTILITYKIT_API UMRUKLoadFromDevice : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMRUKLoaded);

	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit", meta = (WorldContext = "WorldContext", BlueprintInternalUseOnly = "true"
																  ))
	static UMRUKLoadFromDevice* LoadSceneFromDeviceAsync(const UObject* WorldContext
	);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FMRUKLoaded Success;

	UPROPERTY(BlueprintAssignable)
	FMRUKLoaded Failure;

private:
	UFUNCTION(CallInEditor)
	void OnSceneLoaded(bool Succeeded);

	TWeakObjectPtr<UWorld> World = nullptr;
};

UCLASS()
class MRUTILITYKIT_API UMRUKLoadFromJson : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMRUKLoaded);

	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit", meta = (WorldContext = "WorldContext", BlueprintInternalUseOnly = "true"))
	static UMRUKLoadFromJson* LoadSceneFromJsonAsync(const UObject* WorldContext, const FString& JsonString);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FMRUKLoaded Success;

	UPROPERTY(BlueprintAssignable)
	FMRUKLoaded Failure;

private:
	FString Json;

	UFUNCTION(CallInEditor)
	void OnSceneLoaded(bool Succeeded);

	TWeakObjectPtr<UWorld> World = nullptr;
};

/**
 * Mixed Reality Utility Kit Blueprint Function Library.
 * See functions for further information.
 */
UCLASS()
class MRUTILITYKIT_API UMRUKBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Load the global mesh from the device.
	 * @param SpaceHandle       Space handle of the room.
	 * @param OutProceduralMesh Procedural mesh to load the triangle data in.
	 * @param LoadCollision     Whether to generate collision or not.
	 * @param WorldContext      Context of the world.
	 * @return                  Whether the load was successful or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit", meta = (WorldContext = "WorldContext"))
	static bool LoadGlobalMeshFromDevice(FOculusXRUInt64 SpaceHandle, UProceduralMeshComponent* OutProceduralMesh, bool LoadCollision, const UObject* WorldContext);

	/**
	 * (Re)Calculate Normals and Tangents of the given procedural mesh.
	 * @param Mesh The procedural mesh.
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	static void RecalculateProceduralMeshAndTangents(class UProceduralMeshComponent* Mesh);

	/**
	 * Check if the current Unreal Engine is the fork of Meta.
	 * @return Whether its the fork or not.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MR Utility Kit")
	static bool IsUnrealEngineMetaFork();

	/**
	 *  Compute the centroid of a polygon that is defined by the points.
	 *  The centroid may be outside of the polygon in case the polygon is non convex.
	 * @param PolygonPoints Points that define the polygon.
	 * @return The centroid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MR Utility Kit")
	static FVector2D ComputeCentroid(const TArray<FVector2D>& PolygonPoints);

	/**
	 * In Unreal Engine, scale is always applied in the local space to avoid any skew.
	 * This means that if you have a component which has a 90 degree rotation and is scaled, or any of its
	 * children are scaled then the scale axes will not be applied as you would expect. This is can make it
	 * very awkward to work with when trying to scale the actors to fit within the scene volumes. To work around
	 * this problem, this function will attempt to adjust the scale axes recursively to match the expected behaviour.
	 * This will only work reliably if the rotations involved are 90 degrees, if they are not then it will pick the closest axis.
	 * @param SceneComponent        The component where the scale should be set
	 * @param UnRotatedScale        The scale you would like to have without considering any rotations
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	static void SetScaleRecursivelyAdjustingForRotation(USceneComponent* SceneComponent, const FVector& UnRotatedScale);

	/**
	 * Compute the direction that faces away from the closest wall of the given anchor.
	 * @param Anchor The anchor for which the direction should be computed.
	 * @param OutCardinalAxisIndex The index of the computed cardinal axis. Can be either 0, 1, 2 or 3
	 * @param ExcludedAxes Axes to exclude in the computation. Can contain 0, 1, 2, 3
	 * @return The direction
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	static FVector ComputeDirectionAwayFromClosestWall(const AMRUKAnchor* Anchor, int& OutCardinalAxisIndex, const TArray<int> ExcludedAxes);

	/**
	 * Construct a 2D texture from a render target.
	 * @param RenderTarget2D The render target from which the texture should be created.
	 * @param Outer	The (optional) outer object for the created texture.
	 * @param TexName Name for the new texture.
	 * @return The newly created texture.
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	static UTexture2D* ConstructTexture2D(UTextureRenderTarget2D* RenderTarget2D, UObject* Outer, const FString& TexName);

	/**
	 * Extract a column from a matrix.
	 * @param Matrix The matrix to use.
	 * @param Index The column index.
	 * @return The column of the matrix.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MR Utility Kit")
	static FLinearColor GetMatrixColumn(const FMatrix& Matrix, int32 Index);

	/**
	 * Compute a grid by taking into account the room box geometry. E.g. create evenly spaced points on ceiling, floor and walls.
	 * @param Room The room to use
	 * @param MaxPointsCount The maximum number of points
	 * @param PointsPerUnitX The density of points on the X axis
	 * @param PointsPerUnitY The density of points on the Y axis
	 * @param bIncludeFloor Whether or not to include the floor
	 * @param bIncludeCeiling Whether or not to include the ceiling
	 * @param bIncludeWalls Whether or not to include the walls
	 * @return The computed points
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	static TArray<FVector> ComputeRoomBoxGrid(const AMRUKRoom* Room, int32 MaxPointsCount, double PointsPerUnitX = 1.0, double PointsPerUnitY = 1.0);

	/**
	 * Create mesh segments from the given mesh. This can be used for creating a destructible mesh system.
	 * @param MeshPositions The mesh positions that should be segmented
	 * @param MeshIndices The mesh indices that should be segmented
	 * @param SegmentationPoints A set of points that should be used to calculate the segments
	 * @param ReservedMin Reserved space from the lower part of the bound box
	 * @param ReservedMax Reserved space from the upper part of the bounding box
	 * @param OutSegments The segmented meshes that have been created from the given mesh
	 * @param OutReservedSegment
	 */
	static void CreateMeshSegmentation(const TArray<FVector>& MeshPositions, const TArray<uint32>& MeshIndices,
		const TArray<FVector>& SegmentationPoints, const FVector& ReservedMin, const FVector& ReservedMax,
		TArray<FMRUKMeshSegment>& OutSegments, FMRUKMeshSegment& OutReservedSegment);
};
