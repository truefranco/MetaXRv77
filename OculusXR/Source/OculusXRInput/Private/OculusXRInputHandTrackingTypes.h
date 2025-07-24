// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "khronos/openxr/openxr.h"
#include "xr_linear.h"

#include "CoreMinimal.h"
#include "OculusXRInputFunctionLibrary.h"

namespace OculusXRInput
{
	const XrPosef XrLeftHandLegacyBindPoseThumb0 = {
		{ 0.375387, 0.424584, -0.007779, 0.823864 },
		{ 0.020069, 0.011554, -0.010497 }
	};
	const XrPosef XrRightHandLegacyBindPoseThumb0 = {
		{ 0.375387, 0.424584, -0.007779, 0.823864 },
		{ -0.020069, -0.011554, 0.010497 }
	};
	const XrPosef XrLeftHandLegacyBindPoseThumb1 = {
		{ 0.260230, 0.024331, 0.125678, 0.957023 },
		{ 0.024853, 0.000000, -0.000000 }
	};
	const XrPosef XrRightHandLegacyBindPoseThumb1 = {
		{ 0.260230, 0.024331, 0.125678, 0.957023 },
		{ -0.024853, 0.000000, 0.000000 }
	};
	const float SMALLEST_NON_DENORMAL = 1.1754943508222875e-038f; // ( 1U << 23 )
	const int MaxHandCount = 2;

	enum class EHandFinger
	{
		Thumb = 0,
		Index = 1,
		Middle = 2,
		Ring = 3,
		Pinky = 4,
		Count = 5,
	};

	enum class EHandBoneId
	{
		Invalid = -1,

		// hand bones
		Start = 0,
		WristRoot = Start + 0,	 // root frame of the hand, where the wrist is located
		ForearmStub = Start + 1, // frame for user's forearm
		Thumb0 = Start + 2,		 // thumb trapezium bone
		Thumb1 = Start + 3,		 // thumb metacarpal bone
		Thumb2 = Start + 4,		 // thumb proximal phalange bone
		Thumb3 = Start + 5,		 // thumb distal phalange bone
		Index1 = Start + 6,		 // index proximal phalange bone
		Index2 = Start + 7,		 // index intermediate phalange bone
		Index3 = Start + 8,		 // index distal phalange bone
		Middle1 = Start + 9,	 // middle proximal phalange bone
		Middle2 = Start + 10,	 // middle intermediate phalange bone
		Middle3 = Start + 11,	 // middle distal phalange bone
		Ring1 = Start + 12,		 // ring proximal phalange bone
		Ring2 = Start + 13,		 // ring intermediate phalange bone
		Ring3 = Start + 14,		 // ring distal phalange bone
		Pinky0 = Start + 15,	 // pinky metacarpal bone
		Pinky1 = Start + 16,	 // pinky proximal phalange bone
		Pinky2 = Start + 17,	 // pinky intermediate phalange bone
		Pinky3 = Start + 18,	 // pinky distal phalange bone
		MaxSkinnable = Start + 19,
		// Bone tips are position only. They are not used for skinning but useful for hit-testing.
		// NOTE: ThumbTip == MaxSkinnable since the extended tips need to be contiguous
		ThumbTip = MaxSkinnable + 0,  // tip of the thumb
		IndexTip = MaxSkinnable + 1,  // tip of the index finger
		MiddleTip = MaxSkinnable + 2, // tip of the middle finger
		RingTip = MaxSkinnable + 3,	  // tip of the ring finger
		PinkyTip = MaxSkinnable + 4,  // tip of the pinky
		End = MaxSkinnable + 5,

