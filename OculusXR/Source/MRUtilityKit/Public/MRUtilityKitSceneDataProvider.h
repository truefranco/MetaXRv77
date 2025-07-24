// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MRUtilityKitSceneDataProvider.generated.h"

UCLASS(ClassGroup = MRUtilityKit, meta = (DisplayName = "MR Utility Kit Scene Data Provider"))
/*
 * This actor is used to provide scene data to the MR Utility Kit when running in editor.
 * You can also use it to not load a room from device.
 * Use RandomRoom to load a random room from the list of rooms.
 */
class MRUTILITYKIT_API AMRUKSceneDataProvider : public AActor
{
	GENERATED_BODY()

public:
	/*
	 * This list holds the rooms that can be loaded, the key is the room type and the value is a data table that contains multiple rooms.
	 * Roomtypes such as Bedrooms, Livingrooms, etc.
	 */
	UPROPERTY(EditAnywhere, Category = "MR Utility Kit")
	TMap<FString, UDataTable*> Rooms;

	/*
	 * When this is true, a random room will be loaded from the list of rooms.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MR Utility Kit")
	bool bUseRandomRoom = true;

	/*
	 * When this is true, a random room will be loaded a specific room class, defined in Rooms (Bedrooms, Offices, ..).
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MR Utility Kit", meta = (EditCondition = "!bUseRandomRoom", EditConditionHides))
	bool bUseRandomRoomFromClass = false;

	/*
	 * Use this property to define a specific room class to load, only visible when bUseRandomRoomFromClass is true.
	 * This can be a room class such as Bedrooms, Offices, ..
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MR Utility Kit", meta = (EditCondition = "bUseRandomRoomFromClass && !bUseRandomRoom", EditConditionHides))
	FString SpecificRoomClass;

	/*
	 * Define a specific room to load, only visible when bUseRandomRoom is false.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MR Utility Kit", meta = (EditCondition = "!bUseRandomRoom && !bUseRandomRoomFromClass", EditConditionHides))
	FString SpecificRoomName;

	/*
	 * Gets you a room from the list of rooms, if bUseRandomRoom is true, a random room will be returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "MR Utility Kit")
	void GetRoom(FString& RoomJSON, FString& RoomName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

USTRUCT(Blueprintable, BlueprintType)
struct FJSONData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MR Utility Kit")
	FString JSON;
};
