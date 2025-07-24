// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationXR.h"
#include "OpenXRCore.h"
#include "OpenXRHMD.h"
#include "IOpenXRHMDModule.h"
#include "OpenXR/OculusXROpenXRUtilities.h"
#include "OculusXRAnchorsModule.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRColocationUtil.h"
#include "OculusXRColocationEventDelegates.h"
#include "OculusXRColocationModule.h"

#define LOCTEXT_NAMESPACE "OculusXRColocation"

namespace XRColocation
{
	PFN_xrStartColocationDiscoveryMETA xrStartColocationDiscoveryMETA = nullptr;
	PFN_xrStopColocationDiscoveryMETA xrStopColocationDiscoveryMETA = nullptr;
	PFN_xrStartColocationAdvertisementMETA xrStartColocationAdvertisementMETA = nullptr;
	PFN_xrStopColocationAdvertisementMETA xrStopColocationAdvertisementMETA = nullptr;

	FColocationXR::FColocationXR()
		: bExtColocationDiscoveryEnabled(false)
		, OpenXRHMD(nullptr)
	{
	}

	FColocationXR::~FColocationXR()
	{
	}

	void FColocationXR::RegisterAsOpenXRExtension()
	{
		RegisterOpenXRExtensionModularFeature();
	}

	bool FColocationXR::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_META_COLOCATION_DISCOVERY_EXTENSION_NAME);
		return true;
	}

	bool FColocationXR::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		return true;
	}

	const void* FColocationXR::OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext)
	{
		if (InModule != nullptr)
		{
			bExtColocationDiscoveryEnabled = InModule->IsExtensionEnabled(XR_META_COLOCATION_DISCOVERY_EXTENSION_NAME);

			UE_LOG(LogOculusXRColocation, Log, TEXT("[Colocation] Extensions available"));
			UE_LOG(LogOculusXRColocation, Log, TEXT("			   Colocation Discovery: %hs"), bExtColocationDiscoveryEnabled ? "ENABLED" : "DISABLED");
		}

		return InNext;
	}

	const void* FColocationXR::OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext)
	{
		InitOpenXRFunctions(InInstance);

		const FName SystemName(TEXT("OpenXR"));
		const bool IsOpenXR = GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName);
		if (IsOpenXR)
		{
			OpenXRHMD = (FOpenXRHMD*)GEngine->XRSystem.Get();
		}

		return InNext;
	}

	void FColocationXR::OnDestroySession(XrSession InSession)
	{
		OpenXRHMD = nullptr;
	}

	void FColocationXR::OnEvent(XrSession InSession, const XrEventDataBaseHeader* InHeader)
	{
		if (OpenXRHMD == nullptr)
		{
			UE_LOG(LogOculusXRColocation, Log, TEXT("[FColocationXR::OnEvent] Receieved event but no HMD was present."));
			return;
		}

		if (InHeader->type == XR_TYPE_EVENT_DATA_START_COLOCATION_DISCOVERY_COMPLETE_META)
		{
			const XrEventDataStartColocationDiscoveryCompleteMETA* const event =
				reinterpret_cast<const XrEventDataStartColocationDiscoveryCompleteMETA*>(InHeader);

			UE_LOG(LogOculusXRColocation, Verbose, TEXT("[FColocationXR::OnEvent] XrEventDataStartColocationDiscoveryCompleteMETA"));
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    RequestId: %llu"), event->discoveryRequestId);
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    Result:    %d"), event->result);

			FOculusXRColocationEventDelegates::StartColocationDiscoveryComplete.Broadcast(event->discoveryRequestId, OculusXRColocation::GetResult(event->result));
		}
		else if (InHeader->type == XR_TYPE_EVENT_DATA_COLOCATION_DISCOVERY_RESULT_META)
		{
			const XrEventDataColocationDiscoveryResultMETA* const event =
				reinterpret_cast<const XrEventDataColocationDiscoveryResultMETA*>(InHeader);

			UE_LOG(LogOculusXRColocation, Verbose, TEXT("[FColocationXR::OnEvent] XrEventDataStartColocationDiscoveryCompleteMETA"));
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    RequestId: %s"), *FOculusXRUUID(event->advertisementUuid.data).ToString());

			TArray<uint8> metadata(event->buffer, event->bufferSize);
			FOculusXRColocationEventDelegates::ColocationDiscoveryResultAvailable.Broadcast(event->discoveryRequestId, event->advertisementUuid.data, metadata);
		}
		else if (InHeader->type == XR_TYPE_EVENT_DATA_COLOCATION_DISCOVERY_COMPLETE_META)
		{
			const XrEventDataColocationDiscoveryCompleteMETA* const event =
				reinterpret_cast<const XrEventDataColocationDiscoveryCompleteMETA*>(InHeader);

			UE_LOG(LogOculusXRColocation, Verbose, TEXT("[FColocationXR::OnEvent] XrEventDataColocationDiscoveryCompleteMETA"));
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    RequestId: %llu"), event->discoveryRequestId);
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    Result:    %d"), event->result);

			FOculusXRColocationEventDelegates::ColocationDiscoveryComplete.Broadcast(event->discoveryRequestId, OculusXRColocation::GetResult(event->result));
		}
		else if (InHeader->type == XR_TYPE_EVENT_DATA_STOP_COLOCATION_DISCOVERY_COMPLETE_META)
		{
			const XrEventDataStopColocationDiscoveryCompleteMETA* const event =
				reinterpret_cast<const XrEventDataStopColocationDiscoveryCompleteMETA*>(InHeader);

			UE_LOG(LogOculusXRColocation, Verbose, TEXT("[FColocationXR::OnEvent] XrEventDataStopColocationDiscoveryCompleteMETA"));
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    RequestId: %llu"), event->requestId);
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    Result:    %d"), event->result);

			FOculusXRColocationEventDelegates::StopColocationDiscoveryComplete.Broadcast(event->requestId, OculusXRColocation::GetResult(event->result));
		}
		else if (InHeader->type == XR_TYPE_EVENT_DATA_START_COLOCATION_ADVERTISEMENT_COMPLETE_META)
		{
			const XrEventDataStartColocationAdvertisementCompleteMETA* const event =
				reinterpret_cast<const XrEventDataStartColocationAdvertisementCompleteMETA*>(InHeader);

			UE_LOG(LogOculusXRColocation, Verbose, TEXT("[FColocationXR::OnEvent] XrEventDataStartColocationAdvertisementCompleteMETA"));
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    RequestId: %llu"), event->advertisementRequestId);
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    RequestId: %s"), *FOculusXRUUID(event->advertisementUuid.data).ToString());
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    Result:    %d"), event->result);

			FOculusXRColocationEventDelegates::StartColocationAdvertisementComplete.Broadcast(event->advertisementRequestId, event->advertisementUuid.data, OculusXRColocation::GetResult(event->result));
		}
		else if (InHeader->type == XR_TYPE_EVENT_DATA_COLOCATION_ADVERTISEMENT_COMPLETE_META)
		{
			const XrEventDataColocationAdvertisementCompleteMETA* const event =
				reinterpret_cast<const XrEventDataColocationAdvertisementCompleteMETA*>(InHeader);

			UE_LOG(LogOculusXRColocation, Verbose, TEXT("[FColocationXR::OnEvent] XrEventDataColocationAdvertisementCompleteMETA"));
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("					  	    RequestId: %llu"), event->advertisementRequestId);
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("					        Result:    %d"), event->result);

			FOculusXRColocationEventDelegates::StopColocationDiscoveryComplete.Broadcast(event->advertisementRequestId, OculusXRColocation::GetResult(event->result));
		}
		else if (InHeader->type == XR_TYPE_EVENT_DATA_STOP_COLOCATION_ADVERTISEMENT_COMPLETE_META)
		{
			const XrEventDataStopColocationAdvertisementCompleteMETA* const event =
				reinterpret_cast<const XrEventDataStopColocationAdvertisementCompleteMETA*>(InHeader);

			UE_LOG(LogOculusXRColocation, Verbose, TEXT("[FColocationXR::OnEvent] XrEventDataStopColocationAdvertisementCompleteMETA"));
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    RequestId: %llu"), event->requestId);
			UE_LOG(LogOculusXRColocation, Verbose, TEXT("						    Result:    %d"), event->result);

			FOculusXRColocationEventDelegates::StopColocationDiscoveryComplete.Broadcast(event->requestId, OculusXRColocation::GetResult(event->result));
		}
	}

	XrResult FColocationXR::StartColocationDiscovery(uint64& OutRequestId)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StartColocationDiscovery] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsColocationDiscoveryEnabled())
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StartColocationDiscovery] Colocation discovery extensions are unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrColocationDiscoveryStartInfoMETA info{ XR_TYPE_COLOCATION_DISCOVERY_START_INFO_META, nullptr };
		auto result = xrStartColocationDiscoveryMETA(OpenXRHMD->GetSession(), &info, (XrAsyncRequestIdFB*)&OutRequestId);
		if (!XR_SUCCEEDED(result))
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StartColocationDiscovery] Start colocation discovery failed. Result: %d"), result);
		}

		return result;
	}

	XrResult FColocationXR::StopColocationDiscovery(uint64& OutRequestId)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StopColocationDiscovery] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsColocationDiscoveryEnabled())
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StopColocationDiscovery] Colocation discovery extensions are unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrColocationDiscoveryStopInfoMETA info{ XR_TYPE_COLOCATION_DISCOVERY_STOP_INFO_META, nullptr };
		auto result = xrStopColocationDiscoveryMETA(OpenXRHMD->GetSession(), &info, (XrAsyncRequestIdFB*)&OutRequestId);
		if (!XR_SUCCEEDED(result))
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StopColocationDiscovery] Stop colocation discovery failed. Result: %d"), result);
		}

		return result;
	}

	XrResult FColocationXR::StartColocationAdvertisement(const TArray<uint8>& Metadata, uint64& OutRequestId)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StartColocationAdvertisement] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsColocationDiscoveryEnabled())
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StartColocationAdvertisement] Colocation discovery extensions are unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrColocationAdvertisementStartInfoMETA info{ XR_TYPE_COLOCATION_ADVERTISEMENT_START_INFO_META, nullptr };
		info.buffer = (uint8_t*)Metadata.GetData();
		info.bufferSize = Metadata.Num();

		auto result = xrStartColocationAdvertisementMETA(OpenXRHMD->GetSession(), &info, (XrAsyncRequestIdFB*)&OutRequestId);
		if (!XR_SUCCEEDED(result))
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StartColocationAdvertisement] Start colocation advertisement failed. Result: %d"), result);
		}

		return result;
	}

	XrResult FColocationXR::StopColocationAdvertisement(uint64& OutRequestId)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StopColocationAdvertisement] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsColocationDiscoveryEnabled())
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StopColocationAdvertisement] Colocation discovery extensions are unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrColocationAdvertisementStopInfoMETA info{ XR_TYPE_COLOCATION_ADVERTISEMENT_STOP_INFO_META, nullptr };
		auto result = xrStopColocationAdvertisementMETA(OpenXRHMD->GetSession(), &info, (XrAsyncRequestIdFB*)&OutRequestId);
		if (!XR_SUCCEEDED(result))
		{
			UE_LOG(LogOculusXRColocation, Warning, TEXT("[StopColocationAdvertisement] Stop colocation advertisement failed. Result: %d"), result);
		}

		return result;
	}

	void FColocationXR::InitOpenXRFunctions(XrInstance InInstance)
	{
		// XR_META_colocation_discovery
		if (IsColocationDiscoveryEnabled())
		{
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrStartColocationDiscoveryMETA", &xrStartColocationDiscoveryMETA);
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrStopColocationDiscoveryMETA", &xrStopColocationDiscoveryMETA);
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrStartColocationAdvertisementMETA", &xrStartColocationAdvertisementMETA);
			OculusXR::XRGetInstanceProcAddr(InInstance, "xrStopColocationAdvertisementMETA", &xrStopColocationAdvertisementMETA);
		}
	}

} // namespace XRColocation

#undef LOCTEXT_NAMESPACE
