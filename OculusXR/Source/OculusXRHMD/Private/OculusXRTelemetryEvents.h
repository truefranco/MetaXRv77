// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRTelemetry.h"

namespace OculusXRTelemetry::Events
{
	using FEditorConsent = TMarker<191965622>;
	using FSimulator = TMarker<191963436>;
	using FEnableHardOcclusions = TMarker<191958638>;
	using FEnableSoftOcclusions = TMarker<191958877>;
	constexpr const char* ConsentOriginKey = "Origin";
} // namespace OculusXRTelemetry::Events
