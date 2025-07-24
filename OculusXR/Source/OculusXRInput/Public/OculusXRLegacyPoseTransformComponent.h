// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Components/SceneComponent.h"
#include "OculusXRLegacyPoseTransformComponent.generated.h"

static const FQuat OculusPoseToGripRotation = FQuat(FVector(0, 1, 0), -FMath::DegreesToRadians(double(60)));
static const FVector OculusPoseToGripPosition = FVector(-0.04, 0, -0.03);

/**
 * Handles conversion of components created for the legacy Oculus controller pose into
 * the OpenXR Grip pose. Attach components that need to be transformed under this component.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent), ClassGroup = OculusHand, DisplayName = "OculusXR Legacy Pose Transform Component")
class OCULUSXRINPUT_API UOculusXRLegacyPoseTransformComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * Applies the transformation from legacy Oculus pose to OpenXR grip pose onto the parent component.
	 */
	virtual void BeginPlay() override;
};
