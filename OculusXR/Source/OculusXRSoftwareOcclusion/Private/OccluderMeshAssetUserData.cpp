// Copyright Epic Games, Inc. All Rights Reserved.

#include "OccluderMeshAssetUserData.h"
#include "Engine/StaticMesh.h"
#include "Misc/AssertionMacros.h"
#include "UObject/ObjectPtr.h"

#include "SceneManagement.h"
#include "StaticMeshResources.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"

#if WITH_EDITOR
#include "StaticMeshCompiler.h"
#endif // #if WITH_EDITOR

DEFINE_LOG_CATEGORY(LogSofwareOcclusion);

#include UE_INLINE_GENERATED_CPP_BY_NAME(OccluderMeshAssetUserData)

UOccluderMeshAssetUserData::UOccluderMeshAssetUserData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOccluderMeshAssetUserData::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
#ifdef WITH_OCULUS_BRANCH
	bool bCooked = Ar.IsCooking();
	Ar << bCooked;

	bool bSerizlie = bCooked && !IsTemplate() && !Ar.IsCountingMemory();
	if (!bSerizlie)
	{
		return;
	}

#if WITH_EDITOR
	static const auto CVarMobileAllowSfOc = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.Mobile.AllowCustomOcclusion"));
	if (Ar.IsSaving() && CVarMobileAllowSfOc && (CVarMobileAllowSfOc->GetValueOnAnyThread() != 0))
	{
		bool bHasOccluderData = false;
		if (OccluderData)
		{
			bHasOccluderData = true;
		}

		Ar << bHasOccluderData;

		if (bHasOccluderData)
		{
			OccluderData->VerticesSP->BulkSerialize(Ar);
			OccluderData->IndicesSP->BulkSerialize(Ar);
			Ar << OccluderData->OccluderMeshScale;
			Ar << OccluderData->OccluderMeshOffset;
		}
	}
	else
#endif // WITH_EDITOR
	{
		bool bHasOccluderData;
		Ar << bHasOccluderData;
		if (bHasOccluderData)
		{
			SetOccluderData(MakeUnique<FStaticMeshOccluderData>());

			OccluderData->VerticesSP->BulkSerialize(Ar);
			OccluderData->IndicesSP->BulkSerialize(Ar);
			Ar << OccluderData->OccluderMeshScale;
			Ar << OccluderData->OccluderMeshOffset;
		}
	}
#endif // WITH_OCULUS_BRANCH
}

#ifdef WITH_OCULUS_BRANCH
void UOccluderMeshAssetUserData::FinishCompilation()
{
#if WITH_EDITOR
	if (CustomOccluderMesh && CustomOccluderMesh.Get() != GetOuter() && CustomOccluderMesh->IsCompiling())
	{
		FStaticMeshCompilingManager::Get().FinishCompilation({ CustomOccluderMesh });
	}
#endif
}

void UOccluderMeshAssetUserData::CacheDerivedData()
{
	BuildOccluderData();
}

void UOccluderMeshAssetUserData::PostLoad()
{
	Super::PostLoad();

	if (CustomOccluderMesh)
	{
		CustomOccluderMesh->ConditionalPostLoad();
	}
}

#if WITH_EDITOR
void UOccluderMeshAssetUserData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (IsOwnerSupported())
	{
		if (Cast<USkeletalMesh>(GetOuter())
			&& PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UOccluderMeshAssetUserData, LODForOccluderMesh)
			&& LODForOccluderMesh != -1)
		{
			UE_LOG(LogSofwareOcclusion, Error, TEXT("LODForOccluderMesh CANNOT be used for SkeletalMesh."));
			LODForOccluderMesh = -1;
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UOccluderMeshAssetUserData::BuildOccluderData()
{
	if (!IsOwnerSupported())
	{
		return;
	}

	// Add logic to build;
	// Conditionally create occluder data
	const FStaticMeshLODResources* LODModel = nullptr;
	if (LODForOccluderMesh >= 0 && Cast<UStaticMesh>(GetOuter()))
	{
		UStaticMesh* Owner = Cast<UStaticMesh>(GetOuter());

		int32 LODIndex = FMath::Min(LODForOccluderMesh, Owner->GetRenderData()->LODResources.Num() - 1);
		LODModel = &Owner->GetRenderData()->LODResources[LODIndex];
		SetOccluderData(Build(LODModel));
	}
	else if (CustomOccluderMesh)
	{
		if (CustomOccluderMesh->GetRenderData())
		{
			LODModel = &CustomOccluderMesh->GetRenderData()->LODResources[0];
			SetOccluderData(Build(LODModel));
		}
		else
		{
			UE_LOG(LogSofwareOcclusion, Error, TEXT("CustomOccluderMesh is not setup correctly for %s."), *GetName());
		}
	}
}

TObjectPtr<UStaticMesh> UOccluderMeshAssetUserData::GetCustomOccluderMesh()
{
	return CustomOccluderMesh;
}

TUniquePtr<FStaticMeshOccluderData> UOccluderMeshAssetUserData::Build(const FStaticMeshLODResources* LODModel)
{
	TUniquePtr<FStaticMeshOccluderData> Result;
#if WITH_EDITOR
	check(LODModel);

	const FRawStaticIndexBuffer& IndexBuffer = LODModel->DepthOnlyIndexBuffer.GetNumIndices() > 0 ? LODModel->DepthOnlyIndexBuffer : LODModel->IndexBuffer;
	int32 NumVtx = LODModel->VertexBuffers.PositionVertexBuffer.GetNumVertices();
	int32 NumIndices = IndexBuffer.GetNumIndices();

	if (NumVtx > 0 && NumIndices > 0 && !IndexBuffer.Is32Bit())
	{
		Result = MakeUnique<FStaticMeshOccluderData>();

		Result->VerticesSP->SetNumUninitialized(NumVtx);
		Result->IndicesSP->SetNumUninitialized(NumIndices);

		FVector V0 = (FVector)(LODModel->VertexBuffers.PositionVertexBuffer.VertexPosition(0));
		const uint16* Indices = IndexBuffer.AccessStream16();

		for (int i = 0; i < NumVtx; ++i)
		{
			FVector Elem = FVector(LODModel->VertexBuffers.PositionVertexBuffer.VertexPosition(i));
			Result->VerticesSP->GetData()[i] = Elem;
		}

		for (int i = 0; i < NumIndices; ++i)
		{
			uint16 Elem = IndexBuffer.AccessStream16()[i];
			Result->IndicesSP->GetData()[i] = Elem;
		}

		if (CustomOccluderMesh)
		{
			Result->OccluderMeshScale = CustomOccluderMeshScale;
			Result->OccluderMeshOffset = CustomOccluderMeshOffset;
		}
	}
#endif // WITH_EDITOR
	return Result;
}

bool UOccluderMeshAssetUserData::IsOwnerSupported()
{
	return (Cast<UStaticMesh>(GetOuter()) || Cast<USkeletalMesh>(GetOuter()));
}

FStaticMeshOccluderData* UOccluderMeshAssetUserData::GetOccluderData()
{
	// TODO: Do we still need below?
	// WaitUntilAsyncPropertyReleased(EStaticMeshAsyncProperties::OccluderData);

	return OccluderData.Get();
}

const FStaticMeshOccluderData* UOccluderMeshAssetUserData::GetOccluderData() const
{
	return OccluderData.Get();
}

void UOccluderMeshAssetUserData::SetOccluderData(TUniquePtr<class FStaticMeshOccluderData>&& InOccluderData)
{
	OccluderData = MoveTemp(InOccluderData);
}
#endif // WITH_OCULUS_BRANCH
