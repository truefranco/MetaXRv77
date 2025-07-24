// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Components/SceneComponent.h"
#include "OculusXRLegacyPoseTransformComponent.h"
#include "OculusXRControllerLegacyPoseTransformComponent.generated.h"

/**
 * This class is deprecated, please use OculusXRLegacyPoseTransformComponent instead.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent, DeprecationMessage = "Please use OculusXRLegacyPoseTransformComponent instead."), ClassGroup = OculusHand, DisplayName = "[Deprecated] OculusXR Controller Legacy Pose Transform Component")
class OCULUSXRINPUT_API UOculusXRControllerLegacyPoseTransformComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

public:
	/**
	 * Applies the transformation from legacy Oculus pose to OpenXR grip pose onto the parent component.
	 */
	virtual void BeginPlay() override;
};
