// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSceneLatentActions.h"
#include "OculusXRHMD.h"
#include "OculusXRAnchorBPFunctionLibrary.h"
#include "OculusXRSceneModule.h"
#include "OculusXRSceneDelegates.h"
#include "OculusXRScene.h"

UOculusXRAsyncAction_LaunchCaptureFlow* UOculusXRAsyncAction_LaunchCaptureFlow::LaunchCaptureFlowAsync(const UObject* WorldContext)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
	if (!ensureAlwaysMsgf(IsValid(WorldContext), TEXT("World Context was not valid.")))
	{
		return nullptr;
	}

	auto NewAction = NewObject<UOculusXRAsyncAction_LaunchCaptureFlow>();
	NewAction->RegisterWithGameInstance(World->GetGameInstance());
	return NewAction;
}

void UOculusXRAsyncAction_LaunchCaptureFlow::Activate()
{
	RequestId = 0;
	FOculusXRSceneEventDelegates::OculusSceneCaptureComplete.AddUObject(this, &UOculusXRAsyncAction_LaunchCaptureFlow::OnCaptureFinish);
	auto result = OculusXRScene::FOculusXRScene::RequestSceneCapture(RequestId);
	if (!UOculusXRAnchorBPFunctionLibrary::IsAnchorResultSuccess(result))
	{
		FOculusXRSceneEventDelegates::OculusSceneCaptureComplete.RemoveAll(this);
		Failure.Broadcast();
	}
}

void UOculusXRAsyncAction_LaunchCaptureFlow::OnCaptureFinish(FOculusXRUInt64 Id, bool bSuccess)
{
	if (RequestId != Id.GetValue())
	{
		UE_LOG(LogOculusXRScene, Verbose, TEXT("Incoming request id (%llu) doesn't match expected request id (%llu). Ignoring request."), Id.GetValue(), RequestId);
		return;
	}

	FOculusXRSceneEventDelegates::OculusSceneCaptureComplete.RemoveAll(this);
	Success.Broadcast();
	SetReadyToDestroy();
}
