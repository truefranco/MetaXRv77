// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "Generated/MRUtilityKitShared.h"
#include "MRUtilityKit.h"
#include "OculusXRAnchorTypes.h"

MRUKShared::MrukUuid ToMrukShared(const FOculusXRUUID& Uuid);

FOculusXRUUID ToOculusXR(const MRUKShared::MrukUuid& Uuid);

FTransform ToOculusXR(const MRUKShared::MrukPosef& Pose);

FBox3d ToOculusXR(const UWorld* World, const MRUKShared::MrukVolume& Volume);

FBox2d ToOculusXR(const UWorld* World, const MRUKShared::MrukPlane& Plane);

TArray<FVector2D> ToOculusXR(const UWorld* World, const FVector2f* const Boundary, uint32_t BoundaryCount);

FString ToOculusXR(MRUKShared::MrukLabel Label);

uint32_t ToMrukSharedSurfaceTypes(int32 ComponentTypes);

MRUKShared::MrukLabelFilter ToMrukShared(const FMRUKLabelFilter LabelFilter);

FVector FromOpenXrToUnreal(const FVector3f& V, const float WorldToMeters = 1.0f);

FVector3f FromUnrealToOpenXr(const FVector& V, const float WorldToMeters = 1.0f);
