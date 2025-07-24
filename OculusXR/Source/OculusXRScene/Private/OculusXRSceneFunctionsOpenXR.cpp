// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSceneFunctionsOpenXR.h"
#include "OculusXRSceneModule.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRHMD.h"
#include "OculusXRPluginWrapper.h"
#include "OculusXRAnchorsUtil.h"

#define LOCTEXT_NAMESPACE "OculusXRScene"

namespace OculusXRScene
{
	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::GetScenePlane(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->GetScenePlane(AnchorHandle, OutPos, OutSize);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::GetSceneVolume(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->GetSceneVolume(AnchorHandle, OutPos, OutSize);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::GetSemanticClassification(uint64 AnchorHandle, TArray<FString>& OutSemanticClassifications)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->GetSemanticClassification(AnchorHandle, OutSemanticClassifications);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::GetBoundary2D(uint64 AnchorHandle, TArray<FVector2f>& OutVertices)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->GetBoundary2D(AnchorHandle, OutVertices);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::RequestSceneCapture(uint64& OutRequestID)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->RequestSceneCapture(OutRequestID);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::GetRoomLayout(uint64 AnchorHandle, const uint32 MaxWallsCapacity, FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->GetRoomLayout(AnchorHandle, MaxWallsCapacity, OutCeilingUuid, OutFloorUuid, OutWallsUuid);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::GetTriangleMesh(uint64 AnchorHandle, TArray<FVector>& Vertices, TArray<int32>& Triangles)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->GetTriangleMesh(AnchorHandle, Vertices, Triangles);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

	// Requests to change the current boundary visibility
	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::RequestBoundaryVisibility(EOculusXRBoundaryVisibility NewVisibilityRequest)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->RequestBoundaryVisibility(NewVisibilityRequest);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOpenXR::GetBoundaryVisibility(EOculusXRBoundaryVisibility& OutVisibility)
	{
		auto result = FOculusXRSceneModule::Get().GetXrScene()->GetBoundaryVisibility(OutVisibility);
		return OculusXRAnchors::GetResultFromXrResult(result);
	}

} // namespace OculusXRScene

#undef LOCTEXT_NAMESPACE
