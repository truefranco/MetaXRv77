// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRColocationXRIncludes.h"
#include "IOpenXRExtensionPlugin.h"
#include "OculusXRColocationTypes.h"

#define LOCTEXT_NAMESPACE "OculusXRColocation"

class FOpenXRHMD;

namespace XRColocation
{
	extern PFN_xrStartColocationDiscoveryMETA xrStartColocationDiscoveryMETA;
	extern PFN_xrStopColocationDiscoveryMETA xrStopColocationDiscoveryMETA;
	extern PFN_xrStartColocationAdvertisementMETA xrStartColocationAdvertisementMETA;
	extern PFN_xrStopColocationAdvertisementMETA xrStopColocationAdvertisementMETA;

	class FColocationXR : public IOpenXRExtensionPlugin
	{
	public:
		// IOculusXROpenXRHMDPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual const void* OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext) override;
		virtual const void* OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext) override;
		virtual void OnDestroySession(XrSession InSession) override;
		virtual void OnEvent(XrSession InSession, const XrEventDataBaseHeader* InHeader) override;

	public:
		FColocationXR();
		virtual ~FColocationXR();

		void RegisterAsOpenXRExtension();

		bool IsColocationDiscoveryEnabled() const { return bExtColocationDiscoveryEnabled; }

		XrResult StartColocationDiscovery(uint64& OutRequestId);
		XrResult StopColocationDiscovery(uint64& OutRequestId);
		XrResult StartColocationAdvertisement(const TArray<uint8>& Metadata, uint64& OutRequestId);
		XrResult StopColocationAdvertisement(uint64& OutRequestId);

	private:
		void InitOpenXRFunctions(XrInstance InInstance);

		bool bExtColocationDiscoveryEnabled;

		FOpenXRHMD* OpenXRHMD;
	};

} // namespace XRColocation

#undef LOCTEXT_NAMESPACE
