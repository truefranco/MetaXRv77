// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "MRUtilityKitBPLibrary.h"

#include "MRUtilityKit.h"
#include "Generated/MRUtilityKitShared.h"
#include "MRUtilityKitAnchor.h"
#include "MRUtilityKitSubsystem.h"
#include "ProceduralMeshComponent.h"
#include "VectorUtil.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "TextureResource.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "OculusXRRoomLayoutManagerComponent.h"

namespace
{
	TArray<FVector> RecalculateNormals(const TArray<FVector>& Vertices, const TArray<uint32>& Triangles)
	{
		TArray<FVector> Normals;

		// Initialize the normals array with zero vectors
		Normals.Init(FVector::ZeroVector, Vertices.Num());

		// Iterate through each triangle
		for (int32 TriIndex = 0; TriIndex < Triangles.Num(); TriIndex += 3)
		{
			// Get the vertices of the triangle
			FVector VertexA = Vertices[Triangles[TriIndex]];
			FVector VertexB = Vertices[Triangles[TriIndex + 1]];
			FVector VertexC = Vertices[Triangles[TriIndex + 2]];

			// Calculate the triangle's normal
			const FVector TriangleNormal = FVector::CrossProduct(VertexC - VertexA, VertexB - VertexA).GetSafeNormal();

			// Add the triangle's normal to each of its vertices' normals
			Normals[Triangles[TriIndex]] += TriangleNormal;
			Normals[Triangles[TriIndex + 1]] += TriangleNormal;
			Normals[Triangles[TriIndex + 2]] += TriangleNormal;
		}

		// Normalize the vertex normals
		for (FVector& Normal : Normals)
		{
			if (!Normal.IsNearlyZero())
			{
				Normal.Normalize();
			}
			else
			{
				Normal = FVector::UpVector;
			}
		}

		return Normals;
	}

	TArray<FProcMeshTangent> RecalculateTangents(const TArray<FVector>& Normals)
	{
		TArray<FProcMeshTangent> Tangents;

		// Initialize the tangents array with zero tangents
		Tangents.Init(FProcMeshTangent(0.f, 0.f, 0.f), Normals.Num());

		// Iterate through each normal
		for (int32 NormalIndex = 0; NormalIndex < Normals.Num(); NormalIndex++)
		{
			const FVector& Normal = Normals[NormalIndex];

			// Calculate a tangent based on the normal
			FVector TangentX = FVector(1.0f, 0.0f, 0.0f);

			// Gram-Schmidt orthogonalization
			TangentX -= Normal * FVector::DotProduct(TangentX, Normal);
			if (!TangentX.IsNearlyZero())
			{
				TangentX.Normalize();
			}
			else
			{
				TangentX = FVector::UpVector;
			}

			// Store the tangent in the array
			Tangents[NormalIndex] = FProcMeshTangent(TangentX, false);
		}

		return Tangents;
	}

