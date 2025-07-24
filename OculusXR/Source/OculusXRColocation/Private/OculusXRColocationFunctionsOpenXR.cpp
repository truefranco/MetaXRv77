// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationFunctionsOpenXR.h"
#include "OculusXRHMD.h"
#include "OculusXRColocationUtil.h"
#include "OculusXRColocationModule.h"

EColocationResult FOculusXRColocationFunctionsOpenXR::StartColocationDiscovery(uint64& OutRequestId)
{
	auto result = FOculusXRColocationModule::Get().GetXrColocation()->StartColocationDiscovery(OutRequestId);
	return OculusXRColocation::GetResult(result);
}

EColocationResult FOculusXRColocationFunctionsOpenXR::StopColocationDiscovery(uint64& OutRequestId)
{
	auto result = FOculusXRColocationModule::Get().GetXrColocation()->StopColocationDiscovery(OutRequestId);
	return OculusXRColocation::GetResult(result);
}

EColocationResult FOculusXRColocationFunctionsOpenXR::StartColocationAdvertisement(const TArray<uint8>& MetaData, uint64& OutRequestId)
{
	auto result = FOculusXRColocationModule::Get().GetXrColocation()->StartColocationAdvertisement(MetaData, OutRequestId);
	return OculusXRColocation::GetResult(result);
}

EColocationResult FOculusXRColocationFunctionsOpenXR::StopColocationAdvertisement(uint64& OutRequestId)
{
	auto result = FOculusXRColocationModule::Get().GetXrColocation()->StopColocationAdvertisement(OutRequestId);
	return OculusXRColocation::GetResult(result);
}
