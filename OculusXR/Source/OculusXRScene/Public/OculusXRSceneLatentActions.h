// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "Templates/SharedPointer.h"
#include "OculusXRSceneTypes.h"
#include "OculusXRSceneLatentActions.generated.h"

UCLASS()
class OCULUSXRSCENE_API UOculusXRAsyncAction_LaunchCaptureFlow : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOculusXRAsyncAction_CaptureFlowFinished);

	UFUNCTION(BlueprintCallable, Category = "OculusXR|SpatialAnchor", meta = (WorldContext = "WorldContext", BlueprintInternalUseOnly = "true"))
	static UOculusXRAsyncAction_LaunchCaptureFlow* LaunchCaptureFlowAsync(const UObject* WorldContext);

	void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FOculusXRAsyncAction_CaptureFlowFinished Success;

	UPROPERTY(BlueprintAssignable)
	FOculusXRAsyncAction_CaptureFlowFinished Failure;

private:
	uint64 RequestId = 0;

	UFUNCTION(CallInEditor)
	void OnCaptureFinish(FOculusXRUInt64 Id, bool bSuccess);
};