	void SetScaleRecursivelyAdjustingForRotationInternal(USceneComponent* SceneComponent, const FVector& UnRotatedScale, const FQuat& AccumulatedRotation, const FVector& ParentReciprocalScale)
	{
		if (SceneComponent)
		{
			const auto RelativeRotation = SceneComponent->GetRelativeRotationCache().RotatorToQuat(SceneComponent->GetRelativeRotation());
			const auto Rotation = AccumulatedRotation * RelativeRotation;
			const FVector RotatedXAxis = Rotation.GetAxisX();
			const FVector RotatedYAxis = Rotation.GetAxisY();
			const FVector RotatedZAxis = Rotation.GetAxisZ();
			FVector RotatedScale;
			if (FMath::Abs(RotatedXAxis.X) >= UE_INV_SQRT_2)
			{
				RotatedScale.X = UnRotatedScale.X;
			}
			else if (FMath::Abs(RotatedXAxis.Y) >= UE_INV_SQRT_2)
			{
				RotatedScale.X = UnRotatedScale.Y;
			}
			else
			{
				RotatedScale.X = UnRotatedScale.Z;
			}

			if (FMath::Abs(RotatedYAxis.X) >= UE_INV_SQRT_2)
			{
				RotatedScale.Y = UnRotatedScale.X;
			}
			else if (FMath::Abs(RotatedYAxis.Y) >= UE_INV_SQRT_2)
			{
				RotatedScale.Y = UnRotatedScale.Y;
			}
			else
			{
				RotatedScale.Y = UnRotatedScale.Z;
			}

			if (FMath::Abs(RotatedZAxis.X) >= UE_INV_SQRT_2)
			{
				RotatedScale.Z = UnRotatedScale.X;
			}
			else if (FMath::Abs(RotatedZAxis.Y) >= UE_INV_SQRT_2)
			{
				RotatedScale.Z = UnRotatedScale.Y;
			}
			else
			{
				RotatedScale.Z = UnRotatedScale.Z;
			}

			const FVector OldScale = SceneComponent->GetRelativeScale3D();
			const FVector NewScale = ParentReciprocalScale * RotatedScale * OldScale;
			SceneComponent->SetRelativeScale3D(NewScale);
			const FVector NewParentReciprocalScale = ParentReciprocalScale * (OldScale / NewScale);
			for (auto Child : SceneComponent->GetAttachChildren())
			{
				if (Child)
				{
					SetScaleRecursivelyAdjustingForRotationInternal(Child, UnRotatedScale, Rotation, NewParentReciprocalScale);
				}
			}
		}
	}

	TArray<FVector> GeneratePoints(const FTransform& Plane, const FBox2D& PlaneBounds, double PointsPerUnitX, double PointsPerUnitY, double WorldToMeters = 100.0)
	{
		const FVector PlaneRight = Plane.GetRotation().GetRightVector();
		const FVector PlaneUp = Plane.GetRotation().GetUpVector();
		const FVector PlaneSize = FVector(PlaneBounds.GetSize().X, PlaneBounds.GetSize().Y, 0.0);
		const FVector PlaneBottomLeft = Plane.GetLocation() - PlaneRight * PlaneSize.X * 0.5f - PlaneUp * PlaneSize.Y * 0.5f;

		const int32 PointsX = FMath::Max(FMathf::Ceil(PointsPerUnitX * PlaneSize.X) / WorldToMeters, 1);
		const int32 PointsY = FMath::Max(FMathf::Ceil(PointsPerUnitY * PlaneSize.Y) / WorldToMeters, 1);

		const FVector2D Stride{ PlaneSize.X / (PointsX + 1), PlaneSize.Y / (PointsY + 1) };

		TArray<FVector> Points;
		Points.SetNum(PointsX * PointsY);

		for (int Iy = 0; Iy < PointsY; ++Iy)
		{
			for (int Ix = 0; Ix < PointsX; ++Ix)
			{
				const float Dx = (Ix + 1) * Stride.X;
				const float Dy = (Iy + 1) * Stride.Y;
				const FVector Point = PlaneBottomLeft + Dx * PlaneRight + Dy * PlaneUp;
				Points[Ix + Iy * PointsX] = Point;
			}
		}

		return Points;
	}
} // namespace

UMRUKLoadFromDevice* UMRUKLoadFromDevice::LoadSceneFromDeviceAsync(const UObject* WorldContext
)
{
	// We must have a valid contextual world for this action, so we don't even make it
	// unless we can resolve the UWorld from WorldContext.
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!ensureAlwaysMsgf(IsValid(WorldContext), TEXT("World Context was not valid.")))
	{
		return nullptr;
	}

	// Create a new UMyDelayAsyncAction, and store function arguments in it.
	UMRUKLoadFromDevice* NewAction = NewObject<UMRUKLoadFromDevice>();
	NewAction->World = World;
	NewAction->RegisterWithGameInstance(World->GetGameInstance());
	return NewAction;
}

