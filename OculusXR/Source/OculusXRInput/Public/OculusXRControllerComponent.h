// Copyright (c) Meta Platforms, Inc. and affiliates.

// A class to render the currently connected controller.
// Similar to how hands are tracked.

#pragma once
#include "OculusXRInputFunctionLibrary.h"
#include "OculusXRFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include <Engine/StreamableManager.h>

// Must always be the last include.
#include "OculusXRControllerComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent), ClassGroup = OculusHand)
class UOculusXRControllerComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UOculusXRControllerComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** The skeleton that will be loaded */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties")
	EOculusXRSide SkeletonType;

	/** Should this controller be rendered when using controller driven hand poses */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties")
	bool RenderWhenUsingControllerDrivenHands;

private:
	enum MeshLoadingState
	{
		None,
		Loading,
		Loaded
	};

	UStaticMesh* _runtimeMesh;
	MeshLoadingState _meshLoadingState;
	TSharedPtr<FStreamableHandle> _loadAssetHandle;
	FStreamableManager _streamableManager;
	EOculusXRControllerType _controllerType;
	FSoftObjectPath _runtimeMeshPath;
	EOculusXRControllerDrivenHandPoseTypes _cachedControllerHandType;

	void InitializeMesh();
	void MeshLoaded();
	EOculusXRControllerType GetControllerType();

	// These position and rotation offsets are needed to correctly position the controller
	// when using natural or controller based hand positioning.
	// Why do these need to be hardcoded and not come from the skeleton etc?
	// It seems like the offset comes from somewhere in unreal in the first place,
	// not from a bone position, so there's not a place to load the correct orientation from.
	const FVector PositionOffsets[EOculusXRSideCount][EOculusXRControllerDrivenHandPoseTypesCount]{
		{
			FVector(0, 0, 0), // Side:  None, Controller Mapping: None
			FVector(0, 0, 0), // Side:  None, Controller Mapping: Natural
			FVector(0, 0, 0), // Side:  None, Controller Mapping: Controller
		},
		{
			FVector(0, 0, 0),			  // Side:  Left, Controller Mapping: None
			FVector(4.278, 9.969, 4.638), // Side:  Left, Controller Mapping: Natural
			FVector(4.278, 9.969, 4.638), // Side:  Left, Controller Mapping: Controller
		},
		{
			FVector(0, 0, 0),				 // Side: Right, Controller Mapping: None
			FVector(-4.104, -9.993, -4.244), // Side: Right, Controller Mapping: Natural
			FVector(-4.104, -9.993, -4.244), // Side: Right, Controller Mapping: Controller
		},
	};
	const FVector RotationOffsets[EOculusXRSideCount][EOculusXRControllerDrivenHandPoseTypesCount]{
		{
			FVector(0, 0, 0), // Side:  None, Controller Mapping: None
			FVector(0, 0, 0), // Side:  None, Controller Mapping: Natural
			FVector(0, 0, 0), // Side:  None, Controller Mapping: Controller
		},
		{
			FVector(0, 0, 0),			   // Side:  Left, Controller Mapping: None
			FVector(90, 166.229, 263.738), // Side:  Left, Controller Mapping: Natural
			FVector(90, 168.515, 259.149), // Side:  Left, Controller Mapping: Controller
		},
		{
			FVector(0, 0, 0),			  // Side: Right, Controller Mapping: None
			FVector(90, 194.995, 83.863), // Side: Right, Controller Mapping: Natural
			FVector(90, 191.485, 79.149), // Side: Right, Controller Mapping: Controller
		},
	};
};
