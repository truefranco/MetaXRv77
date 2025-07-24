// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "MRUtilityKit.h"
#include "MRUtilityKitBPLibrary.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Tasks/Task.h"
#include "MRUtilityKitDestructibleMesh.generated.h"

/**
 * Destructible mesh component. Creates mesh segments for the given geometry.
 * The segments will be created async.
 * In addition, its possible to define areas that are indestructible.
 */
UCLASS(ClassGroup = MRUtilityKit, Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent, DisplayName = "MR Utility Kit Destructible Mesh Component"))
class MRUTILITYKIT_API UMRUKDestructibleMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeshesGenerated);

	UMRUKDestructibleMeshComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "MR Utility Kit")
	FOnMeshesGenerated OnMeshesGenerated;

	/**
	 * Material to display on the global mesh
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	UMaterialInterface* GlobalMeshMaterial;

	/**
	 * Area on the top of the mesh that should be indestructible.
	 * The area is given in centimeters 1.0 == 1 cm.
	 * -1.0 means no reserved area.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	double ReservedTop = -1.0;

	/**
	 * Area on the bottom of the mesh that should be indestructible.
	 * The area is given in centimeters 1.0 == 1 cm
	 * -1.0 means no reserved area.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	double ReservedBottom = 30.0;

	/**
	 * Segment the given geometry into smaller chunks. For each chunk a procedural mesh component will be spawned and attached to the owning actor.
	 * @param MeshPositions Positions of the mesh to segment
	 * @param MeshIndices Indices of the mesh to segment
	 * @param SegmentationPoints Points to use to determine the segments.
	 */
	void SegmentMesh(const TArray<FVector>& MeshPositions, const TArray<uint32>& MeshIndices, const TArray<FVector>& SegmentationPoints);

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UE::Tasks::TTask<TPair<TArray<FMRUKMeshSegment>, FMRUKMeshSegment>> TaskResult;
};

/**
 * Actor that constructs a destructible mesh for the given room
 * The actor will automatically attach to the global mesh anchor of the given room to take it location and orientation.
 */
UCLASS(ClassGroup = MRUtilityKit, Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent, DisplayName = "MR Utility Kit Destructible Global Mesh"))
class MRUTILITYKIT_API AMRUKDestructibleGlobalMesh : public AActor
{
	GENERATED_BODY()

public:
	AMRUKDestructibleGlobalMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MR Utility Kit")
	UMRUKDestructibleMeshComponent* DestructibleMeshComponent;

	/**
	 * Density of mesh segments on the X axis.
	 * Increase this value to get smaller cracks in the global mesh.
	 * Decrease this value to get bigger cracks in the global mesh.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	double PointsPerUnitX = 1.0;

	/**
	 * How many segmentation points should be created at a maximum.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	int MaxPointsCount = 256;

	/**
	 * Density of mesh segments on the Y axis.
	 * Increase this value to get smaller cracks in the global mesh.
	 * Decrease this value to get bigger cracks in the global mesh.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	double PointsPerUnitY = 1.0;

	/**
	 * Create a destructible mesh for the given room. If the global mesh has not yet been loaded
	 * this function will attempt to load the global mesh from the device.
	 * @param Room The room
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	void CreateDestructibleMesh(AMRUKRoom* Room = nullptr);

	/**
	 * Remove a segment of the global mesh. Takes care of not removing the reserved global mesh segment.
	 * @param Mesh The mesh to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	void RemoveGlobalMeshSegment(UPrimitiveComponent* Mesh);
};

/**
 * The destructible global mesh spawner allows to spawn (automatically) destructible global meshes
 * when new rooms are created.
 * A destructible global mesh is a version of the global mesh that can be destructed during runtime.
 * The bulk of the work is performed in UDestructibleMeshComponent. It will perform on start a one time
 * preprocessing step to segment the given global mesh into smaller chunks. After that the chunks can be used
 * during the game and removed (e.g. with ray casts) at any time during the game simulating as if the global
 * mesh would crack down. To enhance the visual quality when cracking the (e.g. removing mesh chunks) global mesh
 * a particle system could be used. The system allows to define areas that should be non destructible.
 */
UCLASS(ClassGroup = MRUtilityKit, Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent, DisplayName = "MR Utility Kit Destructible Global Mesh Spawner"))
class MRUTILITYKIT_API AMRUKDestructibleGlobalMeshSpawner : public AActor
{
	GENERATED_BODY()

public:
	/**
	 * Whether destructible meshes should be spawned automatically.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	EMRUKSpawnMode SpawnMode = EMRUKSpawnMode::CurrentRoomOnly;

	/**
	 * Material to display on the global mesh
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	UMaterialInterface* GlobalMeshMaterial;

	/**
	 * Density of mesh segments on the X axis.
	 * Increase this value to get smaller cracks in the global mesh.
	 * Decrease this value to get bigger cracks in the global mesh.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	double PointsPerUnitX = 1.0;

	/**
	 * How many segmentation points should be created at a maximum.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	int MaxPointsCount = 256;

	/**
	 * Density of mesh segments on the Y axis.
	 * Increase this value to get smaller cracks in the global mesh.
	 * Decrease this value to get bigger cracks in the global mesh.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	double PointsPerUnitY = 1.0;

	/**
	 * Area on the top of the mesh that should be indestructible.
	 * The area is given in centimeters 1.0 == 1 cm
	 * -1.0 means no reserved area.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	double ReservedTop = -1.0;

	/**
	 * Area on the bottom of the mesh that should be indestructible.
	 * The area is given in centimeters 1.0 == 1 cm
	 * -1.0 means no reserved area.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	double ReservedBottom = 30.0;

	void BeginPlay() override;

	/**
	 * Find the destructible mesh that has been spawned for the given room.
	 * @param Room Room to look for the destructible mesh
	 * @return The destructible mesh
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	AMRUKDestructibleGlobalMesh* FindDestructibleMeshForRoom(AMRUKRoom* Room);

	/**
	 * Add new destructible mesh for the given room. A mesh will only get spawned if no
	 * destructible mesh has been spawned for the room yet.
	 * @param Room The room.
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	AMRUKDestructibleGlobalMesh* AddDestructibleGlobalMesh(AMRUKRoom* Room);

private:
	TMap<AMRUKRoom*, AMRUKDestructibleGlobalMesh*> SpawnedMeshes;

	UFUNCTION()
	void OnRoomCreated(AMRUKRoom* Room);

	UFUNCTION()
	void OnRoomRemoved(AMRUKRoom* Room);
};
