// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "OculusXRHMDTypes.h"
#include "OpenXR/IOculusXRExtensionPlugin.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOculusXRGuardian, Log, All);

namespace OculusXR
{
	class FGuardianExtensionPlugin : public IOculusXRExtensionPlugin
	{
	public:
		// IOpenXRExtensionPlugin
		virtual void PostCreateSession(XrSession InSession) override;

		bool IsGuardianConfigured();
		bool GetGuardianPoints(TArray<FVector>& BoundaryPoints);
		FVector GetGuardianDimensions();

	private:
		XrSession Session;
	};
} // namespace OculusXR
