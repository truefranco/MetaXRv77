// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRPluginWrapper.h"

namespace OculusXRColocation
{
	struct FColocationEventPolling
	{
	public:
		static void OnPollEvent(ovrpEventDataBuffer* EventDataBuffer, bool& EventPollResult);
	};
} // namespace OculusXRColocation
