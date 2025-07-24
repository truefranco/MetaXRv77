// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSceneFunctionsOVR.h"
#include "OculusXRSceneModule.h"
#include "OculusXRAnchorsUtil.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRHMD.h"
#include "OculusXRPluginWrapper.h"

#define LOCTEXT_NAMESPACE "OculusXRScene"

namespace OculusXRScene
{
	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::GetScenePlane(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize)
	{
		OutPos.X = OutPos.Y = OutPos.Z = 0.f;
		OutSize.X = OutSize.Y = OutSize.Z = 0.f;

		ovrpRectf rect;
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceBoundingBox2D(&AnchorHandle, &rect);

		if (OVRP_SUCCESS(Result))
		{
			// Convert to UE's coordinates system
			OutPos.Y = rect.Pos.x;
			OutPos.Z = rect.Pos.y;
			OutSize.Y = rect.Size.w;
			OutSize.Z = rect.Size.h;
		}

		return OculusXRAnchors::GetResultFromOVRResult(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::GetSceneVolume(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize)
	{
		OutPos.X = OutPos.Y = OutPos.Z = 0.f;
		OutSize.X = OutSize.Y = OutSize.Z = 0.f;

		ovrpBoundsf bounds;
		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceBoundingBox3D(&AnchorHandle, &bounds);

		if (OVRP_SUCCESS(Result))
		{
			// Convert from OpenXR's right-handed to Unreal's left-handed coordinate system.
			//    OpenXR             Unreal
			//       | y            | z
			//       |              |
			// z <----+              +----> x
			//      /              /
			//    x/             y/
			//
			OutPos.X = -bounds.Pos.z;
			OutPos.Y = bounds.Pos.x;
			OutPos.Z = bounds.Pos.y;

			// The position represents the corner of the volume which has the lowest value
			// of each axis. Since we flipped the sign of one of the axes we need to adjust
			// the position to the other side of the volume
			OutPos.X -= bounds.Size.d;

			// We keep the size positive for all dimensions
			OutSize.X = bounds.Size.d;
			OutSize.Y = bounds.Size.w;
			OutSize.Z = bounds.Size.h;
		}

		return OculusXRAnchors::GetResultFromOVRResult(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::GetSemanticClassification(uint64 AnchorHandle, TArray<FString>& OutSemanticClassifications)
	{
		OutSemanticClassifications.Empty();

		const int32 maxByteSize = 1024;
		char labelsChars[maxByteSize];

		ovrpSemanticLabels labels;
		labels.byteCapacityInput = maxByteSize;
		labels.labels = labelsChars;

		const ovrpResult Result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceSemanticLabels(&AnchorHandle, &labels);

		if (OVRP_SUCCESS(Result))
		{
			FString labelsStr(labels.byteCountOutput, labels.labels);
			labelsStr.ParseIntoArray(OutSemanticClassifications, TEXT(","));
		}

		return OculusXRAnchors::GetResultFromOVRResult(Result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::GetBoundary2D(uint64 AnchorHandle, TArray<FVector2f>& OutVertices)
	{
		TArray<ovrpVector2f> vertices;

		// Get the number of elements in the container
		ovrpBoundary2D boundary;
		boundary.vertexCapacityInput = 0;
		boundary.vertexCountOutput = 0;
		boundary.vertices = nullptr;

		ovrpResult result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceBoundary2D(&AnchorHandle, &boundary);
		if (OVRP_FAILURE(result))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("Failed to get space boundary 2d %d"), result);
			return OculusXRAnchors::GetResultFromOVRResult(result);
		}

		// Retrieve the actual array of vertices
		vertices.SetNum(boundary.vertexCountOutput);
		boundary.vertexCapacityInput = boundary.vertexCountOutput;
		boundary.vertices = vertices.GetData();

		result = FOculusXRHMDModule::GetPluginWrapper().GetSpaceBoundary2D(&AnchorHandle, &boundary);
		if (OVRP_FAILURE(result))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("Failed to get space boundary 2d %d"), result);
			return OculusXRAnchors::GetResultFromOVRResult(result);
		}

		// Write out the vertices
		OutVertices.Reserve(vertices.Num());
		for (const auto& it : vertices)
		{
			OutVertices.Add(FVector2f(it.x, it.y));
		}

		return EOculusXRAnchorResult::Success;
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::RequestSceneCapture(uint64& OutRequestID)
	{
		OutRequestID = 0;

		ovrpSceneCaptureRequest sceneCaptureRequest;
		sceneCaptureRequest.request = nullptr;
		sceneCaptureRequest.requestByteCount = 0;

		ovrpResult result = FOculusXRHMDModule::GetPluginWrapper().RequestSceneCapture(&sceneCaptureRequest, &OutRequestID);

		return OculusXRAnchors::GetResultFromOVRResult(result);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::GetRoomLayout(uint64 AnchorHandle, const uint32 MaxWallsCapacity, FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid)
	{
		ovrpRoomLayout roomLayout;
		roomLayout.wallUuidCapacityInput = 0;
		roomLayout.wallUuidCountOutput = 0;

		// First call to get output size
		ovrpResult firstCallResult = FOculusXRHMDModule::GetPluginWrapper().GetSpaceRoomLayout(&AnchorHandle, &roomLayout);
		if (OVRP_FAILURE(firstCallResult))
		{
			return OculusXRAnchors::GetResultFromOVRResult(firstCallResult);
		}

		// Set the input size and pointer to the uuid array
		TArray<ovrpUuid> uuids;
		uuids.InsertZeroed(0, roomLayout.wallUuidCountOutput);

		roomLayout.wallUuidCapacityInput = roomLayout.wallUuidCountOutput;
		roomLayout.wallUuids = uuids.GetData();

		ovrpResult secondCallResult = FOculusXRHMDModule::GetPluginWrapper().GetSpaceRoomLayout(&AnchorHandle, &roomLayout);
		if (OVRP_FAILURE(secondCallResult))
		{
			return OculusXRAnchors::GetResultFromOVRResult(secondCallResult);
		}

		OutCeilingUuid = FOculusXRUUID(roomLayout.ceilingUuid.data);
		OutFloorUuid = FOculusXRUUID(roomLayout.floorUuid.data);

		OutWallsUuid.Empty();
		OutWallsUuid.InsertZeroed(0, uuids.Num());

		for (int32 i = 0; i < uuids.Num(); ++i)
		{
			OutWallsUuid[i] = FOculusXRUUID(roomLayout.wallUuids[i].data);
		}

		return OculusXRAnchors::GetResultFromOVRResult(secondCallResult);
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::GetTriangleMesh(uint64 AnchorHandle, TArray<FVector>& Vertices, TArray<int32>& Triangles)
	{
		ovrpTriangleMesh OVRPMesh = { 0, 0, nullptr, 0, 0, nullptr };

		ovrpResult countResult = FOculusXRHMDModule::GetPluginWrapper().GetSpaceTriangleMesh(&AnchorHandle, &OVRPMesh);
		if (OVRP_FAILURE(countResult))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("Failed to load TriangleMesh info - Space: %llu - Result: %d"), AnchorHandle, countResult);
			return OculusXRAnchors::GetResultFromOVRResult(countResult);
		}
		OVRPMesh.indexCapacityInput = OVRPMesh.indexCountOutput;
		OVRPMesh.vertexCapacityInput = OVRPMesh.vertexCountOutput;

		TArray<ovrpVector3f> OVRPVertices;
		OVRPVertices.SetNum(OVRPMesh.vertexCapacityInput);
		OVRPMesh.vertices = OVRPVertices.GetData();
		Triangles.SetNum(OVRPMesh.indexCapacityInput);
		check(sizeof(TRemoveReference<decltype(Triangles)>::Type::ElementType) == sizeof(TRemovePointer<decltype(OVRPMesh.indices)>::Type));
		OVRPMesh.indices = Triangles.GetData();

		const ovrpResult meshResult = FOculusXRHMDModule::GetPluginWrapper().GetSpaceTriangleMesh(&AnchorHandle, &OVRPMesh);
		if (OVRP_FAILURE(meshResult))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("Failed to load TriangleMesh data - AnchorHandle: %llu - Result: %d"), AnchorHandle, meshResult);
			return OculusXRAnchors::GetResultFromOVRResult(meshResult);
		}

		UE_LOG(LogOculusXRScene, Verbose, TEXT("Loaded TriangleMesh data - AnchorHandle: %llu - Vertices: %d - Faces: %d"),
			AnchorHandle, OVRPMesh.vertexCapacityInput, OVRPMesh.indexCapacityInput);

		Vertices.Empty(OVRPVertices.Num());
		Algo::Transform(OVRPVertices, Vertices, [](const auto& Vertex) { return OculusXRHMD::ToFVector(Vertex); });

		return OculusXRAnchors::GetResultFromOVRResult(meshResult);
	}

	// Requests to change the current boundary visibility
	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::RequestBoundaryVisibility(EOculusXRBoundaryVisibility NewVisibilityRequest)
	{
		ovrpBoundaryVisibility visibility = ovrpBoundaryVisibility_NotSuppressed;

		switch (NewVisibilityRequest)
		{
			case EOculusXRBoundaryVisibility::Suppressed:
				visibility = ovrpBoundaryVisibility_Suppressed;
				break;
			case EOculusXRBoundaryVisibility::NotSuppressed:
				visibility = ovrpBoundaryVisibility_NotSuppressed;
				break;
			default:
				UE_LOG(LogOculusXRScene, Error, TEXT("RequestBoundaryVisibility -- Unknown boundary visibility value! (%d)"), static_cast<int32>(NewVisibilityRequest));
				return EOculusXRAnchorResult::Failure_InvalidParameter;
		}

		UE_LOG(LogOculusXRScene, Log, TEXT("RequestBoundaryVisibility -- New Visibility Requested (%s)"), *UEnum::GetValueAsString(NewVisibilityRequest));

		auto result = FOculusXRHMDModule::GetPluginWrapper().RequestBoundaryVisibility(visibility);
		auto castedResult = OculusXRAnchors::GetResultFromOVRResult(result);

		if (!OVRP_SUCCESS(result))
		{
			UE_LOG(LogOculusXRScene, Error, TEXT("RequestBoundaryVisibility failed -- Result(%s)"), *UEnum::GetValueAsString(castedResult));
		}

		return castedResult;
	}

	EOculusXRAnchorResult::Type FOculusXRSceneFunctionsOVR::GetBoundaryVisibility(EOculusXRBoundaryVisibility& OutVisibility)
	{
		ovrpBoundaryVisibility visibility = {};
		auto result = FOculusXRHMDModule::GetPluginWrapper().GetBoundaryVisibility(&visibility);
		auto castedResult = OculusXRAnchors::GetResultFromOVRResult(result);

		if (OVRP_SUCCESS(result))
		{
			switch (visibility)
			{
				case ovrpBoundaryVisibility_Suppressed:
					OutVisibility = EOculusXRBoundaryVisibility::Suppressed;
					break;
				case ovrpBoundaryVisibility_NotSuppressed:
					OutVisibility = EOculusXRBoundaryVisibility::NotSuppressed;
					break;
				default:
					OutVisibility = EOculusXRBoundaryVisibility::Invalid;
					UE_LOG(LogOculusXRScene, Error, TEXT("GetBoundaryVisibility -- Unknown boundary visibility value! Value(%d)"), visibility);
					break;
			}
		}
		else
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("GetBoundaryVisibility -- Failed to get boundary visibility. Result(%s)"), *UEnum::GetValueAsString(castedResult));
		}

		return castedResult;
	}

} // namespace OculusXRScene

#undef LOCTEXT_NAMESPACE