		// Hand Skeleton V2 format
		StartV2 = 0,
		PalmV2 = StartV2 + 0,				 // PALM = 0,
		WristV2 = StartV2 + 1,				 // WRIST = 1,
		ThumbMetacarpalV2 = StartV2 + 2,	 // THUMB_METACARPAL = 2,
		ThumbProximalV2 = StartV2 + 3,		 // THUMB_PROXIMAL = 3,
		ThumbDistalV2 = StartV2 + 4,		 // THUMB_DISTAL = 4,
		ThumbTipV2 = StartV2 + 5,			 // THUMB_TIP = 5,
		IndexMetacarpalV2 = StartV2 + 6,	 // INDEX_METACARPAL = 6,
		IndexProximalV2 = StartV2 + 7,		 // INDEX_PROXIMAL = 7,
		IndexIntermediateV2 = StartV2 + 8,	 // INDEX_INTERMEDIATE = 8,
		IndexDistalV2 = StartV2 + 9,		 // INDEX_DISTAL = 9,
		IndexTipV2 = StartV2 + 10,			 // INDEX_TIP = 10,
		MiddleMetacarpalV2 = StartV2 + 11,	 // MIDDLE_METACARPAL = 11,
		MiddleProximalV2 = StartV2 + 12,	 // MIDDLE_PROXIMAL = 12,
		MiddleIntermediateV2 = StartV2 + 13, // MIDDLE_INTERMEDIATE = 13,
		MiddleDistalV2 = StartV2 + 14,		 // MIDDLE_DISTAL = 14,
		MiddleTipV2 = StartV2 + 15,			 // MIDDLE_TIP = 15,
		RingMetacarpalV2 = StartV2 + 16,	 // RING_METACARPAL = 16,
		RingProximalV2 = StartV2 + 17,		 // RING_PROXIMAL = 17,
		RingIntermediateV2 = StartV2 + 18,	 // RING_INTERMEDIATE = 18,
		RingDistalV2 = StartV2 + 19,		 // RING_DISTAL = 19,
		RingTipV2 = StartV2 + 20,			 // RING_TIP = 20,
		LittleMetacarpalV2 = StartV2 + 21,	 // LITTLE_METACARPAL = 21,
		LittleProximalV2 = StartV2 + 22,	 // LITTLE_PROXIMAL = 22,
		LittleIntermediateV2 = StartV2 + 23, // LITTLE_INTERMEDIATE = 23,
		LittleDistalV2 = StartV2 + 24,		 // LITTLE_DISTAL = 24,
		LittleTipV2 = StartV2 + 25,			 // LITTLE_TIP = 25,
		EndV2 = StartV2 + 26,

		// add other skeleton bone definitions here...
		Max = EndV2,
	};

	enum class EHandFingerPinch
	{
		Thumb = (1 << static_cast<int32>(EHandFinger::Thumb)),
		Index = (1 << static_cast<int32>(EHandFinger::Index)),
		Middle = (1 << static_cast<int32>(EHandFinger::Middle)),
		Ring = (1 << static_cast<int32>(EHandFinger::Ring)),
		Pinky = (1 << static_cast<int32>(EHandFinger::Pinky)),
		Max,
	};

	enum class EHandSkeletonConstants
	{
		MaxHandBones = static_cast<int32>(EHandBoneId::End),
		MaxHandBones_V2 = static_cast<int32>(EHandBoneId::EndV2),
		MaxBones = static_cast<int32>(EHandBoneId::Max),
		MaxBoneCapsules = 19,
	};

	struct FHandBoneCapsule
	{
		short BoneIndex;
		// Points at either end of the cylinder inscribed in the capsule. Also the center points for
		// spheres at either end of the capsule. Points A and B in the diagram above.
		XrVector3f Points[2];
		// The radius of the capsule cylinder and of the half-sphere caps on the ends of the capsule.
		float Radius;
	};

	struct FHandBone
	{
		EHandBoneId BoneId;
		// index of this bone's parent bone (-1 if no parent)
		short ParentBoneIndex;
		XrPosef Pose;
	};

	struct FHandSkeleton
	{
		EOculusXRHandType HandType;
		unsigned int NumBones;
		unsigned int NumBoneCapsules;
		FHandBone Bones[static_cast<int32>(EHandSkeletonConstants::MaxBones)];
		FHandBoneCapsule BoneCapsules[static_cast<int32>(EHandSkeletonConstants::MaxBoneCapsules)];
	};

