// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include <Templates/TypeHash.h>

namespace OculusXR
{
	constexpr uint64_t INVALID_TASK_REQUEST_ID = 0;

	template <typename TResultEnum>
	struct FAsyncResultDefaultSuccess
	{
		bool operator()(TResultEnum Val) { return Val == TResultEnum::Success; }
	};

	template <typename TResultEnum, typename TValueType, typename TSuccess = FAsyncResultDefaultSuccess<TResultEnum>>
	struct FAsyncResult
	{
	public:
		~FAsyncResult() {}

		static FAsyncResult FromEmpty()
		{
			return FAsyncResult();
		}

		static FAsyncResult FromResult(TResultEnum Status, const TValueType& Value)
		{
			return FAsyncResult(Status, Value);
		}

		static FAsyncResult FromError(TResultEnum Status)
		{
			return FAsyncResult(Status, {});
		}

		TResultEnum GetStatus() const { return Status; }
		bool IsSuccess() const { return TSuccess()(Status); }
		const TValueType& GetValue() const
		{
			check(IsSuccess());
			return Value;
		}

	private:
		FAsyncResult() {}

		FAsyncResult(TResultEnum InStatus, const TValueType& InValue)
		{
			Status = InStatus;
			Value = InValue;
		}

		TResultEnum Status;
		TValueType Value;
	};

	// Base event class that the internal task system uses to reference task data
	struct FAsyncRequestBase
	{
	public:
		FAsyncRequestBase()
			: InternalEventId(EventId(0))
			, InternalRequestId(RequestId(INVALID_TASK_REQUEST_ID))
		{
			// The derived
		}

		struct EventId
		{
			EventId(uint64_t InId) { Id = InId; }

			uint64_t Id;

			bool operator==(const EventId& Rhs) const { return Id == Rhs.Id; }
			bool operator!=(const EventId& Rhs) const { return !operator==(Rhs); }
			friend uint32_t GetTypeHash(const EventId& X) { return GetTypeHash(X.Id); }
		};

		struct RequestId
		{
			RequestId(uint64_t InId) { Id = InId; }

			uint64_t Id;

			bool operator==(const RequestId& Rhs) const { return Id == Rhs.Id; }
			bool operator!=(const RequestId& Rhs) const { return !operator==(Rhs); }
			friend uint32_t GetTypeHash(const RequestId& X) { return GetTypeHash(X.Id); }
		};

		virtual ~FAsyncRequestBase() {}

		EventId GetEventId() const { return InternalEventId; }
		RequestId GetRequestId() const { return InternalRequestId; }

	protected:
		void SetEventId(EventId InEventId) { InternalEventId = InEventId; }
		void SetRequestId(RequestId InRequestId) { InternalRequestId = InRequestId; }

	private:
		EventId InternalEventId;	 // Unique identifier, set regardless of the request ID value
		RequestId InternalRequestId; // Request id returned from successfully starting an async xr method
	};
} // namespace OculusXR
