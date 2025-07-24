// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "MRUtilityKitDestructibleMesh.h"

#include "Engine/GameInstance.h"
#include "MRUtilityKitBPLibrary.h"
#include "MRUtilityKitSubsystem.h"
#include "MRUtilityKitAnchor.h"
#include "MRUtilityKitRoom.h"
#include "MRUtilityKitTelemetry.h"
#include "OculusXRTelemetry.h"
#include "Tasks/Task.h"

constexpr const char* RESERVED_MESH_SEGMENT_TAG = "ReservedMeshSegment";

UMRUKDestructibleMeshComponent::UMRUKDestructibleMeshComponent(const FObjectInitializer& ObjectInitializer)
	: UProceduralMeshComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMRUKDestructibleMeshComponent::SegmentMesh(const TArray<FVector>& MeshPositions, const TArray<uint32>& MeshIndices, const TArray<FVector>& SegmentationPoints)
{
	TaskResult = UE::Tasks::Launch(UE_SOURCE_LOCATION, [this, MeshPositions, MeshIndices, SegmentationPoints]() {
		TArray<FMRUKMeshSegment> Segments;
		FMRUKMeshSegment ReservedMeshSegment;

		const FVector ReservedMin(ReservedTop, -1.0, -1.0);
		const FVector ReservedMax(ReservedBottom, -1.0, -1.0);

		UMRUKBPLibrary::CreateMeshSegmentation(MeshPositions, MeshIndices, SegmentationPoints, ReservedMin, ReservedMax, Segments, ReservedMeshSegment);
		return TPair<TArray<FMRUKMeshSegment>, FMRUKMeshSegment>{ MoveTemp(Segments), MoveTemp(ReservedMeshSegment) };
	});
	SetComponentTickEnabled(true);
}

void UMRUKDestructibleMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UMRUKDestructibleMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!TaskResult.IsCompleted())
	{
		return;
	}

	const auto& [MeshSegments, ReservedMeshSegment] = TaskResult.GetResult();

	for (int32 i = 0; i < MeshSegments.Num(); ++i)
	{
		const auto& [Positions, Indices] = MeshSegments[i];
		const FString ProcMeshName = FString::Printf(TEXT("DestructibleMeshSegment%d"), i);
		const auto ProcMesh = NewObject<UProceduralMeshComponent>(GetOwner(), *ProcMeshName);
		const FAttachmentTransformRules TransformRules{ EAttachmentRule::KeepRelative, false };
		ProcMesh->AttachToComponent(GetOwner()->GetRootComponent(), TransformRules);
		ProcMesh->RegisterComponent();
		ProcMesh->ComponentTags.AddUnique(TEXT("DestructibleMeshSegment"));
		GetOwner()->AddInstanceComponent(ProcMesh);
		ProcMesh->CreateMeshSection(0, Positions, Indices, {}, {}, {}, {}, true);
		if (GlobalMeshMaterial)
		{
			ProcMesh->SetMaterial(0, GlobalMeshMaterial);
		}
	}
	if (ReservedMeshSegment.Indices.Num() > 0)
	{
		const auto ProcMesh = NewObject<UProceduralMeshComponent>(GetOwner(), TEXT("ReservedMeshSegment"));
		const FAttachmentTransformRules TransformRules{ EAttachmentRule::KeepRelative, false };
		ProcMesh->AttachToComponent(GetOwner()->GetRootComponent(), TransformRules);
		ProcMesh->RegisterComponent();
		ProcMesh->ComponentTags.AddUnique(RESERVED_MESH_SEGMENT_TAG);
		GetOwner()->AddInstanceComponent(ProcMesh);
		ProcMesh->CreateMeshSection(0, ReservedMeshSegment.Positions, ReservedMeshSegment.Indices, {}, {}, {}, {}, true);
		if (GlobalMeshMaterial)
		{
			ProcMesh->SetMaterial(0, GlobalMeshMaterial);
		}
	}

	SetComponentTickEnabled(false);

	OnMeshesGenerated.Broadcast();
}

