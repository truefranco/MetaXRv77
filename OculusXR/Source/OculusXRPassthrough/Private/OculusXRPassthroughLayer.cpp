// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRPassthroughLayer.h"

#include "Components/MeshComponent.h"
#include "Engine/Engine.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRPassthroughXRFunctions.h"
#include "OculusXRPassthroughXR.h"
#include "OculusXRPassthroughModule.h"
#include "ProceduralMeshComponent.h"
#include "XRThreadUtils.h"

namespace XRPassthrough
{
	static UWorld* GetWorld()
	{
		UWorld* World = nullptr;
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::Game || Context.WorldType == EWorldType::PIE)
			{
				World = Context.World();
			}
		}
		return World;
	}

	FPassthroughLayer::FPassthroughLayer(XrPassthroughFB PassthroughInstance, TWeakPtr<FPassthroughXR> Extension)
		: PassthroughExtension(Extension)
		, UserDefinedGeometryMap(nullptr)
		, PassthroughPokeActorMap(nullptr)
		, XrPassthroughLayer{ XR_NULL_HANDLE }
		, XrCompositionLayerHeader{}
		, XrPassthroughInstance(PassthroughInstance)
	{
	}

	FPassthroughLayer::FPassthroughLayer(const FPassthroughLayer& Layer)
		: PassthroughExtension(Layer.PassthroughExtension)
		, UserDefinedGeometryMap(Layer.UserDefinedGeometryMap)
		, PassthroughPokeActorMap(Layer.PassthroughPokeActorMap)
		, Session(Layer.Session)
		, LayerDesc(Layer.LayerDesc)
		, XrPassthroughLayer(Layer.XrPassthroughLayer)
		, XrCompositionLayerHeader(Layer.XrCompositionLayerHeader)
		, XrPassthroughInstance(Layer.XrPassthroughInstance)
	{
	}

	TSharedPtr<FPassthroughLayer, ESPMode::ThreadSafe> FPassthroughLayer::Clone() const
	{
		return MakeShareable(new FPassthroughLayer(*this));
	}

	FPassthroughLayer::~FPassthroughLayer()
	{
	}

	void FPassthroughLayer::SetDesc(const IStereoLayers::FLayerDesc& InLayerDesc)
	{
		LayerDesc = InLayerDesc;

		if (!PassthroughPokeActorMap)
		{
			PassthroughPokeActorMap = MakeShared<TMap<FString, FPassthroughPokeActor>, ESPMode::ThreadSafe>();
		}

		UpdatePassthroughPokeActors_GameThread();
	}

	void FPassthroughLayer::DestroyLayer()
	{
		OculusXRHMD::CheckInGameThread();

		ClearPassthroughPokeActors();
	}

	void FPassthroughLayer::DestroyLayer_RenderThread()
	{
		OculusXRHMD::CheckInRenderThread();

		// Clear user defined meshes
		for (auto& Entry : *UserDefinedGeometryMap)
		{
			const XrTriangleMeshFB MeshHandle = Entry.Value.MeshHandle;
			const XrGeometryInstanceFB InstanceHandle = Entry.Value.InstanceHandle;
			RemovePassthroughMesh_RenderThread(MeshHandle, InstanceHandle);
		}
		UserDefinedGeometryMap->Empty();

		// Destroy passthrough layer
		if (XrPassthroughLayer != XR_NULL_HANDLE)
		{
			xrDestroyPassthroughLayerFB(XrPassthroughLayer);
		}
		else
		{
			UE_LOG(LogOculusXRPassthrough, Warning, TEXT("Failed to destroy layer as handle was null"));
		}
	}

	bool FPassthroughLayer::IsPassthoughLayerDesc(const IStereoLayers::FLayerDesc& LayerDesc)
	{
		return LayerDesc.HasShape<FReconstructedLayer>() || LayerDesc.HasShape<FUserDefinedLayer>();
	}

	bool FPassthroughLayer::CanReuseResources(const FPassthroughLayer* InLayer) const
	{
		if (!InLayer)
		{
			return false;
		}

		if (!IsPassthoughLayerDesc(InLayer->LayerDesc) || InLayer->LayerDesc.HasShape<FReconstructedLayer>() != LayerDesc.HasShape<FReconstructedLayer>() || InLayer->LayerDesc.HasShape<FUserDefinedLayer>() != LayerDesc.HasShape<FUserDefinedLayer>())
		{
			return false;
		}

		return true;
	}

	bool FPassthroughLayer::Initialize_RenderThread(XrSession InSession, const FPassthroughLayer* InLayer)
	{
		OculusXRHMD::CheckInRenderThread();

		if (!CanReuseResources(InLayer))
		{
			Session = InSession;

			if (XrPassthroughLayer != XR_NULL_HANDLE)
			{
				xrDestroyPassthroughLayerFB(XrPassthroughLayer);
				XrPassthroughLayer = XR_NULL_HANDLE;
			}

			if (LayerDesc.HasShape<FReconstructedLayer>() || LayerDesc.HasShape<FUserDefinedLayer>())
			{
				XrPassthroughLayerCreateInfoFB PassthroughLayerCreateInfo = { XR_TYPE_PASSTHROUGH_LAYER_CREATE_INFO_FB };
				PassthroughLayerCreateInfo.passthrough = XrPassthroughInstance;
				PassthroughLayerCreateInfo.purpose = LayerDesc.HasShape<FReconstructedLayer>() ? XR_PASSTHROUGH_LAYER_PURPOSE_RECONSTRUCTION_FB : XR_PASSTHROUGH_LAYER_PURPOSE_PROJECTED_FB;

				XrResult CreateLayerResult = xrCreatePassthroughLayerFB(Session, &PassthroughLayerCreateInfo, &XrPassthroughLayer);
				if (!XR_SUCCEEDED(CreateLayerResult))
				{
					UE_LOG(LogOculusXRPassthrough, Warning, TEXT("Failed to create passthrough layer, error : %i"), CreateLayerResult);
					return false;
				}

				XrResult ResumeLayerResult = xrPassthroughLayerResumeFB(XrPassthroughLayer);
				if (!XR_SUCCEEDED(ResumeLayerResult))
				{
					UE_LOG(LogOculusXRPassthrough, Warning, TEXT("Failed to resume passthrough layer, error : %i"), ResumeLayerResult);
					return false;
				}
			}
		}
		else
		{
			PassthroughExtension = InLayer->PassthroughExtension;
			UserDefinedGeometryMap = InLayer->UserDefinedGeometryMap;
			PassthroughPokeActorMap = InLayer->PassthroughPokeActorMap;
			Session = InLayer->Session;
			XrPassthroughLayer = InLayer->XrPassthroughLayer;
			XrCompositionLayerHeader = InLayer->XrCompositionLayerHeader;
			XrPassthroughInstance = InLayer->XrPassthroughInstance;
		}

		if (!UserDefinedGeometryMap)
		{
			UserDefinedGeometryMap = MakeShared<TMap<FString, FPassthroughMesh>, ESPMode::ThreadSafe>();
		}

		check(IsPassthoughLayerDesc(LayerDesc));

		return true;
	}

	bool FPassthroughLayer::BuildPassthroughPokeActor(OculusXRHMD::FOculusPassthroughMeshRef PassthroughMesh, FPassthroughPokeActor& OutPassthroughPokeActor)
	{
		UWorld* World = GetWorld();

		if (!World || !PassthroughExtension.IsValid())
		{
			return false;
		}

		const FString BaseComponentName = FString::Printf(TEXT("OculusPassthroughPoke_%d"), LayerDesc.Id);
		const FName ComponentName(*BaseComponentName);
		AActor* PassthoughPokeActor = World->SpawnActor<AActor>();
		UProceduralMeshComponent* PassthoughPokeComponentPtr = NewObject<UProceduralMeshComponent>(PassthoughPokeActor, ComponentName);
		PassthoughPokeComponentPtr->RegisterComponent();

		const TArray<int32>& Triangles = PassthroughMesh->GetTriangles();
		const TArray<FVector>& Vertices = PassthroughMesh->GetVertices();
		TArray<FVector> Normals;
		TArray<FVector2D> UV0;
		TArray<FLinearColor> VertexColors;
		TArray<FProcMeshTangent> Tangents;

		PassthoughPokeComponentPtr->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false);

		UMaterial* PokeAHoleMaterial = PassthroughExtension.Pin()->GetSettings()->PokeAHoleMaterial;

		if (PokeAHoleMaterial)
		{
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(PokeAHoleMaterial, nullptr);
			PassthoughPokeComponentPtr->SetMaterial(0, DynamicMaterial);
		}

		OutPassthroughPokeActor.PokeAHoleActor = PassthoughPokeActor;
		OutPassthroughPokeActor.PokeAHoleComponentPtr = PassthoughPokeComponentPtr;

		return true;
	}

	void FPassthroughLayer::UpdatePassthroughPokeActors_GameThread()
	{
		if (LayerDesc.HasShape<FUserDefinedLayer>())
		{
			const FUserDefinedLayer& UserDefinedLayerProps = LayerDesc.GetShape<FUserDefinedLayer>();
			const TArray<FUserDefinedGeometryDesc>& UserGeometryList = UserDefinedLayerProps.UserGeometryList;
			TSet<FString> UsedSet = {};

			if (PassthroughSupportsDepth())
			{
				for (const FUserDefinedGeometryDesc& GeometryDesc : UserGeometryList)
				{
					const FString MeshName = GeometryDesc.MeshName;
					UsedSet.Add(MeshName);

					FPassthroughPokeActor* FoundPassthroughPokeActor = PassthroughPokeActorMap->Find(MeshName);
					if (!FoundPassthroughPokeActor)
					{
						OculusXRHMD::FOculusPassthroughMeshRef GeomPassthroughMesh = GeometryDesc.PassthroughMesh;
						if (GeomPassthroughMesh)
						{
							FPassthroughPokeActor PassthroughPokeActor;
							if (BuildPassthroughPokeActor(GeomPassthroughMesh, PassthroughPokeActor))
							{
								PassthroughPokeActor.PokeAHoleComponentPtr->SetWorldTransform(GeometryDesc.Transform);
								PassthroughPokeActorMap->Add(MeshName, PassthroughPokeActor);
							}
						}
					}
					else if (GeometryDesc.bUpdateTransform && FoundPassthroughPokeActor->PokeAHoleComponentPtr.IsValid())
					{
						FoundPassthroughPokeActor->PokeAHoleComponentPtr->SetWorldTransform(GeometryDesc.Transform);
					}
				}
			}

			// find actors that no longer exist
			TArray<FString> ItemsToRemove;
			for (auto& Entry : *PassthroughPokeActorMap)
			{
				if (!UsedSet.Contains(Entry.Key))
				{
					ItemsToRemove.Add(Entry.Key);
				}
			}

			for (FString Entry : ItemsToRemove)
			{
				FPassthroughPokeActor* PassthroughPokeActor = PassthroughPokeActorMap->Find(Entry);
				if (PassthroughPokeActor)
				{
					UWorld* World = GetWorld();
					if (World && PassthroughPokeActor->PokeAHoleActor.IsValid())
					{
						World->DestroyActor(PassthroughPokeActor->PokeAHoleActor.Get());
					}
				}
				PassthroughPokeActorMap->Remove(Entry);
			}
		}
	}

	void FPassthroughLayer::UpdatePassthroughStyle_RenderThread(const FEdgeStyleParameters& EdgeStyleParameters)
	{
		if (!PassthroughExtension.IsValid())
		{
			return;
		}

		XrPassthroughStyleFB Style = { XR_TYPE_PASSTHROUGH_STYLE_FB };

		Style.textureOpacityFactor = EdgeStyleParameters.TextureOpacityFactor;

		Style.edgeColor = { 0, 0, 0, 0 };
		if (EdgeStyleParameters.bEnableEdgeColor)
		{
			Style.edgeColor = {
				EdgeStyleParameters.EdgeColor.R,
				EdgeStyleParameters.EdgeColor.G,
				EdgeStyleParameters.EdgeColor.B,
				EdgeStyleParameters.EdgeColor.A
			};
		}

		/// Color map
		union AllColorMapDescriptors
		{
			XrPassthroughColorMapMonoToRgbaFB rgba;
			XrPassthroughColorMapMonoToMonoFB mono;
			XrPassthroughBrightnessContrastSaturationFB bcs;
			XrPassthroughColorMapLutMETA lut;
			XrPassthroughColorMapInterpolatedLutMETA interpLut;
		};
		AllColorMapDescriptors colorMap;
		if (PassthroughExtension.Pin()->GetSettings()->bExtColorLutAvailable && EdgeStyleParameters.bEnableColorMap)
		{
			void* colorMapDataDestination = nullptr;
			unsigned int expectedColorMapDataSize = 0;
			switch (EdgeStyleParameters.ColorMapType)
			{
				case ColorMapType_None:
					break;
				case ColorMapType_GrayscaleToColor:
					colorMap.rgba = { XR_TYPE_PASSTHROUGH_COLOR_MAP_MONO_TO_RGBA_FB };
					expectedColorMapDataSize = sizeof(colorMap.rgba.textureColorMap);
					colorMapDataDestination = colorMap.rgba.textureColorMap;
					Style.next = &colorMap.rgba;
					break;
				case ColorMapType_Grayscale:
					colorMap.mono = { XR_TYPE_PASSTHROUGH_COLOR_MAP_MONO_TO_MONO_FB };
					expectedColorMapDataSize = sizeof(colorMap.mono.textureColorMap);
					colorMapDataDestination = colorMap.mono.textureColorMap;
					Style.next = &colorMap.mono;
					break;
				case ColorMapType_ColorAdjustment:
					colorMap.bcs = { XR_TYPE_PASSTHROUGH_BRIGHTNESS_CONTRAST_SATURATION_FB };
					expectedColorMapDataSize = 3 * sizeof(float);
					colorMapDataDestination = &colorMap.bcs.brightness;
					Style.next = &colorMap.bcs;
					break;
				case ColorMapType_ColorLut:
					colorMap.lut = { XR_TYPE_PASSTHROUGH_COLOR_MAP_LUT_META };
					colorMap.lut.colorLut = reinterpret_cast<const XrPassthroughColorLutMETA&>(EdgeStyleParameters.ColorLutDesc.ColorLuts[0]);
					colorMap.lut.weight = EdgeStyleParameters.ColorLutDesc.Weight;
					Style.next = &colorMap.lut;
					break;
				case ColorMapType_ColorLut_Interpolated:
					colorMap.interpLut = { XR_TYPE_PASSTHROUGH_COLOR_MAP_INTERPOLATED_LUT_META };
					colorMap.interpLut.sourceColorLut = reinterpret_cast<const XrPassthroughColorLutMETA&>(EdgeStyleParameters.ColorLutDesc.ColorLuts[0]);
					colorMap.interpLut.targetColorLut = reinterpret_cast<const XrPassthroughColorLutMETA&>(EdgeStyleParameters.ColorLutDesc.ColorLuts[1]);
					colorMap.interpLut.weight = EdgeStyleParameters.ColorLutDesc.Weight;
					Style.next = &colorMap.lut;
					break;
				default:
					UE_LOG(LogOculusXRPassthrough, Error, TEXT("Passthrough style has unexpected color map type: %i"), EdgeStyleParameters.ColorMapType);
					return;
			}

			// Validate color map data size and copy it over
			if (colorMapDataDestination != nullptr)
			{
				if (EdgeStyleParameters.ColorMapData.Num() != expectedColorMapDataSize)
				{
					UE_LOG(LogOculusXRPassthrough, Error,
						TEXT("Passthrough color map size for type %i is expected to be %i instead of %i"),
						EdgeStyleParameters.ColorMapType,
						expectedColorMapDataSize,
						EdgeStyleParameters.ColorMapData.Num());
					return;
				}

				uint8* ColorMapData = (uint8*)EdgeStyleParameters.ColorMapData.GetData();
				memcpy(colorMapDataDestination, ColorMapData, expectedColorMapDataSize);
			}
		}

		XrResult Result = xrPassthroughLayerSetStyleFB(XrPassthroughLayer, &Style);
		if (!XR_SUCCEEDED(Result))
		{
			UE_LOG(LogOculusXRPassthrough, Error, TEXT("Failed setting passthrough style, error : %i"), Result);
			return;
		}
	}

	static FMatrix TransformToPassthroughSpace(FTransform Transform, float WorldToMetersScale, FTransform TrackingToWorld)
	{
		const FVector WorldToMetersScaleInv = FVector(WorldToMetersScale).Reciprocal();
		FTransform TransformWorld = Transform * TrackingToWorld.Inverse();
		TransformWorld.MultiplyScale3D(WorldToMetersScaleInv);
		TransformWorld.ScaleTranslation(WorldToMetersScaleInv);
		const FMatrix TransformWorldScaled = TransformWorld.ToMatrixWithScale();

		const FMatrix SwapAxisMatrix(
			FPlane(0.0f, 0.0f, -1.0f, 0.0f),
			FPlane(1.0f, 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 1.0f, 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, 0.0f, 1.0f));

		return TransformWorldScaled * SwapAxisMatrix;
	}

	void FPassthroughLayer::UpdatePassthrough_RenderThread(FRHICommandListImmediate& RHICmdList, XrSpace Space, XrTime Time, float WorldToMetersScale, FTransform TrackingToWorld)
	{
		check(IsInRenderingThread());

		if (LayerDesc.HasShape<FReconstructedLayer>())
		{
			const FReconstructedLayer& ReconstructedLayerProps = LayerDesc.GetShape<FReconstructedLayer>();
			UpdatePassthroughStyle_RenderThread(ReconstructedLayerProps.EdgeStyleParameters);
		}
		else if (LayerDesc.HasShape<FUserDefinedLayer>())
		{
			const FUserDefinedLayer& UserDefinedLayerProps = LayerDesc.GetShape<FUserDefinedLayer>();
			UpdatePassthroughStyle_RenderThread(UserDefinedLayerProps.EdgeStyleParameters);
		}

		if (LayerDesc.HasShape<FUserDefinedLayer>())
		{
			const FUserDefinedLayer& UserDefinedLayerProps = LayerDesc.GetShape<FUserDefinedLayer>();
			const TArray<FUserDefinedGeometryDesc>& UserGeometryList = UserDefinedLayerProps.UserGeometryList;
			TSet<FString> UsedSet;

			for (const FUserDefinedGeometryDesc& GeometryDesc : UserGeometryList)
			{
				const FString MeshName = GeometryDesc.MeshName;
				UsedSet.Add(MeshName);

				FPassthroughMesh* LayerPassthroughMesh = UserDefinedGeometryMap->Find(MeshName);
				if (!LayerPassthroughMesh)
				{
					OculusXRHMD::FOculusPassthroughMeshRef GeomPassthroughMesh = GeometryDesc.PassthroughMesh;
					if (GeomPassthroughMesh)
					{
						const FMatrix Transform = TransformToPassthroughSpace(GeometryDesc.Transform, WorldToMetersScale, TrackingToWorld);
						XrTriangleMeshFB MeshHandle = 0;
						XrGeometryInstanceFB InstanceHandle = 0;
						AddPassthroughMesh_RenderThread(GeomPassthroughMesh->GetVertices(), GeomPassthroughMesh->GetTriangles(), Transform, Space, MeshHandle, InstanceHandle);
						UserDefinedGeometryMap->Add(MeshName, FPassthroughMesh(MeshHandle, InstanceHandle, GeometryDesc.Transform));
					}
				}
				else
				{
					const FMatrix Transform = TransformToPassthroughSpace(GeometryDesc.Transform, WorldToMetersScale, TrackingToWorld);
					UpdatePassthroughMeshTransform_RenderThread(LayerPassthroughMesh->InstanceHandle, Transform, Space, Time);
					LayerPassthroughMesh->LastTransform = GeometryDesc.Transform;
				}
			}

			// find meshes that no longer exist
			TArray<FString> ItemsToRemove;
			for (auto& Entry : *UserDefinedGeometryMap)
			{
				if (!UsedSet.Contains(Entry.Key))
				{
					ItemsToRemove.Add(Entry.Key);
				}
			}

			for (FString Entry : ItemsToRemove)
			{
				FPassthroughMesh* PassthroughMesh = UserDefinedGeometryMap->Find(Entry);
				if (PassthroughMesh)
				{
					const XrTriangleMeshFB MeshHandle = PassthroughMesh->MeshHandle;
					const XrGeometryInstanceFB InstanceHandle = PassthroughMesh->InstanceHandle;
					RemovePassthroughMesh_RenderThread(MeshHandle, InstanceHandle);
				}
				else
				{
					UE_LOG(LogOculusXRPassthrough, Error, TEXT("PassthroughMesh: %s doesn't exist."), *Entry);
					return;
				}

				UserDefinedGeometryMap->Remove(Entry);
			}
		}
	}

	XrCompositionLayerBaseHeaderType* FPassthroughLayer::GetXrCompositionLayerHeader()
	{
		OculusXRHMD::CheckInRHIThread();
		if (XrPassthroughLayer != nullptr)
		{
			XrCompositionLayerPassthroughFB& CompositionLayer = XrCompositionLayerHeader;
			memset(&CompositionLayer, 0, sizeof(CompositionLayer));
			CompositionLayer.type = XR_TYPE_COMPOSITION_LAYER_PASSTHROUGH_FB;
			CompositionLayer.layerHandle = XrPassthroughLayer;
			CompositionLayer.flags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
			CompositionLayer.space = XR_NULL_HANDLE;
			return reinterpret_cast<XrCompositionLayerBaseHeaderType*>(&CompositionLayer);
		}
		return nullptr;
	}

	bool FPassthroughLayer::IsBackgroundLayer() const
	{
		return (LayerDesc.HasShape<FReconstructedLayer>() && (LayerDesc.GetShape<FReconstructedLayer>().PassthroughLayerOrder == PassthroughLayerOrder_Underlay))
			|| (LayerDesc.HasShape<FUserDefinedLayer>() && (LayerDesc.GetShape<FUserDefinedLayer>().PassthroughLayerOrder == PassthroughLayerOrder_Underlay));
	}

	bool FPassthroughLayer::IsOverlayLayer() const
	{
		return (LayerDesc.HasShape<FReconstructedLayer>() && (LayerDesc.GetShape<FReconstructedLayer>().PassthroughLayerOrder == PassthroughLayerOrder_Overlay))
			|| (LayerDesc.HasShape<FUserDefinedLayer>() && (LayerDesc.GetShape<FUserDefinedLayer>().PassthroughLayerOrder == PassthroughLayerOrder_Overlay));
	}

	bool FPassthroughLayer::PassthroughSupportsDepth() const
	{
		return ((LayerDesc.Flags & IStereoLayers::LAYER_FLAG_SUPPORT_DEPTH) != 0) && LayerDesc.HasShape<FUserDefinedLayer>();
	}

	// Code taken from OVRPlugin (InsightMrManager.cpp)
	static bool DecomposeTransformMatrix(FMatrix Transform, XrPosef& OutPose, XrVector3f& OutScale)
	{
		FTransform outTransform = FTransform(Transform);
		FVector3f scale = FVector3f(outTransform.GetScale3D());
		FQuat4f rotation = FQuat4f(outTransform.GetRotation());
		FVector3f position = FVector3f(outTransform.GetLocation());

		if (scale.X == 0 || scale.Y == 0 || scale.Z == 0)
		{
			return false;
		}

		OutScale = XrVector3f{ scale.X, scale.Y, scale.Z };
		OutPose = XrPosef{ XrQuaternionf{ rotation.X, rotation.Y, rotation.Z, rotation.W }, XrVector3f{ position.X, position.Y, position.Z } };

		return true;
	}

	void FPassthroughLayer::AddPassthroughMesh_RenderThread(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, FMatrix Transformation, XrSpace Space, XrTriangleMeshFB& OutMeshHandle, XrGeometryInstanceFB& OutInstanceHandle)
	{
		OculusXRHMD::CheckInRenderThread();

		XrTriangleMeshFB MeshHandle = 0;
		XrGeometryInstanceFB InstanceHandle = 0;

		// Explicit conversion is needed since FVector contains double elements.
		// Converting Vertices.Data() to float* causes issues when memory is parsed.
		TArray<XrVector3f> VertexData;
		VertexData.SetNumUninitialized(Vertices.Num());

		size_t i = 0;
		for (const FVector& vertex : Vertices)
		{
			VertexData[i++] = { (float)vertex.X, (float)vertex.Y, (float)vertex.Z };
		}

		TArray<uint32_t> TriangleData;
		TriangleData.SetNumUninitialized(Triangles.Num());

		i = 0;
		for (const int32& tri : Triangles)
		{
			TriangleData[i++] = (uint32_t)tri;
		}

		XrTriangleMeshCreateInfoFB TriangleMeshInfo = { XR_TYPE_TRIANGLE_MESH_CREATE_INFO_FB };
		TriangleMeshInfo.flags = 0; // not mutable
		TriangleMeshInfo.triangleCount = Triangles.Num() / 3;
		TriangleMeshInfo.indexBuffer = TriangleData.GetData();
		TriangleMeshInfo.vertexCount = Vertices.Num();
		TriangleMeshInfo.vertexBuffer = VertexData.GetData();
		TriangleMeshInfo.windingOrder = XR_WINDING_ORDER_UNKNOWN_FB;

		if (XR_FAILED(xrCreateTriangleMeshFB.GetValue()(Session, &TriangleMeshInfo, &MeshHandle)))
		{
			UE_LOG(LogOculusXRPassthrough, Error, TEXT("Failed creating passthrough mesh surface."));
			return;
		}

		XrGeometryInstanceCreateInfoFB createInfo = { XR_TYPE_GEOMETRY_INSTANCE_CREATE_INFO_FB };

		bool result = DecomposeTransformMatrix(Transformation, createInfo.pose, createInfo.scale);
		if (!result)
		{
			UE_LOG(LogOculusXRPassthrough, Error, TEXT("Failed decomposing the transform matrix."));
			return;
		}

		createInfo.layer = XrPassthroughLayer;
		createInfo.mesh = MeshHandle;
		createInfo.baseSpace = Space;

		if (XR_FAILED(xrCreateGeometryInstanceFB(Session, &createInfo, &InstanceHandle)))
		{
			UE_LOG(LogOculusXRPassthrough, Error, TEXT("Failed adding passthrough mesh surface to scene."));
			return;
		}

		OutMeshHandle = MeshHandle;
		OutInstanceHandle = InstanceHandle;
	}

	void FPassthroughLayer::UpdatePassthroughMeshTransform_RenderThread(XrGeometryInstanceFB InstanceHandle, FMatrix Transformation, XrSpace Space, XrTime Time)
	{
		OculusXRHMD::CheckInRenderThread();

		XrGeometryInstanceTransformFB UpdateInfo = { XR_TYPE_GEOMETRY_INSTANCE_TRANSFORM_FB };
		bool result = DecomposeTransformMatrix(Transformation, UpdateInfo.pose, UpdateInfo.scale);
		if (!result)
		{
			UE_LOG(LogOculusXRPassthrough, Error, TEXT("Failed decomposing the transform matrix."));
			return;
		}

		UpdateInfo.baseSpace = Space;
		UpdateInfo.time = Time;

		if (XR_FAILED(xrGeometryInstanceSetTransformFB(InstanceHandle, &UpdateInfo)))
		{
			UE_LOG(LogOculusXRPassthrough, Error, TEXT("Failed updating passthrough mesh surface transform."));
			return;
		}
	}

	void FPassthroughLayer::RemovePassthroughMesh_RenderThread(XrTriangleMeshFB MeshHandle, XrGeometryInstanceFB InstanceHandle)
	{
		OculusXRHMD::CheckInRenderThread();

		if (XR_FAILED(xrDestroyGeometryInstanceFB(InstanceHandle)))
		{
			UE_LOG(LogOculusXRPassthrough, Error, TEXT("Failed removing passthrough surface from scene."));
			return;
		}

		if (XR_FAILED(xrDestroyTriangleMeshFB.GetValue()(MeshHandle)))
		{
			UE_LOG(LogOculusXRPassthrough, Error, TEXT("Failed destroying passthrough surface mesh."));
			return;
		}
	}

	void FPassthroughLayer::ClearPassthroughPokeActors()
	{
		if (PassthroughPokeActorMap)
		{
			UWorld* World = GetWorld();
			if (!World)
			{
				UE_LOG(LogOculusXRPassthrough, Warning, TEXT("Couldn't retrieve World. Passthrough Pokeahole actors will not be destroyed."));
				return;
			}

			for (auto& Entry : *PassthroughPokeActorMap)
			{
				// Check if actor is still valid. In some specific cases the actor might be destroyed before we clean the PokeActorMap
				// (e.g. when loading to a new level)
				if (Entry.Value.PokeAHoleActor.IsValid())
				{
					World->DestroyActor(Entry.Value.PokeAHoleActor.Get());
				}
			}
			PassthroughPokeActorMap.Reset();
		}
	}

} // namespace XRPassthrough