void UMRUKLoadFromDevice::Activate()
{
	const auto Subsystem = World->GetGameInstance()->GetSubsystem<UMRUKSubsystem>();
	Subsystem->OnSceneLoaded.AddDynamic(this, &UMRUKLoadFromDevice::OnSceneLoaded);

	{
		Subsystem->LoadSceneFromDevice();
	}
}

void UMRUKLoadFromDevice::OnSceneLoaded(bool Succeeded)
{
	if (!World.IsValid())
	{
		UE_LOG(LogMRUK, Warning, TEXT("World is not valid anymore. Can not fire on scene loaded events"));
		return;
	}
	const auto Subsystem = World->GetGameInstance()->GetSubsystem<UMRUKSubsystem>();
	Subsystem->OnSceneLoaded.RemoveDynamic(this, &UMRUKLoadFromDevice::OnSceneLoaded);
	if (Succeeded)
	{
		Success.Broadcast();
	}
	else
	{
		Failure.Broadcast();
	}
	SetReadyToDestroy();
}

UMRUKLoadFromJson* UMRUKLoadFromJson::LoadSceneFromJsonAsync(const UObject* WorldContext, const FString& JsonString)
{
	// We must have a valid contextual world for this action, so we don't even make it
	// unless we can resolve the UWorld from WorldContext.
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!ensureAlwaysMsgf(IsValid(WorldContext), TEXT("World Context was not valid.")))
	{
		return nullptr;
	}

	// Create a new UMyDelayAsyncAction, and store function arguments in it.
	UMRUKLoadFromJson* NewAction = NewObject<UMRUKLoadFromJson>();
	NewAction->World = World;
	NewAction->Json = JsonString;
	NewAction->RegisterWithGameInstance(World->GetGameInstance());
	return NewAction;
}

void UMRUKLoadFromJson::Activate()
{
	const auto Subsystem = World->GetGameInstance()->GetSubsystem<UMRUKSubsystem>();
	Subsystem->OnSceneLoaded.AddDynamic(this, &UMRUKLoadFromJson::OnSceneLoaded);
	Subsystem->LoadSceneFromJsonString(Json);
}

void UMRUKLoadFromJson::OnSceneLoaded(bool Succeeded)
{
	if (!World.IsValid())
	{
		UE_LOG(LogMRUK, Warning, TEXT("World is not valid anymore. Can not fire on scene loaded events"));
		return;
	}
	const auto Subsystem = World->GetGameInstance()->GetSubsystem<UMRUKSubsystem>();
	Subsystem->OnSceneLoaded.RemoveDynamic(this, &UMRUKLoadFromJson::OnSceneLoaded);
	if (Succeeded)
	{
		Success.Broadcast();
	}
	else
	{
		Failure.Broadcast();
	}
	SetReadyToDestroy();
}

bool UMRUKBPLibrary::LoadGlobalMeshFromDevice(FOculusXRUInt64 SpaceHandle, UProceduralMeshComponent* OutProceduralMesh, bool LoadCollision, const UObject* WorldContext)
{
	ensure(OutProceduralMesh);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!ensureAlwaysMsgf(IsValid(WorldContext), TEXT("World Context was not valid.")))
	{
		return false;
	}

	const auto RoomLayoutManager = World->GetGameInstance()->GetSubsystem<UMRUKSubsystem>()->GetRoomLayoutManager();
	const bool LoadResult = RoomLayoutManager->LoadTriangleMesh(SpaceHandle.Value, OutProceduralMesh, LoadCollision);
	if (!LoadResult)
	{
		UE_LOG(LogMRUK, Warning, TEXT("Could not load triangle mesh from layout manager"));
		return false;
	}

	return true;
}

