// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "khronos/openxr/openxr.h"
#include "IStereoLayers.h"
#include "OculusXRPassthroughLayerShapes.h"
#include "OculusXRPassthroughMesh.h"

class UProceduralMeshComponent;

namespace XRPassthrough
{

#ifdef WITH_OCULUS_BRANCH
	using XrCompositionLayerBaseHeaderType = XrCompositionLayerBaseHeader;
#else
	// epic branch has member as const
	using XrCompositionLayerBaseHeaderType = XrCompositionLayerBaseHeader;
#endif

	class FPassthroughXR;

	class FPassthroughLayer
	{
	private:
		struct FPassthroughMesh
		{
			FPassthroughMesh(XrTriangleMeshFB MeshHandle, XrGeometryInstanceFB InstanceHandle, FTransform Transform)
				: MeshHandle(MeshHandle)
				, InstanceHandle(InstanceHandle)
				, LastTransform(Transform)
			{
			}
			XrTriangleMeshFB MeshHandle;
			XrGeometryInstanceFB InstanceHandle;
			FTransform LastTransform;
		};
		typedef TSharedPtr<TMap<FString, FPassthroughMesh>, ESPMode::ThreadSafe> FUserDefinedGeometryMapPtr;

		struct FPassthroughPokeActor
		{
			FPassthroughPokeActor(){};
			FPassthroughPokeActor(TWeakObjectPtr<UProceduralMeshComponent> PokeAHoleComponentPtr, TWeakObjectPtr<AActor> PokeAHoleActor)
				: PokeAHoleComponentPtr(PokeAHoleComponentPtr)
				, PokeAHoleActor(PokeAHoleActor){};
			TWeakObjectPtr<UProceduralMeshComponent> PokeAHoleComponentPtr;
			TWeakObjectPtr<AActor> PokeAHoleActor;
		};

		typedef TSharedPtr<TMap<FString, FPassthroughPokeActor>, ESPMode::ThreadSafe> FPassthroughPokeActorMapPtr;

	public:
		static bool IsPassthoughLayerDesc(const IStereoLayers::FLayerDesc& LayerDesc);
		FPassthroughLayer(XrPassthroughFB PassthroughInstance, TWeakPtr<FPassthroughXR> Extension);
		FPassthroughLayer(const FPassthroughLayer& Layer);
		TSharedPtr<FPassthroughLayer, ESPMode::ThreadSafe> Clone() const;
		virtual ~FPassthroughLayer();
		void SetDesc(const IStereoLayers::FLayerDesc& InLayerDesc);
		void DestroyLayer();
		void DestroyLayer_RenderThread();
		bool CanReuseResources(const FPassthroughLayer* InLayer) const;

		bool Initialize_RenderThread(XrSession InSession, const FPassthroughLayer* InLayer = nullptr);
		bool BuildPassthroughPokeActor(OculusXRHMD::FOculusPassthroughMeshRef PassthroughMesh, FPassthroughPokeActor& OutPassthroughPokeActor);
		void UpdatePassthroughPokeActors_GameThread();
		void UpdatePassthroughStyle_RenderThread(const FEdgeStyleParameters& EdgeStyleParameters);
		void UpdatePassthrough_RenderThread(FRHICommandListImmediate& RHICmdList, XrSpace Space, XrTime Time, float WorldToMetersScale, FTransform TrackingToWorld);

		XrCompositionLayerBaseHeaderType* GetXrCompositionLayerHeader();
		bool IsBackgroundLayer() const;
		bool IsOverlayLayer() const;
		bool PassthroughSupportsDepth() const;
		const IStereoLayers::FLayerDesc& GetDesc() const { return LayerDesc; };
		const XrPassthroughLayerFB GetLayerHandle() const { return XrPassthroughLayer; }

		void AddPassthroughMesh_RenderThread(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, FMatrix Transformation, XrSpace Space, XrTriangleMeshFB& OutMeshHandle, XrGeometryInstanceFB& OutInstanceHandle);
		void UpdatePassthroughMeshTransform_RenderThread(XrGeometryInstanceFB InstanceHandle, FMatrix Transformation, XrSpace Space, XrTime Time);
		void RemovePassthroughMesh_RenderThread(XrTriangleMeshFB MeshHandle, XrGeometryInstanceFB InstanceHandle);
		void ClearPassthroughPokeActors();

	private:
		TWeakPtr<FPassthroughXR> PassthroughExtension;

		FUserDefinedGeometryMapPtr UserDefinedGeometryMap;
		FPassthroughPokeActorMapPtr PassthroughPokeActorMap;

		XrSession Session;
		IStereoLayers::FLayerDesc LayerDesc;
		XrPassthroughLayerFB XrPassthroughLayer;
		XrCompositionLayerPassthroughFB XrCompositionLayerHeader;
		XrPassthroughFB XrPassthroughInstance;
	};

	typedef TSharedPtr<FPassthroughLayer, ESPMode::ThreadSafe> FPassthroughLayerPtr;

	struct FPassthroughLayerPtr_CompareId
	{
		FORCEINLINE bool operator()(const FPassthroughLayerPtr& A, const FPassthroughLayerPtr& B) const
		{
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
			return A->GetDesc().GetLayerId() < B->GetDesc().GetLayerId();
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
		}
	};

	struct FLayerDesc_ComparePriority
	{
		FORCEINLINE int32 GetLayerTypePriority(const IStereoLayers::FLayerDesc& LayerDesc) const
		{
			const bool IsPokeAHole = ((LayerDesc.Flags & IStereoLayers::LAYER_FLAG_SUPPORT_DEPTH) != 0) && LayerDesc.HasShape<FUserDefinedLayer>();
			bool IsUnderlay = false;

			if (LayerDesc.HasShape<FReconstructedLayer>())
			{
				const FReconstructedLayer& ReconstructedLayerProps = LayerDesc.GetShape<FReconstructedLayer>();
				IsUnderlay = (ReconstructedLayerProps.PassthroughLayerOrder == PassthroughLayerOrder_Underlay);
			}
			else if (LayerDesc.HasShape<FUserDefinedLayer>())
			{
				const FUserDefinedLayer& UserDefinedLayerProps = LayerDesc.GetShape<FUserDefinedLayer>();
				IsUnderlay = (UserDefinedLayerProps.PassthroughLayerOrder == PassthroughLayerOrder_Underlay);
			}

			const int32 Priority = IsUnderlay ? -2 : IsPokeAHole ? -1
																 : 1;
			return Priority;
		}

		FORCEINLINE bool operator()(const FPassthroughLayer& A, const FPassthroughLayer& B) const
		{
			const IStereoLayers::FLayerDesc DescA = A.GetDesc();
			const IStereoLayers::FLayerDesc DescB = B.GetDesc();

			// First order layers by type
			const int32 PassA = GetLayerTypePriority(DescA);
			const int32 PassB = GetLayerTypePriority(DescB);

			if (PassA != PassB)
			{
				return PassA < PassB;
			}

			// Draw layers by ascending priority
			if (DescA.Priority != DescB.Priority)
			{
				return DescA.Priority < DescB.Priority;
			}

			// Draw layers by ascending id
			return DescA.Id < DescB.Id;
		}
	};

} // namespace XRPassthrough
