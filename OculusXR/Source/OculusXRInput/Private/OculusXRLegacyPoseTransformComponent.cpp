// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRLegacyPoseTransformComponent.h"
#include "OpenXR/OculusXROpenXRUtilities.h"
#include "GameFramework/WorldSettings.h"

UOculusXRLegacyPoseTransformComponent::UOculusXRLegacyPoseTransformComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetComponentTickEnabled(false);
}

void UOculusXRLegacyPoseTransformComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OculusXR::IsOpenXRSystem())
	{
		AddLocalTransform(FTransform(OculusPoseToGripRotation, OculusPoseToGripPosition * GetWorld()->GetWorldSettings()->WorldToMeters).Inverse());
	}
}
