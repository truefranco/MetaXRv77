// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"

#ifdef WITH_OCULUS_BRANCH
/**
 * Convenience typedefs for a software occlusion mesh elements
 */
typedef TArray<FVector> FOccluderVertexArray;
typedef TArray<uint16> FOccluderIndexArray;
typedef TSharedPtr<FOccluderVertexArray, ESPMode::ThreadSafe> FOccluderVertexArraySP;
typedef TSharedPtr<FOccluderIndexArray, ESPMode::ThreadSafe> FOccluderIndexArraySP;

/**
 * This geometry is used to rasterize mesh for software occlusion
 * Generated only for if SoftwareOcclusion is supported
 */
class FStaticMeshOccluderData : public FAssetUserRenderData
{
public:
	FORCEINLINE static uint32 StaticType()
	{
		static uint32 SoftwareOccluderRenderDataType = FAssetUserRenderData::RegisterRenderDataType(TEXT("OculusXRSoftwareOccluderData"));
		return SoftwareOccluderRenderDataType;
	}

	FStaticMeshOccluderData()
		: FAssetUserRenderData(StaticType())
	{
		VerticesSP = MakeShared<FOccluderVertexArray, ESPMode::ThreadSafe>();
		IndicesSP = MakeShared<FOccluderIndexArray, ESPMode::ThreadSafe>();
		OccluderMeshScale = FVector::OneVector;
		OccluderMeshOffset = FVector::ZeroVector;
	}

	SIZE_T GetResourceSizeBytes() const
	{
		return VerticesSP->GetAllocatedSize() + IndicesSP->GetAllocatedSize();
	}

	FOccluderVertexArraySP VerticesSP;
	FOccluderIndexArraySP IndicesSP;
	FVector OccluderMeshScale;
	FVector OccluderMeshOffset;
};
#endif // WITH_OCULUS_BRANCH
