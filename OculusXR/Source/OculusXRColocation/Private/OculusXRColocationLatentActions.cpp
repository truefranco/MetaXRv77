// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationLatentActions.h"
#include "OculusXRHMD.h"
#include "OculusXRAnchorBPFunctionLibrary.h"
#include "OculusXRColocationModule.h"
#include "OculusXRColocationSession.h"
#include "OculusXRAnchorManager.h"

//
// Find sessions
//
void UOculusXRAsyncAction_DiscoverColocationSessions::Activate()
{
	OculusXRColocation::FColocation::DiscoverSessionsAsync(
		OculusXRColocation::FDiscoverSessionsRequest::FCompleteDelegate::CreateUObject(this, &UOculusXRAsyncAction_DiscoverColocationSessions::HandleSessionFindComplete),
		FOculusXRColocationSessionFoundDelegate::CreateUObject(this, &UOculusXRAsyncAction_DiscoverColocationSessions::HandleSessionFound));
}

UOculusXRAsyncAction_DiscoverColocationSessions* UOculusXRAsyncAction_DiscoverColocationSessions::OculusXRAsyncDiscoverColocationSessions()
{
	UOculusXRAsyncAction_DiscoverColocationSessions* Action = NewObject<UOculusXRAsyncAction_DiscoverColocationSessions>();
	Action->RegisterWithGameInstance(GWorld);

	return Action;
}

void UOculusXRAsyncAction_DiscoverColocationSessions::HandleSessionFound(const FOculusXRColocationSession& Session)
{
	TArray<FOculusXRColocationSession> emptyArray;
	SessionFound.Broadcast(true, Session, emptyArray);
}

void UOculusXRAsyncAction_DiscoverColocationSessions::HandleSessionFindComplete(const OculusXRColocation::FDiscoverSessionsRequest::FResultType& Result)
{
	if (Result.IsSuccess())
	{
		OnComplete.Broadcast(true, FOculusXRColocationSession(), Result.GetValue());
	}
	else
	{
		TArray<FOculusXRColocationSession> emptyArray;
		OnComplete.Broadcast(false, FOculusXRColocationSession(), emptyArray);
	}

	SetReadyToDestroy();
}

//
// Start session advertisement
//
void UOculusXRAsyncAction_StartColocationSessionAdvertisement::Activate()
{
	OculusXRColocation::FColocation::StartSessionAdvertisementAsync(
		SessionData,
		OculusXRColocation::FStartSessionAdvertisementRequest::FCompleteDelegate::CreateUObject(
			this, &UOculusXRAsyncAction_StartColocationSessionAdvertisement::HandleStartSessionAdvertisementComplete));
}

UOculusXRAsyncAction_StartColocationSessionAdvertisement* UOculusXRAsyncAction_StartColocationSessionAdvertisement::OculusXRAsyncStartColocationSessionAdvertisement(const TArray<uint8>& SessionData)
{
	UOculusXRAsyncAction_StartColocationSessionAdvertisement* Action = NewObject<UOculusXRAsyncAction_StartColocationSessionAdvertisement>();
	Action->RegisterWithGameInstance(GWorld);
	Action->SessionData = SessionData;
	return Action;
}

void UOculusXRAsyncAction_StartColocationSessionAdvertisement::HandleStartSessionAdvertisementComplete(
	const OculusXRColocation::FStartSessionAdvertisementRequest::FResultType& Result)
{
	OnComplete.Broadcast(Result.IsSuccess(), Result.IsSuccess() ? Result.GetValue() : FOculusXRColocationSession());
	SetReadyToDestroy();
}

//
// Stop session advertisement
//
void UOculusXRAsyncAction_StopColocationSessionAdvertisement::Activate()
{
	OculusXRColocation::FColocation::StopSessionAdvertisementAsync(
		OculusXRColocation::FStopSessionAdvertisementRequest::FCompleteDelegate::CreateUObject(
			this, &UOculusXRAsyncAction_StopColocationSessionAdvertisement::HandleStopSessionAdvertisementComplete));
}

UOculusXRAsyncAction_StopColocationSessionAdvertisement* UOculusXRAsyncAction_StopColocationSessionAdvertisement::OculusXRAsyncStopColocationSessionAdvertisement()
{
	UOculusXRAsyncAction_StopColocationSessionAdvertisement* Action = NewObject<UOculusXRAsyncAction_StopColocationSessionAdvertisement>();
	Action->RegisterWithGameInstance(GWorld);
	return Action;
}

void UOculusXRAsyncAction_StopColocationSessionAdvertisement::HandleStopSessionAdvertisementComplete(
	const OculusXRColocation::FStopSessionAdvertisementRequest::FResultType& Result)
{
	OnComplete.Broadcast(Result.IsSuccess());
	SetReadyToDestroy();
}
