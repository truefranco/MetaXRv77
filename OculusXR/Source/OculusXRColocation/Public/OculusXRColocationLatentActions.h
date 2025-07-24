// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "OculusXRColocationTypes.h"
#include "OculusXRColocation.h"
#include "OculusXRColocationSession.h"
#include "OculusXRColocationLatentActions.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOculusXR_LatentAction_ColocationDiscoveryComplete, bool, Success, const FOculusXRColocationSession&, Session, const TArray<FOculusXRColocationSession>&, FoundSessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOculusXR_LatentAction_ColocationSessionFound, bool, Success, const FOculusXRColocationSession&, Session, const TArray<FOculusXRColocationSession>&, FoundSessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOculusXR_LatentAction_ColocationAdvertisementStarted, bool, Success, const FOculusXRColocationSession&, Session);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOculusXR_LatentAction_ColocationAdvertisementStopped, bool, Success);

//
// Find sessions
//
UCLASS()
class OCULUSXRCOLOCATION_API UOculusXRAsyncAction_DiscoverColocationSessions : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UOculusXRAsyncAction_DiscoverColocationSessions* OculusXRAsyncDiscoverColocationSessions();

	UPROPERTY(BlueprintAssignable)
	FOculusXR_LatentAction_ColocationSessionFound SessionFound;

	UPROPERTY(BlueprintAssignable)
	FOculusXR_LatentAction_ColocationDiscoveryComplete OnComplete;

private:
	void HandleSessionFound(const FOculusXRColocationSession& Session);
	void HandleSessionFindComplete(const OculusXRColocation::FDiscoverSessionsRequest::FResultType& Result);
};

//
// Start advertisement
//
UCLASS()
class OCULUSXRCOLOCATION_API UOculusXRAsyncAction_StartColocationSessionAdvertisement : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UOculusXRAsyncAction_StartColocationSessionAdvertisement* OculusXRAsyncStartColocationSessionAdvertisement(const TArray<uint8>& SessionData);

	UPROPERTY(BlueprintAssignable)
	FOculusXR_LatentAction_ColocationAdvertisementStarted OnComplete;

private:
	TArray<uint8> SessionData;

	void HandleStartSessionAdvertisementComplete(const OculusXRColocation::FStartSessionAdvertisementRequest::FResultType& Result);
};

//
// Stop advertisement
//
UCLASS()
class OCULUSXRCOLOCATION_API UOculusXRAsyncAction_StopColocationSessionAdvertisement : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UOculusXRAsyncAction_StopColocationSessionAdvertisement* OculusXRAsyncStopColocationSessionAdvertisement();

	UPROPERTY(BlueprintAssignable)
	FOculusXR_LatentAction_ColocationAdvertisementStopped OnComplete;

private:
	void HandleStopSessionAdvertisementComplete(const OculusXRColocation::FStopSessionAdvertisementRequest::FResultType& Result);
};
