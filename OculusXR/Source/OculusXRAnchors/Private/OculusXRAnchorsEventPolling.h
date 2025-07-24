// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAnchorTypes.h"
#include "OculusXRPluginWrapper.h"

namespace OculusXRAnchors
{
	struct FOculusXRAnchorsEventPolling
	{
	public:
		static void OnPollEvent(ovrpEventDataBuffer* EventDataBuffer, bool& EventPollResult);
	};

} // namespace OculusXRAnchors
