// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRColocationSession.h"
#include "OculusXRColocationRequests.h"
#include <Engine/World.h>
#include <Subsystems/GameInstanceSubsystem.h>

#include "OculusXRColocationSubsystem.generated.h"

UCLASS()
class OCULUSXRCOLOCATION_API UOculusXRColocationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static UOculusXRColocationSubsystem* Get();

	UOculusXRColocationSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void AssignLocalColocationSessionData(const FOculusXRUUID& Uuid, const TArray<uint8>& Data);
	void ClearLocalColocationSession();
	bool HasLocalColocationSession() const;
	const FOculusXRColocationSession& GetLocalSession() const;

	void SetDiscoveryRequest(TSharedPtr<OculusXRColocation::FDiscoverSessionsRequest> Request);
	void ClearDiscoveryRequest();
	TSharedPtr<OculusXRColocation::FDiscoverSessionsRequest> GetDiscoveryRequest() const;

private:
	void OnColocationAdvertisementStart(FOculusXRUInt64 RequestId, FOculusXRUUID Uuid, EColocationResult Result);
	void OnColocationAdvertisementStopped(FOculusXRUInt64 RequestId, EColocationResult Result);

	FDelegateHandle OnAdvertisementStartHandle;
	FDelegateHandle OnAdvertisementStoppedHandle;
	bool bHasLocalSession;
	FOculusXRColocationSession LocalSession;
	TSharedPtr<OculusXRColocation::FDiscoverSessionsRequest> DiscoverSessionsRequest;
};
