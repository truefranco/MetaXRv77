// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSceneFunctionLibrary.h"

#include "Engine/GameInstance.h"
#include "OculusXRAnchorBPFunctionLibrary.h"
#include "OculusXRScene.h"
#include "OculusXRSceneModule.h"
#include "OculusXRSceneSubsystem.h"
#include "OculusXRAnchors.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRHMD.h"

bool UOculusXRSceneFunctionLibrary::GetBoundaryVisibility(const UObject* WorldContext, EOculusXRBoundaryVisibility& OutVisibility)
{
	OutVisibility = EOculusXRBoundaryVisibility::NotSuppressed;

	check(WorldContext);
	check(WorldContext->GetWorld());

	UOculusXRSceneSubsystem* subsystem = WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UOculusXRSceneSubsystem>();
	if (subsystem == nullptr)
	{
		return false;
	}

	OutVisibility = subsystem->GetBoundaryVisibility();
	return true;
}

bool UOculusXRSceneFunctionLibrary::GetRequestedBoundaryVisibility(const UObject* WorldContext, EOculusXRBoundaryVisibility& OutVisibility)
{
	OutVisibility = EOculusXRBoundaryVisibility::NotSuppressed;

	check(WorldContext);
	check(WorldContext->GetWorld());

	UOculusXRSceneSubsystem* subsystem = WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UOculusXRSceneSubsystem>();
	if (subsystem == nullptr)
	{
		return false;
	}

	OutVisibility = subsystem->GetRequestedBoundaryVisibility();
	return true;
}

bool UOculusXRSceneFunctionLibrary::RequestBoundaryVisibility(const UObject* WorldContext, EOculusXRBoundaryVisibility Visibility)
{
	check(WorldContext);
	check(WorldContext->GetWorld());

	UOculusXRSceneSubsystem* subsystem = WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UOculusXRSceneSubsystem>();
	if (subsystem == nullptr)
	{
		return false;
	}

	subsystem->SetRequestedBoundaryVisibility(Visibility);

	return true;
}

bool UOculusXRSceneFunctionLibrary::GetRoomLayout(FOculusXRUInt64 Space, FOculusXRRoomLayout& RoomLayoutOut, int32 MaxWallsCapacity)
{
	if (MaxWallsCapacity <= 0)
	{
		return false;
	}

	FOculusXRUUID OutCeilingUuid;
	FOculusXRUUID OutFloorUuid;
	TArray<FOculusXRUUID> OutWallsUuid;

	auto result = OculusXRScene::FOculusXRScene::GetRoomLayout(Space.Value, static_cast<uint32>(MaxWallsCapacity), OutCeilingUuid, OutFloorUuid, OutWallsUuid);
	auto bSuccess = UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(result);

	if (bSuccess)
	{
		RoomLayoutOut.CeilingUuid = OutCeilingUuid;
		RoomLayoutOut.FloorUuid = OutFloorUuid;
		RoomLayoutOut.WallsUuid.InsertZeroed(0, OutWallsUuid.Num());

		for (int32 i = 0; i < OutWallsUuid.Num(); ++i)
		{
			RoomLayoutOut.WallsUuid[i] = OutWallsUuid[i];
		}

		TArray<FOculusXRUUID> spaceUUIDs;
		EOculusXRAnchorResult::Type getContainerUUIDsResult;
		OculusXRAnchors::FOculusXRAnchors::GetSpaceContainerUUIDs(Space, spaceUUIDs, getContainerUUIDsResult);

		if (UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(getContainerUUIDsResult))
		{
			RoomLayoutOut.RoomObjectUUIDs = spaceUUIDs;
		}
	}

	return bSuccess;
}

