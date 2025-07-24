// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationFunctionLibrary.h"
#include "OculusXRColocationModule.h"
#include "OculusXRColocationSubsystem.h"
#include "OculusXRColocation.h"

void UOculusXRColocationFunctionLibrary::StopColocationSessionDiscovery()
{
	auto request = UOculusXRColocationSubsystem::Get()->GetDiscoveryRequest();
	if (!request.IsValid())
	{
		UE_LOG(LogOculusXRColocation, Warning, TEXT("Attempted to stop colocation session discovery but there is no active discovery request!"));
		return;
	}

	OculusXRColocation::FColocation::StopDiscoverSessions(request);
}
