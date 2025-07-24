// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "MRUtilityKitSceneDataProvider.h"
#include "UObject/ConstructorHelpers.h"
#include "MRUtilityKit.h"

void AMRUKSceneDataProvider::GetRoom(FString& RoomJSON, FString& RoomName)
{
	if (!bUseRandomRoom)
	{
		if (!SpecificRoomName.IsEmpty())
		{
			for (const auto& Room : Rooms)
			{
				const auto RoomDT = Room.Value;
				const auto TmpJSON = RoomDT->FindRow<FJSONData>(FName(SpecificRoomName), "", false);
				if (TmpJSON != nullptr)
				{
					RoomJSON = TmpJSON->JSON;
					RoomName = SpecificRoomName;
					return;
				}
			}
			UE_LOG(LogMRUK, Warning, TEXT("Specific room name not found, using random room."));
		}
		else
		{
			UE_LOG(LogMRUK, Warning, TEXT("Specific room name not defined, using random room."));
		}
	}

	if (bUseRandomRoomFromClass)
	{
		if (!SpecificRoomClass.IsEmpty())
		{
			const auto RoomDT = *Rooms.Find(SpecificRoomClass);
			if (RoomDT != nullptr)
			{
				TArray<FJSONData*> TmpArray;
				RoomDT->GetAllRows("", TmpArray);
				auto TmpRowNames = RoomDT->GetRowNames();
				const auto Num = TmpArray.Num() - 1;
				const auto Idx = FMath::RandRange(0, Num);

				RoomJSON = TmpArray[Idx]->JSON;
				RoomName = TmpRowNames[Idx].ToString();
				return;
			}

			UE_LOG(LogMRUK, Warning, TEXT("Specific room class not found, using random room."));
		}
		else
		{
			UE_LOG(LogMRUK, Warning, TEXT("Specific room class not defined, using random room."));
		}
	}

	auto Num = Rooms.Num() - 1;
	auto Idx = FMath::RandRange(0, Num);

	TArray<UDataTable*> ChildArray;
	Rooms.GenerateValueArray(ChildArray);

	const auto Room = ChildArray[Idx];

	Num = Room->GetRowMap().Num() - 1;
	Idx = FMath::RandRange(0, Num);

	TArray<FJSONData*> RandomRoomRows;
	auto RandomRoomRowNames = Room->GetRowNames();
	Room->GetAllRows("", RandomRoomRows);

	RoomJSON = RandomRoomRows[Idx]->JSON;
	RoomName = RandomRoomRowNames[Idx].ToString();
}

// Called when the game starts or when spawned
void AMRUKSceneDataProvider::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMRUKSceneDataProvider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
