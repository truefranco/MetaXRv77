// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAsyncRequestBase.h"
#include <Templates/SharedPointer.h>
#include <atomic>

namespace OculusXR
{
	class FAsyncRequestSystem
	{
	public:
		FAsyncRequestSystem();

		static OCULUSXRASYNCREQUEST_API FAsyncRequestBase::EventId GenerateEventId();

		template <typename RequestType, typename... TArgs>
		static TSharedPtr<RequestType> CreateRequest(TArgs&&... Args)
		{
			auto request = MakeShared<RequestType>(std::forward<TArgs>(Args)...);
			GetInstance().Requests.Add(request->GetEventId(), request);

			return request;
		}

		static void RemoveRequest(FAsyncRequestBase::EventId Id)
		{
			auto& systemInstance = GetInstance();
			auto foundPtr = systemInstance.Requests.Find(Id);
			if (foundPtr)
			{
				systemInstance.RequestIdToEventIdMap.Remove((*foundPtr)->GetRequestId());
				systemInstance.Requests.Remove(Id);
			}
		}

		template <typename RequestType>
		static TSharedPtr<RequestType> GetRequest(FAsyncRequestBase::EventId Id)
		{
			auto foundRequestPtr = GetInstance().Requests.Find(Id);
			if (foundRequestPtr != nullptr)
			{
				return StaticCastSharedPtr<RequestType>(*foundRequestPtr);
			}

			return nullptr;
		}

		template <typename RequestType>
		static TSharedPtr<RequestType> GetRequest(FAsyncRequestBase::RequestId Id)
		{
			auto& systemInstance = GetInstance();

			auto foundEventId = systemInstance.RequestIdToEventIdMap.Find(Id);
			if (foundEventId != nullptr)
			{
				auto foundRequestPtr = systemInstance.Requests.Find(*foundEventId);
				if (foundRequestPtr)
				{
					return StaticCastSharedPtr<RequestType>(*foundRequestPtr);
				}
			}

			return nullptr;
		}

		template <typename RequestType>
		static void SetRequestId(FAsyncRequestBase::EventId EventId, FAsyncRequestBase::RequestId RequestId)
		{
			GetInstance().RequestIdToEventIdMap.Add(RequestId, EventId);
		}

		template <typename RequestType>
		static void CompleteRequest(FAsyncRequestBase::EventId EventId, const typename RequestType::FResultType& Result)
		{
			auto& systemInstance = GetInstance();

			auto request = systemInstance.GetRequest<RequestType>(EventId);
			if (request.IsValid())
			{
				request->RequestCompleted(Result);
				systemInstance.RemoveRequest(EventId);
			}
		}

	private:
		static OCULUSXRASYNCREQUEST_API FAsyncRequestSystem& GetInstance();

		TMap<FAsyncRequestBase::EventId, TSharedPtr<FAsyncRequestBase>> Requests;
		TMap<FAsyncRequestBase::RequestId, FAsyncRequestBase::EventId> RequestIdToEventIdMap;
		std::atomic<uint64_t> EventIdCounter;
	};
} // namespace OculusXR
