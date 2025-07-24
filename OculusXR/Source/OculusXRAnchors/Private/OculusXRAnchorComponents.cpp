// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRAnchorComponents.h"

#include "OculusXRAnchorBPFunctionLibrary.h"
#include "OculusXRAnchorManager.h"
#include "OculusXRSpatialAnchorComponent.h"

bool UOculusXRBaseAnchorComponent::IsComponentEnabled() const
{
	bool OutEnabled;
	bool OutChangePending;

	auto OutResult = OculusXRAnchors::FOculusXRAnchorManager::GetAnchorComponentStatus(Space, Type, OutEnabled, OutChangePending);
	return UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(OutResult) && OutEnabled;
}

EOculusXRSpaceComponentType UOculusXRBaseAnchorComponent::GetType() const
{
	return Type;
}

uint64 UOculusXRBaseAnchorComponent::GetSpace() const
{
	return Space;
}

bool UOculusXRLocatableAnchorComponent::GetTransform(FTransform& outTransform) const
{
	ensure(IsComponentEnabled());

	if (!UOculusXRAnchorBPFunctionLibrary::GetAnchorTransformByHandle(Space, outTransform))
	{
		UE_LOG(LogOculusSpatialAnchor, Warning, TEXT("Fetching transform failed."));
		return false;
	}
	return true;
}

bool UOculusXRSpaceContainerAnchorComponent::GetUUIDs(TArray<FOculusXRUUID>& outUUIDs) const
{
	ensure(IsComponentEnabled());

	if (!OculusXRAnchors::FOculusXRAnchorManager::GetAnchorContainerUUIDs(Space, outUUIDs))
	{
		UE_LOG(LogOculusSpatialAnchor, Warning, TEXT("Fetching container uuids failed."));
		return false;
	}

	return true;
}
