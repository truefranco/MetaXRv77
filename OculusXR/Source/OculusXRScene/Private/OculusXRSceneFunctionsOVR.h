// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRSceneFunctions.h"

namespace OculusXRScene
{
	struct OCULUSXRSCENE_API FOculusXRSceneFunctionsOVR : public IOculusXRSceneFunctions
	{
		virtual EOculusXRAnchorResult::Type GetScenePlane(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize) override;
		virtual EOculusXRAnchorResult::Type GetSceneVolume(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize) override;
		virtual EOculusXRAnchorResult::Type GetSemanticClassification(uint64 AnchorHandle, TArray<FString>& OutSemanticClassifications) override;
		virtual EOculusXRAnchorResult::Type GetBoundary2D(uint64 AnchorHandle, TArray<FVector2f>& OutVertices) override;

		virtual EOculusXRAnchorResult::Type RequestSceneCapture(uint64& OutRequestID) override;
		virtual EOculusXRAnchorResult::Type GetRoomLayout(uint64 AnchorHandle, const uint32 MaxWallsCapacity, FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid) override;
		virtual EOculusXRAnchorResult::Type GetTriangleMesh(uint64 AnchorHandle, TArray<FVector>& Vertices, TArray<int32>& Triangles) override;

		// Requests to change the current boundary visibility
		virtual EOculusXRAnchorResult::Type RequestBoundaryVisibility(EOculusXRBoundaryVisibility NewVisibilityRequest) override;

		// Gets the current boundary visibility
		virtual EOculusXRAnchorResult::Type GetBoundaryVisibility(EOculusXRBoundaryVisibility& OutVisibility) override;

	};
} // namespace OculusXRScene
