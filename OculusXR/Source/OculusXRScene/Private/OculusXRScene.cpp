// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRScene.h"
#include "OculusXRSceneModule.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRHMD.h"
#include "OculusXRPluginWrapper.h"
#include "OculusXRSceneFunctionsOVR.h"
#include "OculusXRSceneFunctionsOpenXR.h"

#define LOCTEXT_NAMESPACE "OculusXRScene"

namespace OculusXRScene
{
	EOculusXRAnchorResult::Type FOculusXRScene::GetScenePlane(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize)
	{
		return GetOculusXRSceneFunctionsImpl()->GetScenePlane(AnchorHandle, OutPos, OutSize);
	}

	EOculusXRAnchorResult::Type FOculusXRScene::GetSceneVolume(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize)
	{
		return GetOculusXRSceneFunctionsImpl()->GetSceneVolume(AnchorHandle, OutPos, OutSize);
	}

	EOculusXRAnchorResult::Type FOculusXRScene::GetSemanticClassification(uint64 AnchorHandle, TArray<FString>& OutSemanticClassifications)
	{
		return GetOculusXRSceneFunctionsImpl()->GetSemanticClassification(AnchorHandle, OutSemanticClassifications);
	}

	EOculusXRAnchorResult::Type FOculusXRScene::GetBoundary2D(uint64 AnchorHandle, TArray<FVector2f>& OutVertices)
	{
		return GetOculusXRSceneFunctionsImpl()->GetBoundary2D(AnchorHandle, OutVertices);
	}

	EOculusXRAnchorResult::Type FOculusXRScene::RequestSceneCapture(uint64& OutRequestID)
	{
		return GetOculusXRSceneFunctionsImpl()->RequestSceneCapture(OutRequestID);
	}

	EOculusXRAnchorResult::Type FOculusXRScene::GetRoomLayout(uint64 AnchorHandle, const uint32 MaxWallsCapacity, FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid)
	{
		return GetOculusXRSceneFunctionsImpl()->GetRoomLayout(AnchorHandle, MaxWallsCapacity, OutCeilingUuid, OutFloorUuid, OutWallsUuid);
	}

	EOculusXRAnchorResult::Type FOculusXRScene::GetTriangleMesh(uint64 AnchorHandle, TArray<FVector>& Vertices, TArray<int32>& Triangles)
	{
		return GetOculusXRSceneFunctionsImpl()->GetTriangleMesh(AnchorHandle, Vertices, Triangles);
	}

	// Requests to change the current boundary visibility
	EOculusXRAnchorResult::Type FOculusXRScene::RequestBoundaryVisibility(EOculusXRBoundaryVisibility NewVisibilityRequest)
	{
		return GetOculusXRSceneFunctionsImpl()->RequestBoundaryVisibility(NewVisibilityRequest);
	}

	EOculusXRAnchorResult::Type FOculusXRScene::GetBoundaryVisibility(EOculusXRBoundaryVisibility& OutVisibility)
	{
		return GetOculusXRSceneFunctionsImpl()->GetBoundaryVisibility(OutVisibility);
	}


	TSharedPtr<IOculusXRSceneFunctions> FOculusXRScene::SceneFunctionsImpl = nullptr;

	TSharedPtr<IOculusXRSceneFunctions> FOculusXRScene::GetOculusXRSceneFunctionsImpl()
	{
		if (SceneFunctionsImpl == nullptr)
		{
			const FName SystemName(TEXT("OpenXR"));
			const bool IsOpenXR = GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName);
			if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
			{
				SceneFunctionsImpl = MakeShared<FOculusXRSceneFunctionsOVR>();
			}
			else if (IsOpenXR)
			{
				SceneFunctionsImpl = MakeShared<FOculusXRSceneFunctionsOpenXR>();
			}
		}

		check(SceneFunctionsImpl);
		return SceneFunctionsImpl;
	}
} // namespace OculusXRScene

#undef LOCTEXT_NAMESPACE