void UMRUKBPLibrary::RecalculateProceduralMeshAndTangents(UProceduralMeshComponent* Mesh)
{
	if (!IsValid(Mesh))
		return;

	for (int s = 0; s < Mesh->GetNumSections(); ++s)
	{
		FProcMeshSection* Section = Mesh->GetProcMeshSection(s);

		// Get vertices of the section
		TArray<FVector> Vertices;
		for (FProcMeshVertex Vertex : Section->ProcVertexBuffer)
		{
			Vertices.Add(Vertex.Position);
		}

		// Calculate normals and tangents
		TArray<FVector> Normals = RecalculateNormals(Vertices, Section->ProcIndexBuffer);
		TArray<FProcMeshTangent> Tangents = RecalculateTangents(Normals);
		TArray<FVector2D> EmptyUV;
		TArray<FColor> EmptyVertexColors;

		// Update mesh section
		Mesh->UpdateMeshSection(s, Vertices, Normals, EmptyUV, EmptyVertexColors, Tangents);
	}
}

bool UMRUKBPLibrary::IsUnrealEngineMetaFork()
{
#if defined(WITH_OCULUS_BRANCH)
	return true;
#else
	return false;
#endif
}

FVector2D UMRUKBPLibrary::ComputeCentroid(const TArray<FVector2D>& PolygonPoints)
{
	FVector2D Centroid = FVector2D::ZeroVector;

	double SignedArea = 0.0;
	for (int32 I = 0; I < PolygonPoints.Num(); ++I)
	{
		const double X0 = PolygonPoints[I].X;
		const double Y0 = PolygonPoints[I].Y;
		const double X1 = PolygonPoints[(I + 1) % PolygonPoints.Num()].X;
		const double Y1 = PolygonPoints[(I + 1) % PolygonPoints.Num()].Y;

		const double A = X0 * Y1 - X1 * Y0;
		SignedArea += A;

		Centroid.X += (X0 + X1) * A;
		Centroid.Y += (Y0 + Y1) * A;
	}

	return Centroid / (6.0 * (SignedArea * 0.5));
}

void UMRUKBPLibrary::SetScaleRecursivelyAdjustingForRotation(USceneComponent* SceneComponent, const FVector& UnRotatedScale)
{
	SetScaleRecursivelyAdjustingForRotationInternal(SceneComponent, UnRotatedScale, FQuat::Identity, FVector::OneVector);
}

FVector UMRUKBPLibrary::ComputeDirectionAwayFromClosestWall(const AMRUKAnchor* Anchor, int& OutCardinalAxisIndex, const TArray<int> ExcludedAxes)
{
	double ClosestWallDistance = DBL_MAX;
	FVector AwayFromWall{};

	for (int i = 0; i < 4; ++i)
	{
		if (ExcludedAxes.Contains(i))
		{
			continue;
		}
		// Shoot a ray along the cardinal directions
		// The "Up" (i.e. Z axis) for anchors typically points away from the facing direction, but it depends
		// entirely on how the user defined the volume in scene capture.
		const auto CardinalAxis = (FQuat::MakeFromEuler({ 0.0, 0.0, 90.0 * i }).RotateVector(Anchor->GetActorUpVector()));

		for (const auto& WallAnchor : Anchor->Room->WallAnchors)
		{
			if (!WallAnchor)
			{
				continue;
			}
			FMRUKHit Hit{};
			if (!WallAnchor->Raycast(Anchor->GetActorLocation(), CardinalAxis, 0.0, Hit))
			{
				continue;
			}
			const auto DistToWall = FVector::Distance(Hit.HitPosition, Anchor->GetActorLocation());
			if (DistToWall < ClosestWallDistance)
			{
				ClosestWallDistance = DistToWall;
				AwayFromWall = -CardinalAxis;
				OutCardinalAxisIndex = i;
			}
		}
	}

	return AwayFromWall;
}

