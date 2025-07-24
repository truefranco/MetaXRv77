// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRBodytrackingXR.h"
#include "OpenXRCore.h"
#include "IOpenXRHMDModule.h"
#include "OpenXRHMD.h"
#include "OculusXRMovementLog.h"
#include "OpenXR/OculusXROpenXRUtilities.h"

#define LOCTEXT_NAMESPACE "OculusXRMovement"

namespace XRMovement
{
	PFN_xrCreateBodyTrackerFB xrCreateBodyTrackerFB = nullptr;
	PFN_xrDestroyBodyTrackerFB xrDestroyBodyTrackerFB = nullptr;
	PFN_xrLocateBodyJointsFB xrLocateBodyJointsFB = nullptr;
	PFN_xrGetBodySkeletonFB xrGetBodySkeletonFB = nullptr;
	PFN_xrRequestBodyTrackingFidelityMETA xrRequestBodyTrackingFidelityMETA = nullptr;
	PFN_xrSuggestBodyTrackingCalibrationOverrideMETA xrSuggestBodyTrackingCalibrationOverrideMETA = nullptr;
	PFN_xrResetBodyTrackingCalibrationMETA xrResetBodyTrackingCalibrationMETA = nullptr;

	FBodyTrackingXR::FBodyTrackingXR()
		: bExtBodyTrackingEnabled(false)
		, bExtBodyTrackingFullBodyEnabled(false)
		, bExtBodyTrackingFidelityEnabled(false)
		, bExtBodyTrackingCalibrationEnabled(false)
		, OpenXRHMD(nullptr)
		, BodyTracker(nullptr)
		, FullBodyTracking(false)
	{
	}

	FBodyTrackingXR::~FBodyTrackingXR()
	{
	}

	void FBodyTrackingXR::RegisterAsOpenXRExtension()
	{
		RegisterOpenXRExtensionModularFeature();
	}

