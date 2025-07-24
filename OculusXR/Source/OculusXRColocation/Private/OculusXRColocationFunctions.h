// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRColocationTypes.h"

class OCULUSXRCOLOCATION_API IOculusXRColocationFunctions
{
public:
	virtual EColocationResult StartColocationDiscovery(uint64& OutRequestId) = 0;
	virtual EColocationResult StopColocationDiscovery(uint64& OutRequestId) = 0;

	virtual EColocationResult StartColocationAdvertisement(const TArray<uint8>& MetaData, uint64& OutRequestId) = 0;
	virtual EColocationResult StopColocationAdvertisement(uint64& OutRequestId) = 0;

	static TSharedPtr<IOculusXRColocationFunctions> GetOculusXRColocationFunctionsImpl();
	static TSharedPtr<IOculusXRColocationFunctions> ColocationFunctionsImpl;
};
