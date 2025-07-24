// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "CoreMinimal.h"
#include "OculusXRColocationTypes.h"
#include "OculusXRColocationRequests.h"

namespace OculusXRColocation
{
	struct OCULUSXRCOLOCATION_API FColocation
	{
		static TSharedPtr<FDiscoverSessionsRequest> DiscoverSessionsAsync(const FDiscoverSessionsRequest::FCompleteDelegate& OnComplete, const FOculusXRColocationSessionFoundDelegate& OnSessionFound);
		static EColocationResult StopDiscoverSessions(TSharedPtr<FDiscoverSessionsRequest> Request);
		static TSharedPtr<FStartSessionAdvertisementRequest> StartSessionAdvertisementAsync(const TArray<uint8>& SessionData, const FStartSessionAdvertisementRequest::FCompleteDelegate& OnComplete);
		static TSharedPtr<FStopSessionAdvertisementRequest> StopSessionAdvertisementAsync(const FStopSessionAdvertisementRequest::FCompleteDelegate& OnComplete);
	};

} // namespace OculusXRColocation