AMRUKDestructibleGlobalMesh::AMRUKDestructibleGlobalMesh()
{
	DestructibleMeshComponent = CreateDefaultSubobject<UMRUKDestructibleMeshComponent>(TEXT("DestructibleMesh"));
	SetRootComponent(DestructibleMeshComponent);
}

void AMRUKDestructibleGlobalMesh::CreateDestructibleMesh(AMRUKRoom* Room)
{
	const UMRUKSubsystem* MRUKSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMRUKSubsystem>();
	if (!Room)
	{
		Room = MRUKSubsystem->GetCurrentRoom();
	}
	if (!Room)
	{
		UE_LOG(LogMRUK, Warning, TEXT("Could not find a room for the destructible mesh"));
		return;
	}
	if (!Room->GlobalMeshAnchor)
	{
		UE_LOG(LogMRUK, Warning, TEXT("No global mesh available for creating a destructible mesh"));
		return;
	}
	const AMRUKAnchor* GlobalMesh = Room->GlobalMeshAnchor;
	UProceduralMeshComponent* GlobalProcMesh = Cast<UProceduralMeshComponent>(GlobalMesh->GetComponentByClass(UProceduralMeshComponent::StaticClass()));
	if (!GlobalProcMesh)
	{
		Room->LoadGlobalMeshFromDevice();
	}
	if (!GlobalProcMesh)
	{
		UE_LOG(LogMRUK, Warning, TEXT("Could not load a triangle mesh from the global mesh"));
		return;
	}

	// Attach to the global mesh
	const FAttachmentTransformRules AttachmentTransformRules{ EAttachmentRule::KeepRelative, false };
	AttachToActor(Room->GlobalMeshAnchor, AttachmentTransformRules);

	// Get global mesh data
	ensure(GlobalProcMesh);
	ensure(GlobalProcMesh->ComponentHasTag("GlobalMesh"));
	FProcMeshSection* ProcMeshSection = GlobalProcMesh->GetProcMeshSection(0);
	TArray<FVector> MeshPositions;
	MeshPositions.SetNum(ProcMeshSection->ProcVertexBuffer.Num());
	for (int32 i = 0; i < ProcMeshSection->ProcVertexBuffer.Num(); ++i)
	{
		MeshPositions[i] = ProcMeshSection->ProcVertexBuffer[i].Position * GetWorldSettings()->WorldToMeters;
	}
	const TArray<uint32>& MeshIndices = ProcMeshSection->ProcIndexBuffer;

	TArray<FVector> SegmentationPointsWS = UMRUKBPLibrary::ComputeRoomBoxGrid(Room, MaxPointsCount, PointsPerUnitX, PointsPerUnitY);

	TArray<FVector> SegmentationPointsLS;
	SegmentationPointsLS.SetNum(SegmentationPointsWS.Num());
	const FTransform T = GlobalMesh->GetActorTransform().Inverse();
	for (int32 i = 0; i < SegmentationPointsWS.Num(); ++i)
	{
		SegmentationPointsLS[i] = T.TransformPosition(SegmentationPointsWS[i]);
	}
	DestructibleMeshComponent->SegmentMesh(MeshPositions, MeshIndices, SegmentationPointsLS);
}

void AMRUKDestructibleGlobalMesh::RemoveGlobalMeshSegment(UPrimitiveComponent* Mesh)
{
	if (!Mesh->ComponentTags.Contains(RESERVED_MESH_SEGMENT_TAG))
	{
		// Only remove mesh segments that are allowed to be destroyed
		Mesh->DestroyComponent();
	}
}

