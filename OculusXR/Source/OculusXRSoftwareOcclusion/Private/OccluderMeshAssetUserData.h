// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "StaticMeshOccluderData.h"

#include "OccluderMeshAssetUserData.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSofwareOcclusion, Log, All);

class FArchive;
class UObject;
class FStaticMeshOccluderData;
struct FStaticMeshLODResources;
class UStaticMesh;

/**
 * This AssertUserData is used to hold both LODForOccluderMesh and CustomOccluderMesh for UStaticMeshes or USkeletalMeshes.
 * Note that LODForOccluderMesh and CustomOccluderMesh are exclusive.
 */
UCLASS()
class UOccluderMeshAssetUserData : public UAssetUserData
{
	GENERATED_BODY()

public:
	UOccluderMeshAssetUserData(const FObjectInitializer& ObjectInitializer);

	/**
	 *	Specifies the custom occluder mesh for software occlusion
	 *  Mutually exclusive with LODForOccluderMesh
	 */
	UPROPERTY(EditAnywhere, Category = "CustomOccluder", meta = (DisplayName = "Custom Occluder Mesh", EditCondition = "LODForOccluderMesh < 0"))
	TObjectPtr<UStaticMesh> CustomOccluderMesh;

	/**
	 *	Specifies the custom occluder mesh scale if different with that of the parent component
	 */
	UPROPERTY(EditAnywhere, Category = "CustomOccluder", meta = (DisplayName = "Custom Occluder Mesh Scale", EditCondition = "CustomOccluderMesh != nullptr"))
	FVector CustomOccluderMeshScale = FVector::OneVector;

	/**
	 *	Specifies the custom occluder mesh offset if different with that of the parent component
	 */
	UPROPERTY(EditAnywhere, Category = "CustomOccluder", meta = (DisplayName = "Custom Occluder Mesh Offset", EditCondition = "CustomOccluderMesh != nullptr"))
	FVector CustomOccluderMeshOffset = FVector::ZeroVector;

	/**
	 *	Specifies which staticmesh LOD to use as occluder geometry for software occlusion. Not suitable for skeletal meshes.
	 *  Mutually exclusive with CustomOccluderMesh
	 *  Set to -1 to not use this mesh as occluder
	 */
	UPROPERTY(EditAnywhere, Category = "LOD", AdvancedDisplay, meta = (DisplayName = "Parent StaticMesh LOD For Occluder Mesh"))
	int32 LODForOccluderMesh = -1;

	//~ Begin UAssetUserData
	virtual void Serialize(FArchive& Ar) override;

#ifdef WITH_OCULUS_BRANCH
	virtual void FinishCompilation() override;
	virtual FAssetUserRenderData* GetRenderData() const override { return OccluderData.Get(); }
	virtual void CacheDerivedData() override;
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UAssetUserData

	TObjectPtr<UStaticMesh> GetCustomOccluderMesh();

	FStaticMeshOccluderData* GetOccluderData();
	const FStaticMeshOccluderData* GetOccluderData() const;
	void SetOccluderData(TUniquePtr<class FStaticMeshOccluderData>&& InOccluderData);

private:
	void BuildOccluderData();

	bool IsOwnerSupported();

	TUniquePtr<FStaticMeshOccluderData> Build(const FStaticMeshLODResources* LODModel);

	TUniquePtr<class FStaticMeshOccluderData> OccluderData;
#endif // WITH_OCULUS_BRANCH
};
