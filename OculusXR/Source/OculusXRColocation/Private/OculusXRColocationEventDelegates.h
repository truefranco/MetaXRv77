// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "CoreTypes.h"
#include "OculusXRColocationTypes.h"
#include "Delegates/Delegate.h"

class FOculusXRColocationEventDelegates
{
public:
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FStartColocationAdvertisementComplete, FOculusXRUInt64 /*requestId*/, FOculusXRUUID /*uuid*/, EColocationResult /*result*/);
	static OCULUSXRCOLOCATION_API FStartColocationAdvertisementComplete StartColocationAdvertisementComplete;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FColocationAdvertisementComplete, FOculusXRUInt64 /*requestId*/, EColocationResult /*result*/);
	static OCULUSXRCOLOCATION_API FColocationAdvertisementComplete ColocationAdvertisementComplete;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FStopColocationAdvertisementComplete, FOculusXRUInt64 /*requestId*/, EColocationResult /*result*/);
	static OCULUSXRCOLOCATION_API FStopColocationAdvertisementComplete StopColocationAdvertisementComplete;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FStartColocationDiscoveryComplete, FOculusXRUInt64 /*requestId*/, EColocationResult /*result*/);
	static OCULUSXRCOLOCATION_API FStartColocationDiscoveryComplete StartColocationDiscoveryComplete;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FColocationDiscoveryComplete, FOculusXRUInt64 /*requestId*/, EColocationResult /*result*/);
	static OCULUSXRCOLOCATION_API FColocationDiscoveryComplete ColocationDiscoveryComplete;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FStopColocationDiscoveryComplete, FOculusXRUInt64 /*requestId*/, EColocationResult /*result*/);
	static OCULUSXRCOLOCATION_API FStopColocationDiscoveryComplete StopColocationDiscoveryComplete;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FColocationDiscoveryResultAvailable, FOculusXRUInt64 /*requestId*/, FOculusXRUUID /*resultUuid*/, const TArray<uint8>& /*metadata*/);
	static OCULUSXRCOLOCATION_API FColocationDiscoveryResultAvailable ColocationDiscoveryResultAvailable;
};
