// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRSceneFunctions.h"
#include "OculusXRAnchorTypes.h"
#include "OculusXRSceneTypes.h"

namespace OculusXRScene
{
	struct OCULUSXRSCENE_API FOculusXRScene
	{
		static EOculusXRAnchorResult::Type GetScenePlane(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize);
		static EOculusXRAnchorResult::Type GetSceneVolume(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize);
		static EOculusXRAnchorResult::Type GetSemanticClassification(uint64 AnchorHandle, TArray<FString>& OutSemanticClassifications);
		static EOculusXRAnchorResult::Type GetBoundary2D(uint64 AnchorHandle, TArray<FVector2f>& OutVertices);

		static EOculusXRAnchorResult::Type RequestSceneCapture(uint64& OutRequestID);
		static EOculusXRAnchorResult::Type GetRoomLayout(uint64 AnchorHandle, const uint32 MaxWallsCapacity, FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid);
		static EOculusXRAnchorResult::Type GetTriangleMesh(uint64 AnchorHandle, TArray<FVector>& Vertices, TArray<int32>& Triangles);

		// Requests to change the current boundary visibility
		static EOculusXRAnchorResult::Type RequestBoundaryVisibility(EOculusXRBoundaryVisibility NewVisibilityRequest);

		// Gets the current boundary visibility
		static EOculusXRAnchorResult::Type GetBoundaryVisibility(EOculusXRBoundaryVisibility& OutVisibility);

	private:
		static TSharedPtr<IOculusXRSceneFunctions> GetOculusXRSceneFunctionsImpl();
		static TSharedPtr<IOculusXRSceneFunctions> SceneFunctionsImpl;
	};
} // namespace OculusXRScene
