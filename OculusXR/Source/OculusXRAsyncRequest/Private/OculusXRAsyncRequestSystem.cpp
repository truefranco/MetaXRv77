// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRAsyncRequestSystem.h"
#include "OculusXRAsyncRequestSubsystem.h"

namespace OculusXR
{
	FAsyncRequestSystem::FAsyncRequestSystem()
		: EventIdCounter(0)
	{
	}

	FAsyncRequestBase::EventId FAsyncRequestSystem::GenerateEventId()
	{
		return FAsyncRequestBase::EventId(++(GetInstance().EventIdCounter));
	}

	FAsyncRequestSystem& FAsyncRequestSystem::GetInstance()
	{
		return *UOculusXRAsyncRequestSubsystem::GetRequestSystem();
	}
} // namespace OculusXR
