// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationFunctionsOVR.h"
#include "OculusXRHMD.h"
#include "OculusXRColocationUtil.h"
#include "OculusXRColocationModule.h"

EColocationResult FOculusXRColocationFunctionsOVR::StartColocationDiscovery(uint64& OutRequestId)
{
	ovrpResult result = FOculusXRHMDModule::GetPluginWrapper().StartColocationDiscovery(&OutRequestId);
	auto ueResult = OculusXRColocation::GetResult(result);

	UE_LOG(LogOculusXRColocation, Log, TEXT("[OVR::StartColocationDiscovery] RequestID: %llu, Launch async result: %s"),
		OutRequestId, *OculusXRColocation::ToString(ueResult));

	return ueResult;
}

EColocationResult FOculusXRColocationFunctionsOVR::StopColocationDiscovery(uint64& OutRequestId)
{
	ovrpResult result = FOculusXRHMDModule::GetPluginWrapper().StopColocationDiscovery(&OutRequestId);
	auto ueResult = OculusXRColocation::GetResult(result);

	UE_LOG(LogOculusXRColocation, Log, TEXT("[OVR::StopColocationDiscovery] RequestID: %llu, Launch async result: %s"),
		OutRequestId, *OculusXRColocation::ToString(ueResult));

	return ueResult;
}

EColocationResult FOculusXRColocationFunctionsOVR::StartColocationAdvertisement(const TArray<uint8>& MetaData, uint64& OutRequestId)
{
	ovrpColocationAdvertisementStartInfo startInfo;
	startInfo.Buffer = (ovrpByte*)MetaData.GetData();
	startInfo.BufferSize = MetaData.Num();

	ovrpResult result = FOculusXRHMDModule::GetPluginWrapper().StartColocationAdvertisement(&startInfo, &OutRequestId);
	auto ueResult = OculusXRColocation::GetResult(result);

	UE_LOG(LogOculusXRColocation, Log, TEXT("[OVR::StartColocationAdvertisement] RequestID: %llu, Launch async result: %s"),
		OutRequestId, *OculusXRColocation::ToString(ueResult));

	return ueResult;
}

EColocationResult FOculusXRColocationFunctionsOVR::StopColocationAdvertisement(uint64& OutRequestId)
{
	ovrpResult result = FOculusXRHMDModule::GetPluginWrapper().StopColocationAdvertisement(&OutRequestId);
	auto ueResult = OculusXRColocation::GetResult(result);

	UE_LOG(LogOculusXRColocation, Log, TEXT("[OVR::StopColocationAdvertisement] RequestID: %llu, Launch async result: %s"),
		OutRequestId, *OculusXRColocation::ToString(ueResult));

	return ueResult;
}
