// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAnchorTypes.h"
#include "OculusXRSceneTypes.h"

namespace OculusXRScene
{
	struct OCULUSXRSCENE_API IOculusXRSceneFunctions
	{
		virtual EOculusXRAnchorResult::Type GetScenePlane(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize) = 0;
		virtual EOculusXRAnchorResult::Type GetSceneVolume(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize) = 0;
		virtual EOculusXRAnchorResult::Type GetSemanticClassification(uint64 AnchorHandle, TArray<FString>& OutSemanticClassifications) = 0;
		virtual EOculusXRAnchorResult::Type GetBoundary2D(uint64 AnchorHandle, TArray<FVector2f>& OutVertices) = 0;

		virtual EOculusXRAnchorResult::Type RequestSceneCapture(uint64& OutRequestID) = 0;
		virtual EOculusXRAnchorResult::Type GetRoomLayout(uint64 AnchorHandle, const uint32 MaxWallsCapacity, FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid) = 0;
		virtual EOculusXRAnchorResult::Type GetTriangleMesh(uint64 AnchorHandle, TArray<FVector>& Vertices, TArray<int32>& Triangles) = 0;

		// Requests to change the current boundary visibility
		virtual EOculusXRAnchorResult::Type RequestBoundaryVisibility(EOculusXRBoundaryVisibility NewVisibilityRequest) = 0;

		// Gets the current boundary visibility
		virtual EOculusXRAnchorResult::Type GetBoundaryVisibility(EOculusXRBoundaryVisibility& OutVisibility) = 0;

	};
} // namespace OculusXRScene