UTexture2D* UMRUKBPLibrary::ConstructTexture2D(UTextureRenderTarget2D* RenderTarget2D, UObject* Outer, const FString& TexName)
{
	const auto SizeX = RenderTarget2D->SizeX;
	const auto SizeY = RenderTarget2D->SizeY;
	const auto Tex = UTexture2D::CreateTransient(SizeX, SizeY, RenderTarget2D->GetFormat());
	Tex->AddToRoot();
	Tex->Filter = TF_Bilinear;
	Tex->CompressionSettings = TC_Default;
	Tex->SRGB = 0;
	Tex->UpdateResource();

	FTextureRenderTargetResource* RenderTargetResource = RenderTarget2D->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false);

	TArray<FColor> OutBMP;
	RenderTargetResource->ReadPixels(OutBMP, ReadSurfaceDataFlags);

	FTexture2DMipMap& Mip = Tex->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, OutBMP.GetData(), SizeX * SizeY * 4);
	Mip.BulkData.Unlock();

	Tex->UpdateResource();
	return Tex;
}

FLinearColor UMRUKBPLibrary::GetMatrixColumn(const FMatrix& Matrix, int32 Index)
{
	ensure(0 <= Index && Index < 4);
	FLinearColor V;
	V.R = Matrix.M[0][Index];
	V.G = Matrix.M[1][Index];
	V.B = Matrix.M[2][Index];
	V.A = Matrix.M[3][Index];
	return V;
}

TArray<FVector> UMRUKBPLibrary::ComputeRoomBoxGrid(const AMRUKRoom* Room, int32 MaxPointsCount, double PointsPerUnitX, double PointsPerUnitY)
{
	TArray<FVector> AllPoints;

	const double WorldToMeters = Room->GetWorld()->GetWorldSettings()->WorldToMeters;
	for (const AMRUKAnchor* WallAnchor : Room->WallAnchors)
	{

		const auto Points = GeneratePoints(WallAnchor->GetTransform(), WallAnchor->PlaneBounds, PointsPerUnitX, PointsPerUnitY, WorldToMeters);
		AllPoints.Append(Points);
	}

	// Generate points between floor and ceiling
	const float DistFloorCeiling = Room->CeilingAnchor->GetTransform().GetLocation().Z - Room->FloorAnchor->GetTransform().GetLocation().Z;
	const int32 PlanesCount = FMath::Max(FMathf::Ceil(PointsPerUnitY * DistFloorCeiling) / WorldToMeters, 1);
	const int32 SpaceBetweenPlanes = DistFloorCeiling / PlanesCount;
	for (int i = 1; i < PlanesCount; ++i)
	{
		FTransform Transform = Room->CeilingAnchor->GetTransform();
		Transform.SetLocation(FVector(Transform.GetLocation().X, Transform.GetLocation().Y, Transform.GetLocation().Z - (SpaceBetweenPlanes * i)));
		const auto Points = GeneratePoints(Transform, Room->CeilingAnchor->PlaneBounds, PointsPerUnitX, PointsPerUnitY, WorldToMeters);
		AllPoints.Append(Points);
	}

	const auto CeilingPoints = GeneratePoints(Room->CeilingAnchor->GetTransform(), Room->CeilingAnchor->PlaneBounds, PointsPerUnitX, PointsPerUnitY, WorldToMeters);
	AllPoints.Append(CeilingPoints);

	const auto FloorPoints = GeneratePoints(Room->FloorAnchor->GetTransform(), Room->FloorAnchor->PlaneBounds, PointsPerUnitX, PointsPerUnitY, WorldToMeters);
	AllPoints.Append(FloorPoints);

	if (AllPoints.Num() > MaxPointsCount)
	{
		// Shuffle the array
		AllPoints.Sort([](const FVector& /*Item1*/, const FVector& /*Item2*/) {
			return FMath::FRand() < 0.5f;
		});

		// Randomly remove some points
		int32 PointsToRemoveCount = AllPoints.Num() - MaxPointsCount;
		while (PointsToRemoveCount > 0)
		{
			AllPoints.Pop();
			--PointsToRemoveCount;
		}
	}
	return AllPoints;
}

