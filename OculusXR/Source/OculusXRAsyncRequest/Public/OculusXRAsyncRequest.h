// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRAsyncRequestBase.h"
#include "OculusXRAsyncRequestSystem.h"
#include "IOculusXRAsyncRequestModule.h"

namespace OculusXR
{
	// Templated event type that is derived from to create new requests types
	template <typename TDerived, typename TResultEnum, typename TValueType, typename TResultSuccess = FAsyncResultDefaultSuccess<TResultEnum>>
	struct FAsyncRequest : public FAsyncRequestBase
	{
	public:
		typedef TValueType FResultValueType;
		typedef TResultSuccess FResultSuccessFunctor;
		typedef FAsyncResult<TResultEnum, TValueType, TResultSuccess> FResultType;
		DECLARE_DELEGATE_OneParam(FCompleteDelegate, const FResultType&);

		FAsyncRequest()
			: RequestResult(FResultType::FromEmpty())
			, bIsComplete(false)
			, bHasStarted(false)
		{
			SetEventId(FAsyncRequestSystem::GenerateEventId());
		}

		virtual ~FAsyncRequest() {}

		// Call this to start the request
		void Execute()
		{
			// Validate we aren't starting the request more than once
			check(!bHasStarted);
			bHasStarted = true;

			// Start the async work
			OnInitRequest();

			// Check initial result
			if (!TResultSuccess()(InitialResultStatus))
			{
				// Request failed so fire complete now and remove it from the request system
				// Once it goes out of scope in the calling code the shared ptr will be destroyed
				FAsyncRequestSystem::CompleteRequest<TDerived>(GetEventId(), FResultType::FromError(InitialResultStatus));
				return;
			}

			FAsyncRequestSystem::SetRequestId<TDerived>(GetEventId(), GetRequestId());
		}

		// Bind a callback to the completion of the event
		void BindOnComplete(const FCompleteDelegate& Function) { OnComplete = Function; }

		bool IsComplete() const { return bIsComplete; }
		const FResultType& GetResult() const
		{
			// Result type isn't valid unless we have completed
			check(bIsComplete);
			return RequestResult;
		}

	protected:
		friend class FAsyncRequestSystem;

		void RequestCompleted(const FResultType& InResult)
		{
			bIsComplete = true;
			RequestResult = InResult;
			OnComplete.ExecuteIfBound(InResult);
			OnCompleteRequest(InResult);
		}

		// Called by the derived request to specify the result code that came from starting the async work.
		void SetInitialResult(TResultEnum ResultStatus)
		{
			InitialResultStatus = ResultStatus;
		}

		// Returns a valid request id if there is a valid result value
		OculusXR::FAsyncRequestBase::RequestId DetermineRequestId(TResultEnum Result, uint64 Id)
		{
			return FResultSuccessFunctor()(Result) ? OculusXR::FAsyncRequestBase::RequestId(Id) : OculusXR::FAsyncRequestBase::RequestId(OculusXR::INVALID_TASK_REQUEST_ID);
		}

		// Overload to do request initialization, call OVRP functions, etc..
		virtual void OnInitRequest() {}

		// Overload this request to clean up any remaining data if needed
		virtual void OnCompleteRequest(const FResultType& Result) {}

	private:
		FCompleteDelegate OnComplete;
		TResultEnum InitialResultStatus;
		FResultType RequestResult;
		bool bIsComplete;
		bool bHasStarted;
	};
} // namespace OculusXR