	bool FBodyTrackingXR::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_FB_BODY_TRACKING_EXTENSION_NAME);
		return true;
	}

	bool FBodyTrackingXR::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_META_BODY_TRACKING_FULL_BODY_EXTENSION_NAME);
		OutExtensions.Add(XR_META_BODY_TRACKING_FIDELITY_EXTENSION_NAME);
		OutExtensions.Add(XR_META_BODY_TRACKING_CALIBRATION_EXTENSION_NAME);
		return true;
	}

	const void* FBodyTrackingXR::OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext)
	{
		if (InModule != nullptr)
		{
			bExtBodyTrackingEnabled = InModule->IsExtensionEnabled(XR_FB_BODY_TRACKING_EXTENSION_NAME);
			bExtBodyTrackingFullBodyEnabled = InModule->IsExtensionEnabled(XR_META_BODY_TRACKING_FULL_BODY_EXTENSION_NAME);
			bExtBodyTrackingFidelityEnabled = InModule->IsExtensionEnabled(XR_META_BODY_TRACKING_FIDELITY_EXTENSION_NAME);
			bExtBodyTrackingCalibrationEnabled = InModule->IsExtensionEnabled(XR_META_BODY_TRACKING_CALIBRATION_EXTENSION_NAME);

			UE_LOG(LogOculusXRMovement, Log, TEXT("[Body Tracking] Extensions available: Tracking: %hs -- Full Body: %hs -- Fidelity: %hs -- Calibration: %hs"),
				bExtBodyTrackingEnabled ? "ENABLED" : "DISABLED",
				bExtBodyTrackingFullBodyEnabled ? "ENABLED" : "DISABLED",
				bExtBodyTrackingFidelityEnabled ? "ENABLED" : "DISABLED",
				bExtBodyTrackingCalibrationEnabled ? "ENABLED" : "DISABLED");
		}
		return InNext;
	}

	const void* FBodyTrackingXR::OnCreateSession(XrInstance InInstance, XrSystemId InSystem, const void* InNext)
	{
		InitOpenXRFunctions(InInstance);

		OpenXRHMD = (FOpenXRHMD*)GEngine->XRSystem.Get();

		return InNext;
	}

	void FBodyTrackingXR::OnDestroySession(XrSession InSession)
	{
		OpenXRHMD = nullptr;
	}

	void FBodyTrackingXR::OnBeginRendering_GameThread(XrSession InSession, FSceneViewFamily& InViewFamily, TArrayView<const uint32> VisibleLayers)
	{
		Update_GameThread(InSession);
	}

	XrResult FBodyTrackingXR::StartBodyTracking()
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartBodyTracking] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsBodyTrackingSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartBodyTracking] Body tracking is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (BodyTracker != XR_NULL_HANDLE)
		{
			UE_LOG(LogOculusXRMovement, Log, TEXT("[StartBodyTracking] Body tracking is already started."));
			return XR_SUCCESS;
		}

		XrBodyTrackerCreateInfoFB createInfo = { XR_TYPE_BODY_TRACKER_CREATE_INFO_FB };
		createInfo.next = nullptr;
		createInfo.bodyJointSet = XR_BODY_JOINT_SET_DEFAULT_FB;

		auto result = XRMovement::xrCreateBodyTrackerFB(OpenXRHMD->GetSession(), &createInfo, &BodyTracker);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartBodyTracking] Body tracking failed to start. Result: %d"), result);
			return result;
		}

		return XR_SUCCESS;
	}

	XrResult FBodyTrackingXR::StartBodyTrackingByJointSet(EOculusXRBodyJointSet jointSet)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartBodyTrackingByJointSet] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsBodyTrackingSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartBodyTrackingByJointSet] Body tracking is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (BodyTracker != XR_NULL_HANDLE)
		{
			UE_LOG(LogOculusXRMovement, Log, TEXT("[StartBodyTrackingByJointSet] Body tracking is already started."));
			return XR_SUCCESS;
		}

		XrBodyTrackerCreateInfoFB createInfo = { XR_TYPE_BODY_TRACKER_CREATE_INFO_FB };
		createInfo.next = nullptr;

		switch (jointSet)
		{
			case EOculusXRBodyJointSet::UpperBody:
				createInfo.bodyJointSet = XR_BODY_JOINT_SET_DEFAULT_FB;
				break;
			case EOculusXRBodyJointSet::FullBody:
				createInfo.bodyJointSet = XR_BODY_JOINT_SET_FULL_BODY_META;
				if (!IsFullBodySupported())
				{
					UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartBodyTrackingByJointSet] Full body tracking is unsupported."));
					return XR_ERROR_VALIDATION_FAILURE;
				}
				break;
			default:
				UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartBodyTrackingByJointSet] Unknown body tracking joint set."));
				return XR_ERROR_VALIDATION_FAILURE;
		}

		auto result = XRMovement::xrCreateBodyTrackerFB(OpenXRHMD->GetSession(), &createInfo, &BodyTracker);
		if XR_FAILED (result)
		{
			BodyTracker = XR_NULL_HANDLE;
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StartBodyTrackingByJointSet] Body tracking failed to start. Result: %d"), result);
			return result;
		}
		else
		{
			FullBodyTracking = (jointSet == EOculusXRBodyJointSet::FullBody);
		}

		return XR_SUCCESS;
	}

	XrResult FBodyTrackingXR::StopBodyTracking()
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopBodyTracking] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsBodyTrackingSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopBodyTracking] Body tracking is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		XrResult result = XR_SUCCESS;
		if (IsBodyTrackingEnabled())
		{
			result = XRMovement::xrDestroyBodyTrackerFB(BodyTracker);
			if XR_FAILED (result)
			{
				UE_LOG(LogOculusXRMovement, Warning, TEXT("[StopBodyTracking] Body tracking failed to stop. Result: %d"), result);
			}
		}

		BodyTracker = XR_NULL_HANDLE;
		FullBodyTracking = false;

		return result;
	}

	XrResult FBodyTrackingXR::GetCachedBodyState(FOculusXRBodyState& OutState)
	{
		if (!IsBodyTrackingEnabled())
		{
			return XR_ERROR_VALIDATION_FAILURE;
		}

		OutState = CachedBodyState;
		return XR_SUCCESS;
	}

	XrResult FBodyTrackingXR::GetBodySkeleton(FOculusXRBodySkeleton& OutSkeleton)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[GetBodySkeleton] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		int jointCount = IsFullBodyTrackingEnabled() ? (int)XR_FULL_BODY_JOINT_COUNT_META : (int)XR_BODY_JOINT_COUNT_FB;

		// Allocate enough memory for the larger joint set
		static_assert((int)XR_FULL_BODY_JOINT_COUNT_META >= (int)XR_BODY_JOINT_COUNT_FB);
		XrBodySkeletonJointFB joints[XR_FULL_BODY_JOINT_COUNT_META];

		XrBodySkeletonFB bodySkeleton = { XR_TYPE_BODY_SKELETON_FB };
		bodySkeleton.jointCount = jointCount;
		bodySkeleton.joints = joints;

		auto result = XRMovement::xrGetBodySkeletonFB(BodyTracker, &bodySkeleton);
		if (XR_FAILED(result))
		{
			return result;
		}

		OutSkeleton.NumBones = bodySkeleton.jointCount;
		for (uint32 i = 0; i < bodySkeleton.jointCount; ++i)
		{
			XrBodySkeletonJointFB bone = bodySkeleton.joints[i];
			XrPosef bonePose = bone.pose;

			FOculusXRBodySkeletonBone& OculusXRBone = OutSkeleton.Bones[i];

			OculusXRBone.Orientation = FRotator(ToFQuat(bonePose.orientation));
			OculusXRBone.Position = ToFVector(bonePose.position) * OpenXRHMD->GetWorldToMetersScale();

			if (bone.parentJoint == XR_BODY_JOINT_NONE_FB)
			{
				OculusXRBone.ParentBoneIndex = EOculusXRBoneID::None;
			}
			else
			{
				OculusXRBone.ParentBoneIndex = static_cast<EOculusXRBoneID>(bone.parentJoint);
			}

			OculusXRBone.BoneId = static_cast<EOculusXRBoneID>(bone.joint);
		}

		return XR_SUCCESS;
	}

	XrResult FBodyTrackingXR::RequestBodyTrackingFidelity(EOculusXRBodyTrackingFidelity Fidelity)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[RequestBodyTrackingFidelity] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsFidelitySupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[RequestBodyTrackingFidelity] Fidelity is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (BodyTracker == XR_NULL_HANDLE)
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[RequestBodyTrackingFidelity] Body tracking is not started."));
			return XR_SUCCESS;
		}

		XrBodyTrackingFidelityMETA fidelity;
		switch (Fidelity)
		{
			case EOculusXRBodyTrackingFidelity::High:
				fidelity = XR_BODY_TRACKING_FIDELITY_HIGH_META;
				break;
			case EOculusXRBodyTrackingFidelity::Low:
				fidelity = XR_BODY_TRACKING_FIDELITY_LOW_META;
				break;
			default:
				UE_LOG(LogOculusXRMovement, Warning, TEXT("[RequestBodyTrackingFidelity] Invalid fidelity level."));
				return XR_ERROR_VALIDATION_FAILURE;
		}

		XrResult result = xrRequestBodyTrackingFidelityMETA(BodyTracker, fidelity);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[RequestBodyTrackingFidelity] Failed to request fidelity level. Result: %d"), result);
		}

		return result;
	}

	XrResult FBodyTrackingXR::ResetBodyTrackingFidelity()
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[ResetBodyTrackingFidelity] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsFidelitySupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[ResetBodyTrackingFidelity] Fidelity is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (BodyTracker == XR_NULL_HANDLE)
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[ResetBodyTrackingFidelity] Body tracking is not started."));
			return XR_SUCCESS;
		}

		XrResult result = xrResetBodyTrackingCalibrationMETA(BodyTracker);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[ResetBodyTrackingFidelity] Failed to request fidelity level. Result: %d"), result);
		}

		return result;
	}

	XrResult FBodyTrackingXR::SuggestBodyTrackingCalibrationOverride(float height)
	{
		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[SuggestBodyTrackingCalibrationOverride] XR state is invalid."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (!IsCalibrationSupported())
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[SuggestBodyTrackingCalibrationOverride] Calibration is unsupported."));
			return XR_ERROR_VALIDATION_FAILURE;
		}

		if (BodyTracker == XR_NULL_HANDLE)
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[SuggestBodyTrackingCalibrationOverride] Body tracking is not started."));
			return XR_SUCCESS;
		}

		XrBodyTrackingCalibrationInfoMETA xrCalibrationInfo = { XR_TYPE_BODY_TRACKING_CALIBRATION_INFO_META };
		xrCalibrationInfo.bodyHeight = height;

		XrResult result = xrSuggestBodyTrackingCalibrationOverrideMETA(BodyTracker, &xrCalibrationInfo);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[SuggestBodyTrackingCalibrationOverride] failed to suggest calibration override! Result: %d"), result);
		}

		return result;
	}

	void FBodyTrackingXR::InitOpenXRFunctions(XrInstance InInstance)
	{
		// XR_FB_Body_Tracking
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrCreateBodyTrackerFB", &xrCreateBodyTrackerFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrDestroyBodyTrackerFB", &xrDestroyBodyTrackerFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrLocateBodyJointsFB", &xrLocateBodyJointsFB);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrGetBodySkeletonFB", &xrGetBodySkeletonFB);

		// XR_META_body_tracking_fidelity
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrRequestBodyTrackingFidelityMETA", &xrRequestBodyTrackingFidelityMETA);

		// XR_META_body_tracking_calibration
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrSuggestBodyTrackingCalibrationOverrideMETA", &xrSuggestBodyTrackingCalibrationOverrideMETA);
		OculusXR::XRGetInstanceProcAddr(InInstance, "xrResetBodyTrackingCalibrationMETA", &xrResetBodyTrackingCalibrationMETA);
	}

	void FBodyTrackingXR::Update_GameThread(XrSession InSession)
	{
		check(IsInGameThread());

		if (!OpenXRHMD || !OpenXRHMD->GetInstance() || !OpenXRHMD->GetSession() || !IsBodyTrackingSupported() || !IsBodyTrackingEnabled())
		{
			return;
		}

		static_assert(XR_FULL_BODY_JOINT_COUNT_META == static_cast<int>(EOculusXRBoneID::COUNT), "The size of the XR Bone ID enum should be the same as the EOculusXRBoneID count.");

		int jointCount = IsFullBodyTrackingEnabled() ? (int)XR_FULL_BODY_JOINT_COUNT_META : (int)XR_BODY_JOINT_COUNT_FB;
		CachedBodyState.Joints.SetNum(static_cast<int>(XR_FULL_BODY_JOINT_COUNT_META));

		XrBodyJointsLocateInfoFB info = { XR_TYPE_BODY_JOINTS_LOCATE_INFO_FB };
		info.baseSpace = OpenXRHMD->GetTrackingSpace();
		info.time = OpenXRHMD->GetDisplayTime();

		XrBodyJointLocationsFB locations = { XR_TYPE_BODY_JOINT_LOCATIONS_FB };
		XrBodyJointLocationFB jointLocations[XR_FULL_BODY_JOINT_COUNT_META];

		locations.jointCount = jointCount;
		locations.jointLocations = jointLocations;

		XrBodyTrackingCalibrationStatusMETA calibrationStatus = { XR_TYPE_BODY_TRACKING_CALIBRATION_STATUS_META };
		calibrationStatus.next = XR_NULL_HANDLE;
		if (IsCalibrationSupported())
		{
			OculusXR::XRAppendToChain(
				reinterpret_cast<XrBaseOutStructure*>(&calibrationStatus), reinterpret_cast<XrBaseOutStructure*>(&locations));
		}

		XrBodyTrackingFidelityStatusMETA fidelityStatus = { XR_TYPE_BODY_TRACKING_FIDELITY_STATUS_META };
		fidelityStatus.next = XR_NULL_HANDLE;
		if (IsFidelitySupported())
		{
			OculusXR::XRAppendToChain(
				reinterpret_cast<XrBaseOutStructure*>(&fidelityStatus), reinterpret_cast<XrBaseOutStructure*>(&locations));
		}

		auto result = XRMovement::xrLocateBodyJointsFB(BodyTracker, &info, &locations);
		if (XR_FAILED(result))
		{
			UE_LOG(LogOculusXRMovement, Warning, TEXT("[LocateBodyJoints] Failed to locate joints! Result: %d"), result);
			return;
		}

		CachedBodyState.IsActive = (bool)locations.isActive;
		CachedBodyState.Confidence = locations.confidence;
		CachedBodyState.SkeletonChangedCount = locations.skeletonChangedCount;
		CachedBodyState.Time = locations.time * 1e-9; // FromXrTime

		for (int i = 0; i < jointCount; ++i)
		{
			XrBodyJointLocationFB jointLocation = locations.jointLocations[i];
			XrPosef jointPose = jointLocation.pose;

			FOculusXRBodyJoint& OculusXRBodyJoint = CachedBodyState.Joints[i];
			OculusXRBodyJoint.LocationFlags = jointLocation.locationFlags;
			OculusXRBodyJoint.bIsValid = jointLocation.locationFlags & (XRSpaceFlags::XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XRSpaceFlags::XR_SPACE_LOCATION_POSITION_VALID_BIT);
			OculusXRBodyJoint.Orientation = FRotator(ToFQuat(jointPose.orientation));
			OculusXRBodyJoint.Position = ToFVector(jointPose.position) * OpenXRHMD->GetWorldToMetersScale();
		}

		// If using less joints than the max count we can just set the remaining joints to null
		if (jointCount < CachedBodyState.Joints.Num())
		{
			for (int i = jointCount; i < CachedBodyState.Joints.Num(); ++i)
			{
				CachedBodyState.Joints[i].bIsValid = false;
			}
		}
	}

} // namespace XRMovement

#undef LOCTEXT_NAMESPACE