void UMRUKBPLibrary::CreateMeshSegmentation(const TArray<FVector>& MeshPositions, const TArray<uint32>& MeshIndices,
	const TArray<FVector>& SegmentationPoints, const FVector& ReservedMin, const FVector& ReservedMax,
	TArray<FMRUKMeshSegment>& OutSegments, FMRUKMeshSegment& OutReservedSegment)
{
	if (!MRUKShared::GetInstance())
	{
		UE_LOG(LogMRUK, Error, TEXT("MRUK shared library is not available. To use this functionality make sure the library is included"));
		return;
	}

	TArray<FVector3f> MeshPositionsF;
	MeshPositionsF.Reserve(MeshPositions.Num());
	for (const FVector& V : MeshPositions)
	{
		MeshPositionsF.Add(FVector3f(V));
	}

	TArray<FVector3f> SegmentationPointsF;
	SegmentationPointsF.Reserve(SegmentationPoints.Num());
	for (const FVector& V : SegmentationPoints)
	{
		SegmentationPointsF.Add(FVector3f(V));
	}

	MRUKShared::MrukMesh3f* MeshSegmentsF = nullptr;
	uint32_t MeshSegmentsCount = 0;

	MRUKShared::MrukMesh3f ReservedMeshSegmentF{};

	const FVector3f ReservedMinF(ReservedMin);
	const FVector3f ReservedMaxF(ReservedMax);

	MRUKShared::GetInstance()->ComputeMeshSegmentation(MeshPositionsF.GetData(), MeshPositionsF.Num(), MeshIndices.GetData(),
		MeshIndices.Num(), SegmentationPointsF.GetData(), SegmentationPointsF.Num(), ReservedMinF, ReservedMaxF, &MeshSegmentsF,
		&MeshSegmentsCount, &ReservedMeshSegmentF);

	OutSegments.Reserve(MeshSegmentsCount);
	for (uint32_t i = 0; i < MeshSegmentsCount; ++i)
	{
		const MRUKShared::MrukMesh3f& SegmentF = MeshSegmentsF[i];
		if (SegmentF.numIndices == 0)
		{
			continue;
		}

		FMRUKMeshSegment MeshSegment{};
		MeshSegment.Indices.Reserve(SegmentF.numIndices);
		MeshSegment.Positions.Reserve(SegmentF.numVertices);
		for (uint32_t j = 0; j < SegmentF.numIndices; ++j)
		{
			MeshSegment.Indices.Add(SegmentF.indices[j]);
		}
		for (uint32_t j = 0; j < SegmentF.numVertices; ++j)
		{
			const FVector3f& V = SegmentF.vertices[j];
			MeshSegment.Positions.Add({ V.X, V.Y, V.Z });
		}

		OutSegments.Emplace(MoveTemp(MeshSegment));
	}

	if (ReservedMeshSegmentF.numIndices && ReservedMeshSegmentF.numVertices)
	{
		OutReservedSegment.Indices.Reserve(ReservedMeshSegmentF.numIndices);
		OutReservedSegment.Positions.Reserve(ReservedMeshSegmentF.numVertices);
		for (uint32_t j = 0; j < ReservedMeshSegmentF.numIndices; ++j)
		{
			OutReservedSegment.Indices.Add(ReservedMeshSegmentF.indices[j]);
		}
		for (uint32_t j = 0; j < ReservedMeshSegmentF.numVertices; ++j)
		{
			const FVector3f& V = ReservedMeshSegmentF.vertices[j];
			OutReservedSegment.Positions.Add({ V.X, V.Y, V.Z });
		}
	}

	MRUKShared::GetInstance()->FreeMeshSegmentation(MeshSegmentsF, MeshSegmentsCount, &ReservedMeshSegmentF);
}
