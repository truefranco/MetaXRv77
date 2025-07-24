// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRSceneXRIncludes.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRSceneTypes.h"
#include "OculusXRAnchorTypes.h"

#define LOCTEXT_NAMESPACE "OculusXRScene"

class FOpenXRHMD;

namespace XRScene
{
	extern PFN_xrGetSpaceBoundingBox2DFB xrGetSpaceBoundingBox2DFB;
	extern PFN_xrGetSpaceBoundingBox3DFB xrGetSpaceBoundingBox3DFB;
	extern PFN_xrGetSpaceBoundary2DFB xrGetSpaceBoundary2DFB;
	extern PFN_xrGetSpaceSemanticLabelsFB xrGetSpaceSemanticLabelsFB;
	extern PFN_xrRequestSceneCaptureFB xrRequestSceneCaptureFB;
	extern PFN_xrGetSpaceRoomLayoutFB xrGetSpaceRoomLayoutFB;
	extern PFN_xrGetSpaceTriangleMeshMETA xrGetSpaceTriangleMeshMETA;
	extern PFN_xrRequestBoundaryVisibilityMETA xrRequestBoundaryVisibilityMETA;

	class FSceneXR : public IOpenXRExtensionPlugin
	{
	public:
		// IOculusXROpenXRHMDPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual const void* OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext) override;
		virtual const void* OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext) override;
		virtual void OnDestroySession(XrSession InSession) override;
		virtual void OnEvent(XrSession InSession, const XrEventDataBaseHeader* InHeader) override;

	public:
		FSceneXR();
		virtual ~FSceneXR();
		void RegisterAsOpenXRExtension();

		bool IsSceneExtensionSupported() const { return bExtSceneEnabled; }
		bool IsSceneCaptureExtensionSupported() const { return bExtSceneCaptureEnabled; }
		bool IsBoundaryVisibilityExtensionSupported() const { return bExtBoundaryVisibilityEnabled; }
		bool IsSpatialEntityMeshExtensionSupported() const { return bExtSpatialEntityMeshEnabled; }

		XrResult GetScenePlane(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize);
		XrResult GetSceneVolume(uint64 AnchorHandle, FVector& OutPos, FVector& OutSize);
		XrResult GetBoundary2D(uint64 AnchorHandle, TArray<FVector2f>& OutVertices);
		XrResult GetSemanticClassification(uint64 AnchorHandle, TArray<FString>& OutSemanticClassifications);

		XrResult RequestSceneCapture(uint64& OutRequestID);
		XrResult GetRoomLayout(uint64 AnchorHandle, const uint32 MaxWallsCapacity, FOculusXRUUID& OutCeilingUuid, FOculusXRUUID& OutFloorUuid, TArray<FOculusXRUUID>& OutWallsUuid);
		XrResult GetTriangleMesh(uint64 AnchorHandle, TArray<FVector>& Vertices, TArray<int32>& Triangles);

		XrResult RequestBoundaryVisibility(EOculusXRBoundaryVisibility NewVisibilityRequest);
		XrResult GetBoundaryVisibility(EOculusXRBoundaryVisibility& OutVisibility);

	private:
		void InitOpenXRFunctions(XrInstance InInstance);

		bool bExtSceneEnabled;
		bool bExtSceneCaptureEnabled;
		bool bExtBoundaryVisibilityEnabled;
		bool bExtSpatialEntityMeshEnabled;

		XrBoundaryVisibilityMETA LastBoundaryVisibility;
		FOpenXRHMD* OpenXRHMD;
	};

} // namespace XRScene

#undef LOCTEXT_NAMESPACE
