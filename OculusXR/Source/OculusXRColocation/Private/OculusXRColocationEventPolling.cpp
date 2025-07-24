// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationEventPolling.h"
#include "OculusXRColocationModule.h"
#include "OculusXRColocationRequests.h"
#include "OculusXRColocationSubsystem.h"
#include "OculusXRColocationUtil.h"
#include "OculusXRColocationEventDelegates.h"
#include "OculusXRHMDPrivate.h"
#include "OculusXRHMDModule.h"
#include "Engine/Engine.h"

namespace OculusXRColocation
{
	template <typename T>
	void GetEventDataAs(ovrpEventDataBuffer& Buffer, T& OutEventData)
	{
		memcpy(&OutEventData, reinterpret_cast<uint8*>(&Buffer), sizeof(T));
	}

	void FColocationEventPolling::OnPollEvent(ovrpEventDataBuffer* EventDataBuffer, bool& EventPollResult)
	{
		ovrpEventDataBuffer& buf = *EventDataBuffer;

		EventPollResult = true;

		switch (buf.EventType)
		{
			case ovrpEventType_StartColocationAdvertisementComplete:
			{
				ovrpEventStartColocationAdvertisementComplete eventData;
				GetEventDataAs(buf, eventData);

				UE_LOG(LogOculusXRColocation, Log, TEXT("ovrpEventStartColocationAdvertisementComplete: Request ID: %llu  --  SessionUuid: %s  --  Result: %s"),
					eventData.AdvertisementRequestId,
					*FOculusXRUUID(eventData.AdvertisementUuid.data).ToString(),
					*ToString(GetResult(eventData.Result)));

				FOculusXRColocationEventDelegates::StartColocationAdvertisementComplete.Broadcast(eventData.AdvertisementRequestId, eventData.AdvertisementUuid.data, GetResult(eventData.Result));

				break;
			}
			case ovrpEventType_ColocationAdvertisementComplete:
			{
				ovrpEventColocationAdvertisementComplete eventData;
				GetEventDataAs(buf, eventData);

				UE_LOG(LogOculusXRColocation, Log, TEXT("ovrpEventColocationAdvertisementComplete: Request ID: %llu  --  Result: %s"),
					eventData.AdvertisementRequestId,
					*ToString(GetResult(eventData.Result)));

				FOculusXRColocationEventDelegates::ColocationAdvertisementComplete.Broadcast(eventData.AdvertisementRequestId, GetResult(eventData.Result));

				break;
			}
			case ovrpEventType_StopColocationAdvertisementComplete:
			{
				ovrpEventStopColocationAdvertisementComplete eventData;
				GetEventDataAs(buf, eventData);

				FOculusXRColocationEventDelegates::StopColocationAdvertisementComplete.Broadcast(eventData.RequestId, GetResult(eventData.Result));

				break;
			}
			case ovrpEventType_StartColocationDiscoveryComplete:
			{
				ovrpEventStartColocationDiscoveryComplete eventData;
				GetEventDataAs(buf, eventData);

				UE_LOG(LogOculusXRColocation, Log, TEXT("ovrpEventStartColocationDiscoveryComplete: Request ID: %llu  --  Result: %s"),
					eventData.DiscoveryRequestId,
					*ToString(GetResult(eventData.Result)));

				FOculusXRColocationEventDelegates::StartColocationDiscoveryComplete.Broadcast(eventData.DiscoveryRequestId, GetResult(eventData.Result));

				break;
			}
			case ovrpEventType_ColocationDiscoveryResult:
			{
				ovrpEventColocationDiscoveryResult eventData;
				GetEventDataAs(buf, eventData);

				UE_LOG(LogOculusXRColocation, Log, TEXT("ovrpEventColocationDiscoveryResult: Request ID: %llu  --  FoundSessionUuid: %s"),
					eventData.DiscoveryRequestId,
					*FOculusXRUUID(eventData.AdvertisementUuid.data).ToString());

				TArray<uint8> metaData(eventData.Buffer, eventData.BufferSize);
				FOculusXRColocationEventDelegates::ColocationDiscoveryResultAvailable.Broadcast(eventData.DiscoveryRequestId, eventData.AdvertisementUuid.data, metaData);

				break;
			}
			case ovrpEventType_ColocationDiscoveryComplete:
			{
				ovrpEventColocationDiscoveryComplete eventData;
				GetEventDataAs(buf, eventData);

				UE_LOG(LogOculusXRColocation, Log, TEXT("ovrpEventColocationDiscoveryComplete: Request ID: %llu  --  Result: %s"),
					eventData.DiscoveryRequestId,
					*ToString(GetResult(eventData.Result)));

				FOculusXRColocationEventDelegates::ColocationDiscoveryComplete.Broadcast(eventData.DiscoveryRequestId, GetResult(eventData.Result));

				break;
			}
			case ovrpEventType_StopColocationDiscoveryComplete:
			{
				ovrpEventStopColocationDiscoveryComplete eventData;
				GetEventDataAs(buf, eventData);

				UE_LOG(LogOculusXRColocation, Log, TEXT("ovrpEventType_StopColocationDiscoveryComplete: Request ID: %llu  --  Result: %s"),
					eventData.RequestId,
					*ToString(GetResult(eventData.Result)));

				FOculusXRColocationEventDelegates::ColocationDiscoveryComplete.Broadcast(eventData.RequestId, GetResult(eventData.Result));

				// no-op
				break;
			}

			default:
			{
				EventPollResult = false;
				return;
			}
		}
	}
} // namespace OculusXRColocation
