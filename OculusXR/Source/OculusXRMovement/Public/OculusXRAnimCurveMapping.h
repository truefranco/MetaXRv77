// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimTypes.h"
#include "LiveLinkRetargetAsset.h"
#include "Containers/StaticArray.h"
#include "OculusXRMovementTypes.h"
#include "Misc/EngineVersionComparison.h"

#include "OculusXRAnimCurveMapping.generated.h"

USTRUCT(BlueprintType)
struct OCULUSXRMOVEMENT_API FOculusXRAnimCurveMapping
{
	GENERATED_BODY()

	FOculusXRAnimCurveMapping(){};

	FOculusXRAnimCurveMapping(const std::initializer_list<FName> CurveNamesList)
		: CurveNames(CurveNamesList)
	{
	}

	/**
	 * Skeleton's animation curve names
	 */
	UPROPERTY(EditAnywhere, Category = "OculusXR|Movement")
	TArray<FName> CurveNames;
};
