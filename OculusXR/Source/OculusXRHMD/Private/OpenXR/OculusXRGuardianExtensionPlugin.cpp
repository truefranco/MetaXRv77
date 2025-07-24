// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRGuardianExtensionPlugin.h"
#include "Engine/GameEngine.h"
#include "IXRTrackingSystem.h"
#include "OculusXROpenXRUtilities.h"
#include "OpenXRCore.h"

DEFINE_LOG_CATEGORY(LogOculusXRGuardian);

namespace OculusXR
{
	void FGuardianExtensionPlugin::PostCreateSession(XrSession InSession)
	{
		Session = InSession;
	}

	bool FGuardianExtensionPlugin::IsGuardianConfigured()
	{
		XrExtent2Df Extent;
		const XrResult Result = xrGetReferenceSpaceBoundsRect(Session, XR_REFERENCE_SPACE_TYPE_STAGE, &Extent);
		return (XR_SUCCEEDED(Result) && (Result != XR_SPACE_BOUNDS_UNAVAILABLE));
	}

	bool FGuardianExtensionPlugin::GetGuardianPoints(TArray<FVector>& BoundaryPoints)
	{
		XrExtent2Df Extent;
		XrResult Result = xrGetReferenceSpaceBoundsRect(Session, XR_REFERENCE_SPACE_TYPE_STAGE, &Extent);
		if (!XR_SUCCEEDED(Result) || Result == XR_SPACE_BOUNDS_UNAVAILABLE)
		{
			return false;
		}

		IXRTrackingSystem* TrackingSystem = GetOpenXRTrackingSystem();
		if (TrackingSystem == nullptr)
		{
			return false;
		}

		FTransform OriginTransform;
		if (!TrackingSystem->GetTrackingOriginTransform(EHMDTrackingOrigin::Stage, OriginTransform))
		{
			return false;
		}

		const float WorldToMetersScale = TrackingSystem->GetWorldToMetersScale();

		const XrVector3f XrPoints[] = {
			{ -Extent.width * 0.5f, 0.0f, -Extent.height * 0.5 },
			{ -Extent.width * 0.5f, 0.0f, Extent.height * 0.5f },
			{ Extent.width * 0.5f, 0.0f, Extent.height * 0.5f },
			{ Extent.width * 0.5f, 0.0f, -Extent.height * 0.5f }
		};

		BoundaryPoints.Reset(UE_ARRAY_COUNT(XrPoints));
		for (int Index = 0; Index < UE_ARRAY_COUNT(XrPoints); ++Index)
		{
			BoundaryPoints.Emplace(OriginTransform.TransformPosition(ToFVector(XrPoints[Index], WorldToMetersScale)));
		}

		return true;
	}

	FVector FGuardianExtensionPlugin::GetGuardianDimensions()
	{
		XrExtent2Df Extent;
		FVector Dimensions;
		XrResult Result = xrGetReferenceSpaceBoundsRect(Session, XR_REFERENCE_SPACE_TYPE_STAGE, &Extent);
		if (XR_SUCCEEDED(Result) && Result == XR_SPACE_BOUNDS_UNAVAILABLE)
		{
			// Set height to 10.0f which matches expected OVRPlugin Version
			const float BoundsHeight = 10.0f;
			Dimensions = FVector(Extent.width, BoundsHeight, Extent.height);
		}
		return Dimensions;
	}

} // namespace OculusXR
