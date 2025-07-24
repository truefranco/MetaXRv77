// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSceneXR.h"
#include "OpenXRCore.h"
#include "OpenXRHMD.h"
#include "IOpenXRHMDModule.h"
#include "OpenXR/OculusXROpenXRUtilities.h"
#include "OculusXRSceneModule.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRSceneDelegates.h"
#include "OculusXRAnchorsUtil.h"

#define LOCTEXT_NAMESPACE "OculusXRScene"

namespace XRScene
{
	PFN_xrGetSpaceBoundingBox2DFB xrGetSpaceBoundingBox2DFB = nullptr;
	PFN_xrGetSpaceBoundingBox3DFB xrGetSpaceBoundingBox3DFB = nullptr;
	PFN_xrGetSpaceBoundary2DFB xrGetSpaceBoundary2DFB = nullptr;
	PFN_xrGetSpaceSemanticLabelsFB xrGetSpaceSemanticLabelsFB = nullptr;
	PFN_xrRequestSceneCaptureFB xrRequestSceneCaptureFB = nullptr;
	PFN_xrGetSpaceRoomLayoutFB xrGetSpaceRoomLayoutFB = nullptr;
	PFN_xrGetSpaceTriangleMeshMETA xrGetSpaceTriangleMeshMETA = nullptr;
	PFN_xrRequestBoundaryVisibilityMETA xrRequestBoundaryVisibilityMETA = nullptr;

	FSceneXR::FSceneXR()
		: bExtSceneEnabled(false)
		, bExtSceneCaptureEnabled(false)
		, bExtBoundaryVisibilityEnabled(false)
		, bExtSpatialEntityMeshEnabled(false)
		, LastBoundaryVisibility(XR_BOUNDARY_VISIBILITY_MAX_ENUM_META)
		, OpenXRHMD(nullptr)
	{
	}

	FSceneXR::~FSceneXR()
	{
	}

	void FSceneXR::RegisterAsOpenXRExtension()
	{
		RegisterOpenXRExtensionModularFeature();
	}

