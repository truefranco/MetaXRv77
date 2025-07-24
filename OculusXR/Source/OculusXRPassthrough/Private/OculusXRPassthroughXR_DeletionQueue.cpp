// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRPassthroughXR_DeletionQueue.h"

#include "XRThreadUtils.h"
#include "OculusXRHMDModule.h"
#include "OculusXRPassthroughXRFunctions.h"
#include "OculusXRPassthroughLayer.h"

namespace XRPassthrough
{

	//-------------------------------------------------------------------------------------------------
	// FDeferredDeletionQueue
	//-------------------------------------------------------------------------------------------------
	const uint32 NUM_FRAMES_TO_WAIT_FOR_OPENXR_LAYER_DELETE = 7;

	void FDeferredDeletionQueue::AddOpenXRLayerToDeferredDeletionQueue(const XRPassthrough::FPassthroughLayerPtr& ptr)
	{
		DeferredDeletionEntry Entry;
		Entry.OpenXRLayer = ptr;
		Entry.FrameEnqueued = GFrameCounter;
		Entry.EntryType = DeferredDeletionEntry::DeferredDeletionEntryType::OpenXRLayer;
		DeferredDeletionArray.Add(Entry);
	}

	void FDeferredDeletionQueue::HandleLayerDeferredDeletionQueue_RenderThread(bool bDeleteImmediately)
	{
		// Traverse list backwards so the swap switches to elements already tested
		for (int32 Index = DeferredDeletionArray.Num() - 1; Index >= 0; --Index)
		{
			DeferredDeletionEntry* Entry = &DeferredDeletionArray[Index];
			if (Entry->EntryType == DeferredDeletionEntry::DeferredDeletionEntryType::OpenXRLayer)
			{
				if (bDeleteImmediately || GFrameCounter > Entry->FrameEnqueued + NUM_FRAMES_TO_WAIT_FOR_OPENXR_LAYER_DELETE)
				{
					Entry->OpenXRLayer->DestroyLayer_RenderThread();

#if UE_VERSION_OLDER_THAN(5, 5, 0)
					DeferredDeletionArray.RemoveAtSwap(Index, 1, false);
#else
					DeferredDeletionArray.RemoveAtSwap(Index, 1, EAllowShrinking::No);
#endif
				}
			}
		}
	}

} // namespace XRPassthrough
