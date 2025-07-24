// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "OculusXRHMDPrivate.h"

namespace XRPassthrough
{
	class FPassthroughLayer;
	typedef TSharedPtr<XRPassthrough::FPassthroughLayer, ESPMode::ThreadSafe> FPassthroughLayerPtr;
} // namespace XRPassthrough

namespace XRPassthrough
{
	//-------------------------------------------------------------------------------------------------
	// FDeferredDeletionQueue
	//-------------------------------------------------------------------------------------------------

	class FDeferredDeletionQueue
	{
	public:
		void AddOpenXRLayerToDeferredDeletionQueue(const FPassthroughLayerPtr& ptr);
		void HandleLayerDeferredDeletionQueue_RenderThread(bool bDeleteImmediately = false);

	private:
		struct DeferredDeletionEntry
		{
			enum class DeferredDeletionEntryType
			{
				OpenXRLayer
			};

			XRPassthrough::FPassthroughLayerPtr OpenXRLayer;

			uint32 FrameEnqueued;
			DeferredDeletionEntryType EntryType;
		};

		TArray<DeferredDeletionEntry> DeferredDeletionArray;
	};

} // namespace XRPassthrough