	bool FSceneXR::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_FB_SCENE_EXTENSION_NAME);
		return true;
	}

	bool FSceneXR::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_FB_SCENE_CAPTURE_EXTENSION_NAME);
		OutExtensions.Add(XR_META_SPATIAL_ENTITY_MESH_EXTENSION_NAME);
		OutExtensions.Add(XR_META_BOUNDARY_VISIBILITY_EXTENSION_NAME);
		return true;
	}

	const void* FSceneXR::OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext)
	{
		if (InModule != nullptr)
		{
			bExtSceneEnabled = InModule->IsExtensionEnabled(XR_FB_SCENE_EXTENSION_NAME);
			bExtSceneCaptureEnabled = InModule->IsExtensionEnabled(XR_FB_SCENE_CAPTURE_EXTENSION_NAME);
			bExtBoundaryVisibilityEnabled = InModule->IsExtensionEnabled(XR_META_BOUNDARY_VISIBILITY_EXTENSION_NAME);
			bExtSpatialEntityMeshEnabled = InModule->IsExtensionEnabled(XR_META_SPATIAL_ENTITY_MESH_EXTENSION_NAME);

			UE_LOG(LogOculusXRScene, Log, TEXT("[SCENE] Extensions available"));
			UE_LOG(LogOculusXRScene, Log, TEXT("			Scene:         %hs"), bExtSceneEnabled ? "ENABLED" : "DISABLED");
			UE_LOG(LogOculusXRScene, Log, TEXT("			Scene Capture: %hs"), bExtSceneCaptureEnabled ? "ENABLED" : "DISABLED");
			UE_LOG(LogOculusXRScene, Log, TEXT("			Boundary:	   %hs"), bExtBoundaryVisibilityEnabled ? "ENABLED" : "DISABLED");
			UE_LOG(LogOculusXRScene, Log, TEXT("			Mesh:		   %hs"), bExtSpatialEntityMeshEnabled ? "ENABLED" : "DISABLED");
		}

		return InNext;
	}

	const void* FSceneXR::OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext)
	{
		InitOpenXRFunctions(InInstance);

		OpenXRHMD = (FOpenXRHMD*)GEngine->XRSystem.Get();

		return InNext;
	}

	void FSceneXR::OnDestroySession(XrSession InSession)
	{
		OpenXRHMD = nullptr;
	}

	void FSceneXR::OnEvent(XrSession InSession, const XrEventDataBaseHeader* InHeader)
	{
		if (OpenXRHMD == nullptr)
		{
			UE_LOG(LogOculusXRScene, Log, TEXT("[FSceneXR::OnEvent] Receieved event but no HMD was present."));
			return;
		}

		switch (InHeader->type)
		{
			case XR_TYPE_EVENT_DATA_BOUNDARY_VISIBILITY_CHANGED_META:
			{
				if (IsBoundaryVisibilityExtensionSupported())
				{
					const XrEventDataBoundaryVisibilityChangedMETA* const event =
						reinterpret_cast<const XrEventDataBoundaryVisibilityChangedMETA*>(InHeader);

					UE_LOG(LogOculusXRScene, Verbose, TEXT("[FSceneXR::OnEvent] XrEventDataBoundaryVisibilityChangedMETA"));
					UE_LOG(LogOculusXRScene, Verbose, TEXT("						Visibility: %hs"), (event->boundaryVisibility == XR_BOUNDARY_VISIBILITY_SUPPRESSED_META) ? "SUPPRESSED" : "NOT SUPPRESSED");

					FOculusXRSceneEventDelegates::OculusBoundaryVisibilityChanged.Broadcast(event->boundaryVisibility == XR_BOUNDARY_VISIBILITY_SUPPRESSED_META ? EOculusXRBoundaryVisibility::Suppressed : EOculusXRBoundaryVisibility::NotSuppressed);

					LastBoundaryVisibility = event->boundaryVisibility;
				}
				break;
			}
			case XR_TYPE_EVENT_DATA_SCENE_CAPTURE_COMPLETE_FB:
			{
				if (IsSceneCaptureExtensionSupported())
				{
					const XrEventDataSceneCaptureCompleteFB* const event =
						reinterpret_cast<const XrEventDataSceneCaptureCompleteFB*>(InHeader);

					UE_LOG(LogOculusXRScene, Verbose, TEXT("[FSceneXR::OnEvent] XrEventDataSceneCaptureCompleteFB"));
					UE_LOG(LogOculusXRScene, Verbose, TEXT("						Result: d"), event->result);

					FOculusXRSceneEventDelegates::OculusSceneCaptureComplete.Broadcast(event->result, XR_SUCCEEDED(event->result));
				}
				break;
			}
		}
	}

	XrResult FSceneXR::GetScenePlane(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetScenePlane] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsSceneExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetScenePlane] Scene extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrRect2Df rect;
		auto result = xrGetSpaceBoundingBox2DFB(OpenXRHMD->GetSession(), (XrSpace)AnchorHandle, &rect);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetScenePlane] Get space bounding box 2D failed. Result: %d"), result);
			return result;
		}

		// Convert to UE's coordinates system
		OutPos.X = 0;
		OutPos.Y = rect.offset.x;
		OutPos.Z = rect.offset.y;
		OutSize.X = 0;
		OutSize.Y = rect.extent.width;
		OutSize.Z = rect.extent.height;

		return result;
	}

	XrResult FSceneXR::GetSceneVolume(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetSceneVolume] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsSceneExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetSceneVolume] Scene extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrRect3DfFB rect;
		auto result = xrGetSpaceBoundingBox3DFB(OpenXRHMD->GetSession(), (XrSpace)AnchorHandle, &rect);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetSceneVolume] Get space bounding box 3D failed. Result: %d"), result);
			return result;
		}

		// Convert from OpenXR's right-handed to Unreal's left-handed coordinate system.
		//    OpenXR             Unreal
		//       | y            | z
		//       |              |
		// z <----+              +----> x
		//      /              /
		//    x/             y/
		//
		OutPos.X = -rect.offset.z;
		OutPos.Y = rect.offset.x;
		OutPos.Z = rect.offset.y;

		// The position represents the corner of the volume which has the lowest value
		// of each axis. Since we flipped the sign of one of the axes we need to adjust
		// the position to the other side of the volume
		OutPos.X -= rect.extent.depth;

		// We keep the size positive for all dimensions
		OutSize.X = rect.extent.depth;
		OutSize.Y = rect.extent.width;
		OutSize.Z = rect.extent.height;

		return result;
	}

	XrResult FSceneXR::GetBoundary2D(uint64 AnchorHandle, TArray<FVector2f>& OutVertices)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetBoundary2D] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsSceneExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetBoundary2D] Scene extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrBoundary2DFB boundary{ XR_TYPE_BOUNDARY_2D_FB, nullptr };
		boundary.vertexCapacityInput = 0;
		boundary.vertexCountOutput = 0;
		boundary.vertices = nullptr;

		auto getCountResult = xrGetSpaceBoundary2DFB(OpenXRHMD->GetSession(), (XrSpace)AnchorHandle, &boundary);
		if (XR_FAILED(getCountResult))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetBoundary2D] Get space boundary 2D vertex count failed. Result: %d"), getCountResult);
			return getCountResult;
		}

		TArray<XrVector2f> vertices;
		vertices.SetNum(boundary.vertexCountOutput);
		boundary.vertexCapacityInput = boundary.vertexCountOutput;
		boundary.vertices = vertices.GetData();

		auto getVerticesResult = xrGetSpaceBoundary2DFB(OpenXRHMD->GetSession(), (XrSpace)AnchorHandle, &boundary);
		if (XR_FAILED(getVerticesResult))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetBoundary2D] Get space boundary 2D vertices failed. Result: %d"), getVerticesResult);
			return getVerticesResult;
		}

		OutVertices.Reserve(vertices.Num());
		for (auto& it : vertices)
		{
			OutVertices.Add(FVector2f(it.x, it.y));
		}

		return getVerticesResult;
	}

	XrResult FSceneXR::GetSemanticClassification(uint64 AnchorHandle, TArray<FString>& OutSemanticClassifications)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetSemanticClassification] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsSceneExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetSemanticClassification] Scene extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		static const char* recognizedLabels = "DESK,COUCH,FLOOR,CEILING,WALL_FACE,WINDOW_FRAME,DOOR_FRAME,STORAGE,BED,SCREEN,LAMP,PLANT,OTHER,TABLE,WALL_ART,INVISIBLE_WALL_FACE,GLOBAL_MESH"
			;

		const XrSemanticLabelsSupportInfoFB semanticLabelsSupportInfo = {
			XR_TYPE_SEMANTIC_LABELS_SUPPORT_INFO_FB,
			nullptr,
			XR_SEMANTIC_LABELS_SUPPORT_ACCEPT_DESK_TO_TABLE_MIGRATION_BIT_FB | XR_SEMANTIC_LABELS_SUPPORT_ACCEPT_INVISIBLE_WALL_FACE_BIT_FB,
			recognizedLabels
		};

		XrSemanticLabelsFB xrLabels{ XR_TYPE_SEMANTIC_LABELS_FB, &semanticLabelsSupportInfo };
		xrLabels.bufferCountOutput = 0;
		xrLabels.bufferCapacityInput = 0;
		xrLabels.buffer = nullptr;

		XrResult result = xrGetSpaceSemanticLabelsFB(OpenXRHMD->GetSession(), (XrSpace)AnchorHandle, &xrLabels);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetSemanticClassification] Get semantic label buffer size failed. Result: %d"), result);
			return result;
		}

		TArray<char> buffer;
		buffer.SetNum(xrLabels.bufferCountOutput);
		xrLabels.bufferCapacityInput = xrLabels.bufferCountOutput;
		xrLabels.buffer = buffer.GetData();

		result = xrGetSpaceSemanticLabelsFB(OpenXRHMD->GetSession(), (XrSpace)AnchorHandle, &xrLabels);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetSemanticClassification] Get semantic label buffer failed. Result: %d"), result);
			return result;
		}

		FString labelsStr(xrLabels.bufferCountOutput, xrLabels.buffer);
		labelsStr.ParseIntoArray(OutSemanticClassifications, TEXT(","));

		return result;
	}

	XrResult FSceneXR::RequestSceneCapture(uint64& OutRequestID)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[RequestSceneCapture] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsSceneCaptureExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[RequestSceneCapture] Scene capture extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrSceneCaptureRequestInfoFB info{ XR_TYPE_SCENE_CAPTURE_REQUEST_INFO_FB, nullptr };
		info.request = nullptr;
		info.requestByteCount = 0;

		auto result = xrRequestSceneCaptureFB(OpenXRHMD->GetSession(), &info, (XrAsyncRequestIdFB*)&OutRequestID);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[RequestSceneCapture] Get scene capture failed. Result: %d"), result);
		}

		UE_LOG(LogOculusXRScene, Log, TEXT("[RequestSceneCapture] Started scene capture: RequestID (%llu)"), OutRequestID);

		return result;
	}

	XrResult FSceneXR::GetRoomLayout(uint64 AnchorHandle, const uint32 MaxWallsCapacity, FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetRoomLayout] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsSceneExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetRoomLayout] Scene extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrRoomLayoutFB roomLayout{ XR_TYPE_ROOM_LAYOUT_FB, nullptr };
		roomLayout.wallUuidCapacityInput = 0;
		roomLayout.wallUuidCountOutput = 0;
		roomLayout.wallUuids = nullptr;

		auto getWallsResult = xrGetSpaceRoomLayoutFB(OpenXRHMD->GetSession(), (XrSpace)AnchorHandle, &roomLayout);
		if (XR_FAILED(getWallsResult))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetRoomLayout] Failed to get wall count. Result: %d"), getWallsResult);
			return getWallsResult;
		}

		TArray<XrUuidEXT> wallUuids;
		wallUuids.SetNum(roomLayout.wallUuidCountOutput);
		roomLayout.wallUuidCapacityInput = roomLayout.wallUuidCountOutput;
		roomLayout.wallUuids = wallUuids.GetData();

		auto getDataResult = xrGetSpaceRoomLayoutFB(OpenXRHMD->GetSession(), (XrSpace)AnchorHandle, &roomLayout);
		if (XR_FAILED(getDataResult))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetRoomLayout] Failed to get room layout. Result: %d"), getDataResult);
			return getDataResult;
		}

		OutCeilingUuid = FOculusXRUUID(roomLayout.ceilingUuid.data);
		OutFloorUuid = FOculusXRUUID(roomLayout.floorUuid.data);
		for (auto& it : wallUuids)
		{
			OutWallsUuid.Add(it.data);
		}

		return getDataResult;
	}

	XrResult FSceneXR::GetTriangleMesh(uint64 AnchorHandle, TArray<FVector>& Vertices, TArray<int32>& Triangles)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetTriangleMesh] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsSpatialEntityMeshExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetTriangleMesh] Spatial entity mesh extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		const XrSpaceTriangleMeshGetInfoMETA xrGetInfo{ XR_TYPE_SPACE_TRIANGLE_MESH_GET_INFO_META };

		XrSpaceTriangleMeshMETA xrTriangleMesh{ XR_TYPE_SPACE_TRIANGLE_MESH_META, nullptr };
		xrTriangleMesh.indexCapacityInput = 0;
		xrTriangleMesh.indexCountOutput = 0;
		xrTriangleMesh.indices = nullptr;
		xrTriangleMesh.vertexCapacityInput = 0;
		xrTriangleMesh.vertexCountOutput = 0;
		xrTriangleMesh.vertices = nullptr;

		auto getMeshCountsResult = xrGetSpaceTriangleMeshMETA((XrSpace)AnchorHandle, &xrGetInfo, &xrTriangleMesh);
		if (XR_FAILED(getMeshCountsResult))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetTriangleMesh] Failed to get vertex and index count. Result: %d"), getMeshCountsResult);
			return getMeshCountsResult;
		}

		TArray<uint32_t> indices;
		indices.SetNum(xrTriangleMesh.indexCountOutput);
		xrTriangleMesh.indexCapacityInput = xrTriangleMesh.indexCountOutput;
		xrTriangleMesh.indices = indices.GetData();

		TArray<XrVector3f> vertices;
		vertices.SetNum(xrTriangleMesh.vertexCountOutput);
		xrTriangleMesh.vertexCapacityInput = xrTriangleMesh.vertexCountOutput;
		xrTriangleMesh.vertices = vertices.GetData();

		auto getMeshDataResult = xrGetSpaceTriangleMeshMETA((XrSpace)AnchorHandle, &xrGetInfo, &xrTriangleMesh);
		if (XR_FAILED(getMeshDataResult))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetTriangleMesh] Failed to get vertex and index data. Result: %d"), getMeshDataResult);
			return getMeshDataResult;
		}

		for (auto& it : indices)
		{
			Triangles.Add(it);
		}

		for (auto& it : vertices)
		{
			Vertices.Add(ToFVector(it));
		}

		return getMeshDataResult;
	}

	XrResult FSceneXR::RequestBoundaryVisibility(EOculusXRBoundaryVisibility NewVisibilityRequest)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[RequestBoundaryVisibility] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsBoundaryVisibilityExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[RequestBoundaryVisibility] Boundary visibility extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrSceneCaptureRequestInfoFB info{ XR_TYPE_SCENE_CAPTURE_REQUEST_INFO_FB, nullptr };
		info.request = nullptr;
		info.requestByteCount = 0;

		XrBoundaryVisibilityMETA visibility;
		switch (NewVisibilityRequest)
		{
			case EOculusXRBoundaryVisibility::NotSuppressed:
				visibility = XR_BOUNDARY_VISIBILITY_NOT_SUPPRESSED_META;
				break;
			case EOculusXRBoundaryVisibility::Suppressed:
				visibility = XR_BOUNDARY_VISIBILITY_SUPPRESSED_META;
				break;
			default:
				visibility = XR_BOUNDARY_VISIBILITY_MAX_ENUM_META;
		}

		auto result = xrRequestBoundaryVisibilityMETA(OpenXRHMD->GetSession(), visibility);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[RequestBoundaryVisibility] Get boundary visibility failed. Result: %d"), result);
		}

		return result;
	}

	XrResult FSceneXR::GetBoundaryVisibility(EOculusXRBoundaryVisibility& OutVisibility)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetBoundaryVisibility] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsBoundaryVisibilityExtensionSupported())
		{
			UE_LOG(LogOculusXRScene, Warning, TEXT("[GetBoundaryVisibility] Boundary visibility extension is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		OutVisibility = (LastBoundaryVisibility == XR_BOUNDARY_VISIBILITY_SUPPRESSED_META)
			? EOculusXRBoundaryVisibility::Suppressed
			: EOculusXRBoundaryVisibility::NotSuppressed;

		return XR_SUCCESS;
	}

	void FSceneXR::InitOpenXRFunctions(XrInstance InInstance)
	{
		// XR_FB_scene
		if (IsSceneExtensionSupported())
		{
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetSpaceBoundingBox2DFB", &xrGetSpaceBoundingBox2DFB);
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetSpaceBoundingBox3DFB", &xrGetSpaceBoundingBox3DFB);
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetSpaceBoundary2DFB", &xrGetSpaceBoundary2DFB);
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetSpaceSemanticLabelsFB", &xrGetSpaceSemanticLabelsFB);
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetSpaceRoomLayoutFB", &xrGetSpaceRoomLayoutFB);
		}

		// XR_FB_scene_capture
		if (IsSceneCaptureExtensionSupported())
		{
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrRequestSceneCaptureFB", &xrRequestSceneCaptureFB);
		}

		// XR_META_spatial_entity_mesh
		if (IsSpatialEntityMeshExtensionSupported())
		{
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetSpaceTriangleMeshMETA", &xrGetSpaceTriangleMeshMETA);
		}

		// XR_META_boundary_visibility
		if (IsBoundaryVisibilityExtensionSupported())
		{
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrRequestBoundaryVisibilityMETA", &xrRequestBoundaryVisibilityMETA);
		}
	}

} // namespace XRScene

#undef LOCTEXT_NAMESPACE
