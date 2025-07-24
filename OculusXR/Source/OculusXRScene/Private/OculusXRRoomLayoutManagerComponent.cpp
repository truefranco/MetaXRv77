// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRRoomLayoutManagerComponent.h"
#include "OculusXRScene.h"
#include "OculusXRSceneModule.h"
#include "OculusXRSceneDelegates.h"
#include "OculusXRSceneFunctionLibrary.h"
#include "ProceduralMeshComponent.h"

UOculusXRRoomLayoutManagerComponent::UOculusXRRoomLayoutManagerComponent(const FObjectInitializer& ObjectInitializer)
{
	bWantsInitializeComponent = true; // so that InitializeComponent() gets called
}

void UOculusXRRoomLayoutManagerComponent::OnRegister()
{
	Super::OnRegister();

	FOculusXRSceneEventDelegates::OculusSceneCaptureComplete.AddUObject(this, &UOculusXRRoomLayoutManagerComponent::OculusRoomLayoutSceneCaptureComplete_Handler);
}

void UOculusXRRoomLayoutManagerComponent::OnUnregister()
{
	Super::OnUnregister();

	FOculusXRSceneEventDelegates::OculusSceneCaptureComplete.RemoveAll(this);
}

void UOculusXRRoomLayoutManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UOculusXRRoomLayoutManagerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

bool UOculusXRRoomLayoutManagerComponent::LaunchCaptureFlow()
{
	UE_LOG(LogOculusXRScene, Verbose, TEXT("Launch capture flow -- UOculusXRRoomLayoutManagerComponent"));

	uint64 OutRequest = 0;
	auto result = OculusXRScene::FOculusXRScene::RequestSceneCapture(OutRequest);
	bool isSuccess = UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(result);
	if (isSuccess)
	{
		EntityRequestList.Add(OutRequest);
	}

	UE_LOG(LogOculusXRScene, Verbose, TEXT("Launch capture flow -- RequestSceneCapture -- %d"), result);

	return isSuccess;
}

bool UOculusXRRoomLayoutManagerComponent::GetRoomLayout(FOculusXRUInt64 Space, FOculusXRRoomLayout& RoomLayoutOut, int32 MaxWallsCapacity)
{
	return UOculusXRSceneFunctionLibrary::GetRoomLayout(Space, RoomLayoutOut, MaxWallsCapacity);
}

bool UOculusXRRoomLayoutManagerComponent::LoadTriangleMesh(FOculusXRUInt64 Space, UProceduralMeshComponent* Mesh, bool CreateCollision) const
{
	ensure(Mesh);
	TArray<FVector> Vertices;
	TArray<int32> Triangles;

	auto result = OculusXRScene::FOculusXRScene::GetTriangleMesh(Space, Vertices, Triangles);
	bool isSuccess = UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(result);
	if (!isSuccess)
	{
		return false;
	}

	// Mesh->bUseAsyncCooking = true;
	TArray<FVector> EmptyNormals;
	TArray<FVector2D> EmptyUV;
	TArray<FColor> EmptyVertexColors;
	TArray<FProcMeshTangent> EmptyTangents;
	Mesh->CreateMeshSection(0, Vertices, Triangles, EmptyNormals, EmptyUV, EmptyVertexColors, EmptyTangents, CreateCollision);

	return true;
}
