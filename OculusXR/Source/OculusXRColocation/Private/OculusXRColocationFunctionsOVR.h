// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRColocationFunctions.h"

class OCULUSXRCOLOCATION_API FOculusXRColocationFunctionsOVR : public IOculusXRColocationFunctions
{
public:
	virtual EColocationResult StartColocationDiscovery(uint64& OutRequestId) override;
	virtual EColocationResult StopColocationDiscovery(uint64& OutRequestId) override;

	virtual EColocationResult StartColocationAdvertisement(const TArray<uint8>& MetaData, uint64& OutRequestId) override;
	virtual EColocationResult StopColocationAdvertisement(uint64& OutRequestId) override;
};
