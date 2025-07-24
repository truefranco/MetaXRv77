// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "openxr/OculusXRBodyTrackingXR.h"
#include "openxr/OculusXREyeTrackingXR.h"
#include "openxr/OculusXRFaceTrackingXR.h"

#include "OculusXRMovement.h"
#include "OculusXRMovementLiveLink.h"
#include "IOculusXRMovementModule.h"
#include "ILiveLinkSource.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

//-------------------------------------------------------------------------------------------------
// FOculusXRMovementModule
//-------------------------------------------------------------------------------------------------

typedef TSharedPtr<XRMovement::FBodyTrackingXR, ESPMode::ThreadSafe> FBodyTrackingXRPtr;
typedef TSharedPtr<XRMovement::FEyeTrackingXR, ESPMode::ThreadSafe> FEyeTrackingXRPtr;
typedef TSharedPtr<XRMovement::FFaceTrackingXR, ESPMode::ThreadSafe> FFaceTrackingXRPtr;

class FOculusXRMovementModule : public IOculusXRMovementModule
{
public:
	FOculusXRMovementModule();

	static inline FOculusXRMovementModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FOculusXRMovementModule>("OculusXRMovement");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/* Live link */
	virtual TSharedPtr<ILiveLinkSource> GetLiveLinkSource() override;
	virtual bool IsLiveLinkSourceValid() const override;
	virtual void AddLiveLinkSource() override;
	virtual void RemoveLiveLinkSource() override;

	FBodyTrackingXRPtr GetXrBodyTracker() { return BodyTrackingXR; }
	FEyeTrackingXRPtr GetXrEyeTracker() { return EyeTrackingXR; }
	FFaceTrackingXRPtr GetXrFaceTracker() { return FaceTrackingXR; }

private:
	FBodyTrackingXRPtr BodyTrackingXR;
	FEyeTrackingXRPtr EyeTrackingXR;
	FFaceTrackingXRPtr FaceTrackingXR;

	TSharedPtr<MetaXRMovement::LiveLinkSource> MovementSource{ nullptr };
};

#undef LOCTEXT_NAMESPACE