	enum class EHandMeshConstants
	{
		MaxVertices = 3000,
		MaxIndices = MaxVertices * 6,
	};

	struct FHandMesh
	{
		// Type of mesh this data describes.
		EOculusXRHandType HandType;
		// Number of unique vertices in the mesh.
		unsigned int NumVertices;
		// Number of unique indices in the mesh.
		unsigned int NumIndices;
		// An array of count NumVertices positions for each vertex. Always valid.
		XrVector3f VertexPositions[static_cast<int32>(EHandMeshConstants::MaxVertices)];
		// An array of count NumIndices of vertex indices specifying triangles that make up the mesh. Always valid.
		short Indices[static_cast<int32>(EHandMeshConstants::MaxIndices)];
		// An array of count NumVertices of normals for each vertex.
		// If null, this attribute is not used.
		XrVector3f VertexNormals[static_cast<int32>(EHandMeshConstants::MaxVertices)];
		// An array of count NumVertices of texture coordinates for each vertex.
		// If null, this attribute is not used.
		XrVector2f VertexUV0[static_cast<int32>(EHandMeshConstants::MaxVertices)];
		// An array of count NumVertices of blend indices for each of the bones that each vertex is weighted to.
		// Always valid. An index of < 0 means no blend weight.
		XrVector4sFB BlendIndices[static_cast<int32>(EHandMeshConstants::MaxVertices)];
		// An array of count NumVertices of weights for each of the bones affecting each vertex. Always valid.
		XrVector4f BlendWeights[static_cast<int32>(EHandMeshConstants::MaxVertices)];
	};

	enum class EHandStatus
	{
		HandTracked = (1 << 0),				// hand is currently tracked by hand tracking
		InputValid = (1 << 1),				// if this is set the pointer pose and pinch data is usable
		SystemGestureInProgress = (1 << 6), // if this is set the user is performing the system gesture
		DominantHand = (1 << 7),			// if this is set the hand is considered the dominant hand
		MenuPressed = (1 << 8),				// if this is set the hand performed the system gesture as the non-dominant hand
	};

	struct FInternalHandState
	{
		// Hand Status bitfield described by ovrpHandStatus flags.
		unsigned int Status;

		// Root pose of the hand in world space. Not to be confused with the root bone's transform.
		// The root bone can still be offset from this by the skeleton's rest pose.
		XrPosef RootPose;

		// Current rotation of each bone.
		XrQuaternionf BoneRotations[static_cast<int32>(EHandSkeletonConstants::MaxHandBones)];

		// Provides a bitmask indicating if each finger is "pinched" or not. Indexable via bitshifting with the ovrpHandFinger
		// enum i.e. (1 << ovrpHandFinger_Index)
		unsigned int Pinches;

		// Provides a 0.0f to 1.0f value of how "pinched" each finger is. Indexable via the ovrpHandFinger enum.
		float PinchStrength[static_cast<int32>(EHandFinger::Count)];

		// World space position and translation of the pointer attached to the hand.
		XrPosef PointerPose;

		float HandScale;

		// Tracking confidence. Range [0,1], 0.0 = lowest confidence, 1.0 = highest confidence.
		// This is useful for smoothly de-emphasizing hands as confidence decreases.
		// This is the amount of confidence that the system has that the entire hand pose is correct.
		EOculusXRTrackingConfidence HandConfidence;

		// Per-finger tracking confidence. Range [0,1], 0.0 = lowest confidence, 1.0 = highest confidence.
		// This is the amount of confidence the system has that the individual finger poses are correct.
		EOculusXRTrackingConfidence FingerConfidences[static_cast<int32>(EHandFinger::Count)];

		// Time stamp for the pose that was requested in global system time.
		double RequestedTimeStamp;

		// Time stamp of the captured sample that the pose was extrapolated from.
		double SampleTimeStamp;

		bool PosesGeneratedByControllerData;
	};

	struct FInternalHandsState
	{
		FInternalHandState HandState[MaxHandCount];
		XrTime PredictedDisplayTime;
		bool bHandMenuButtonPressed = false;
	};

} // namespace OculusXRInput
