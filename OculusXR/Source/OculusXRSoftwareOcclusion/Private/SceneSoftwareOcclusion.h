// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#ifdef WITH_OCULUS_BRANCH

#include "CoreMinimal.h"
#include "Async/TaskGraphInterfaces.h"
#include "ICustomOcclusion.h"

class FViewInfo;
struct FOcclusionFrameResults;
class FSceneSoftwareOcclusion : public ICustomOcclusion
{
public:
	FSceneSoftwareOcclusion();
	virtual ~FSceneSoftwareOcclusion();

	virtual int32 Process(const FScene* Scene, FViewInfo& View) override;
	virtual void DebugDraw(FRDGBuilder& GraphBuilder, const FViewInfo& View, FScreenPassRenderTarget Output, int32 InX, int32 InY) override;

private:
	void FlushResults();

	FGraphEventRef TaskRef;
	TUniquePtr<FOcclusionFrameResults> Available;
	TUniquePtr<FOcclusionFrameResults> Processing;
};
#endif // WITH_OCULUS_BRANCH