void AMRUKDestructibleGlobalMeshSpawner::BeginPlay()
{
	Super::BeginPlay();
	OculusXRTelemetry::TScopedMarker<MRUKTelemetry::FLoadDestructibleGlobalMeshSpawner> Event(static_cast<int>(GetTypeHash(this)));

	if (SpawnMode == EMRUKSpawnMode::CurrentRoomOnly)
	{
		const auto Subsystem = GetGameInstance()->GetSubsystem<UMRUKSubsystem>();
		if (Subsystem->SceneLoadStatus == EMRUKInitStatus::Complete)
		{
			AddDestructibleGlobalMesh(Subsystem->GetCurrentRoom());
		}
		else
		{
			// Only listen for the room created event in case no current room was available yet
			Subsystem->OnRoomCreated.AddUniqueDynamic(this, &AMRUKDestructibleGlobalMeshSpawner::OnRoomCreated);
			// Remove destructible meshes as soon as the room gets removed
			Subsystem->OnRoomRemoved.AddUniqueDynamic(this, &AMRUKDestructibleGlobalMeshSpawner::OnRoomRemoved);
		}
	}
	else if (SpawnMode == EMRUKSpawnMode::AllRooms)
	{
		const auto Subsystem = GetGameInstance()->GetSubsystem<UMRUKSubsystem>();
		for (auto Room : Subsystem->Rooms)
		{
			AddDestructibleGlobalMesh(Room);
		}

		// Listen for new rooms that get created
		Subsystem->OnRoomCreated.AddUniqueDynamic(this, &AMRUKDestructibleGlobalMeshSpawner::OnRoomCreated);
		// Remove destructible meshes as soon as the room gets removed
		Subsystem->OnRoomRemoved.AddUniqueDynamic(this, &AMRUKDestructibleGlobalMeshSpawner::OnRoomRemoved);
	}
}

void AMRUKDestructibleGlobalMeshSpawner::OnRoomCreated(AMRUKRoom* Room)
{
	if (SpawnMode == EMRUKSpawnMode::CurrentRoomOnly && GetGameInstance()->GetSubsystem<UMRUKSubsystem>()->GetCurrentRoom() != Room)
	{
		// Skip this room if it is not the current room
		return;
	}
	AddDestructibleGlobalMesh(Room);
}

void AMRUKDestructibleGlobalMeshSpawner::OnRoomRemoved(AMRUKRoom* Room)
{
	if (!IsValid(Room))
	{
		return;
	}

	if (AMRUKDestructibleGlobalMesh** Mesh = SpawnedMeshes.Find(Room))
	{
		(*Mesh)->Destroy();
		SpawnedMeshes.Remove(Room);
	}
}

AMRUKDestructibleGlobalMesh* AMRUKDestructibleGlobalMeshSpawner::FindDestructibleMeshForRoom(AMRUKRoom* Room)
{
	if (AMRUKDestructibleGlobalMesh** Mesh = SpawnedMeshes.Find(Room))
	{
		return *Mesh;
	}
	return nullptr;
}

AMRUKDestructibleGlobalMesh* AMRUKDestructibleGlobalMeshSpawner::AddDestructibleGlobalMesh(AMRUKRoom* Room)
{
	if (SpawnedMeshes.Contains(Room))
	{
		return SpawnedMeshes[Room];
	}

	AMRUKDestructibleGlobalMesh* Mesh = GetWorld()->SpawnActor<AMRUKDestructibleGlobalMesh>();
	Mesh->PointsPerUnitX = PointsPerUnitX;
	Mesh->PointsPerUnitY = PointsPerUnitY;
	Mesh->MaxPointsCount = MaxPointsCount;
	Mesh->DestructibleMeshComponent->GlobalMeshMaterial = GlobalMeshMaterial;
	Mesh->DestructibleMeshComponent->ReservedBottom = ReservedBottom;
	Mesh->DestructibleMeshComponent->ReservedTop = ReservedTop;
	Mesh->CreateDestructibleMesh();
	SpawnedMeshes.Add(Room, Mesh);

	return Mesh;
}
