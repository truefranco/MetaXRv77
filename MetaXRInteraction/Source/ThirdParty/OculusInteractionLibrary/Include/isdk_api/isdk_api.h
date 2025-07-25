// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
// @generated

#ifndef ISDK_API_H
#define ISDK_API_H

//-----------------------------------------------------------------------------------
// ***** INTERACTIONSDK_CDECL
//
/// calling convention for windows builds.
//
#if !defined(INTERACTIONSDK_CDECL)
#if defined(_WIN32)
#define INTERACTIONSDK_CDECL __cdecl
#else
#define INTERACTIONSDK_CDECL
#endif
#endif

//-----------------------------------------------------------------------------------
// ***** INTERACTIONSDK_EXTERN_C
//
/// Defined as extern "C" when built from C++ code.
//
#if !defined(INTERACTIONSDK_EXTERN_C)
#ifdef __cplusplus
#define INTERACTIONSDK_EXTERN_C extern "C"
#else
#define INTERACTIONSDK_EXTERN_C
#endif
#endif

//-----------------------------------------------------------------------------------
// ***** INTERACTIONSDK_EXTERN_C
//
/// Defined as extern "C" when built from C++ code.
//
#if !defined(INTERACTIONSDK_EXPORT)
#if defined(INTERACTIONSDK_DLL)
#if defined(_WIN32)
#define INTERACTIONSDK_EXPORT(rval) \
  INTERACTIONSDK_EXTERN_C __declspec(dllexport) rval INTERACTIONSDK_CDECL
#else
#define INTERACTIONSDK_EXPORT(rval) \
  INTERACTIONSDK_EXTERN_C           \
      __attribute__((visibility("default"))) rval INTERACTIONSDK_CDECL /* Requires GCC 4.0+ */
#endif
#elif defined(INTERACTIONSDK_STATIC_BUILD)
#define INTERACTIONSDK_EXPORT(rval) INTERACTIONSDK_EXTERN_C rval INTERACTIONSDK_CDECL
#else
#if defined(_WIN32)
#define INTERACTIONSDK_EXPORT(rval) \
  INTERACTIONSDK_EXTERN_C __declspec(dllimport) rval INTERACTIONSDK_CDECL
#else
#define INTERACTIONSDK_EXPORT(rval) INTERACTIONSDK_EXTERN_C rval INTERACTIONSDK_CDECL
#endif
#endif
#endif
enum isdk_AxisAlignedBox_BoxSide {
  isdk_AxisAlignedBox_BoxSide_Xmin = 0,
  isdk_AxisAlignedBox_BoxSide_Xmax = 1,
  isdk_AxisAlignedBox_BoxSide_Ymin = 2,
  isdk_AxisAlignedBox_BoxSide_Ymax = 3,
  isdk_AxisAlignedBox_BoxSide_Zmin = 4,
  isdk_AxisAlignedBox_BoxSide_Zmax = 5
};

enum isdk_DataSourceUpdateAttributeResult {
  isdk_DataSourceUpdateAttributeResult_Unknown = 0,
  isdk_DataSourceUpdateAttributeResult_Modified = 1,
  isdk_DataSourceUpdateAttributeResult_NotModified = 2,
  isdk_DataSourceUpdateAttributeResult_DoesNotExist = 3,
  isdk_DataSourceUpdateAttributeResult_WrongValueType = 4
};

enum isdk_DataSourceUpdateDataResult {
  isdk_DataSourceUpdateDataResult_Failure_InvalidPrevDataSource = -1,
  isdk_DataSourceUpdateDataResult_Unknown = 0,
  isdk_DataSourceUpdateDataResult_Modified = 1,
  isdk_DataSourceUpdateDataResult_NotModified = 2
};

enum isdk_Detection_FingerCalcType {
  isdk_Detection_FingerCalcType_Curl = 0,
  isdk_Detection_FingerCalcType_Flexion = 1,
  isdk_Detection_FingerCalcType_Abduction = 2,
  isdk_Detection_FingerCalcType_Opposition = 3,
  isdk_Detection_FingerCalcType_Grab = 4,
  isdk_Detection_FingerCalcType_OppositionTangentPlane = 5,
  isdk_Detection_FingerCalcType_OppositionNormal = 6,
  isdk_Detection_FingerCalcType_OppositionTopTwo = 7,
  isdk_Detection_FingerCalcType_OppositionTopThree = 8
};

enum isdk_Detection_ThumbCalcType {
  isdk_Detection_ThumbCalcType_Curl = 0,
  isdk_Detection_ThumbCalcType_Flexion = 1
};

enum isdk_DigitRecognizer_UpdateResult {
  isdk_DigitRecognizer_UpdateResult_Failure = -1,
  isdk_DigitRecognizer_UpdateResult_Unknown = 0,
  isdk_DigitRecognizer_UpdateResult_Success = 1,
  isdk_DigitRecognizer_UpdateResult_FrameDataUnavailable = 2,
  isdk_DigitRecognizer_UpdateResult_FrameDataIsStale = 3
};

enum isdk_FingerJoint {
  isdk_FingerJoint_Metacarpal = 0,
  isdk_FingerJoint_Proximal = 1,
  isdk_FingerJoint_Intermediate = 2,
  isdk_FingerJoint_Distal = 3,
  isdk_FingerJoint_Tip = 4
};

enum isdk_FingerType {
  isdk_FingerType_Index = 0,
  isdk_FingerType_Middle = 1,
  isdk_FingerType_Ring = 2,
  isdk_FingerType_Pinky = 3
};

enum isdk_Handedness { isdk_Handedness_Left = 0, isdk_Handedness_Right = 1 };

enum isdk_InteractableState {
  isdk_InteractableState_Normal = 0,
  isdk_InteractableState_Hover = 1,
  isdk_InteractableState_Select = 2,
  isdk_InteractableState_Disabled = 3
};

enum isdk_InteractorState {
  isdk_InteractorState_Normal = 0,
  isdk_InteractorState_Hover = 1,
  isdk_InteractorState_Select = 2,
  isdk_InteractorState_Disabled = 3
};

enum isdk_NormalFacing {
  isdk_NormalFacing_Any = 0,
  isdk_NormalFacing_In = 1,
  isdk_NormalFacing_Out = 2
};

enum isdk_OneEuroHandFilter_AttributeId {
  isdk_OneEuroHandFilter_AttributeId_Unknown = 0,
  isdk_OneEuroHandFilter_AttributeId_WristPosBeta = 1,
  isdk_OneEuroHandFilter_AttributeId_WristPosMinCutOff = 2,
  isdk_OneEuroHandFilter_AttributeId_WristRotBeta = 3,
  isdk_OneEuroHandFilter_AttributeId_WristRotMinCutOff = 4,
  isdk_OneEuroHandFilter_AttributeId_FingerRotBeta = 5,
  isdk_OneEuroHandFilter_AttributeId_FingerRotMinCutOff = 6,
  isdk_OneEuroHandFilter_AttributeId_Frequency = 7,
  isdk_OneEuroHandFilter_AttributeId_WristPosDeltaMinCutOff = 8,
  isdk_OneEuroHandFilter_AttributeId_WristRotDeltaMinCutOff = 9,
  isdk_OneEuroHandFilter_AttributeId_FingerRotDeltaMinCutOff = 10
};

enum isdk_PointerEventType {
  isdk_PointerEventType_Hover = 0,
  isdk_PointerEventType_Unhover = 1,
  isdk_PointerEventType_Select = 2,
  isdk_PointerEventType_Unselect = 3,
  isdk_PointerEventType_Move = 4,
  isdk_PointerEventType_Cancel = 5
};

enum isdk_SyntheticHand_WristLockMode {
  isdk_SyntheticHand_WristLockMode_Position = 1,
  isdk_SyntheticHand_WristLockMode_Rotation = 2,
  isdk_SyntheticHand_WristLockMode_Full = 3
};

enum isdk_ThumbJoint {
  isdk_ThumbJoint_Metacarpal = 0,
  isdk_ThumbJoint_Proximal = 1,
  isdk_ThumbJoint_Distal = 2,
  isdk_ThumbJoint_Tip = 3
};

typedef struct isdk_ActiveStateBool_ isdk_ActiveStateBool;
typedef struct isdk_ApiContext_ isdk_ApiContext;
typedef struct isdk_ClippedPlaneSurface_ isdk_ClippedPlaneSurface;
typedef struct isdk_CylinderSurface_ isdk_CylinderSurface;
typedef struct isdk_DigitRecognizer_ isdk_DigitRecognizer;
typedef struct isdk_DummyHandSource_ isdk_DummyHandSource;
typedef struct isdk_ExternalHandPositionFrame_ isdk_ExternalHandPositionFrame;
typedef struct isdk_ExternalHandSource_ isdk_ExternalHandSource;
typedef struct isdk_FingerPinchGrabRecognizer_ isdk_FingerPinchGrabRecognizer;
typedef struct isdk_FingerRecognizer_ isdk_FingerRecognizer;
typedef struct isdk_HandPokeLimiterVisual_ isdk_HandPokeLimiterVisual;
typedef struct isdk_IActiveState_ isdk_IActiveState;
typedef struct isdk_IBounds_ isdk_IBounds;
typedef struct isdk_ICandidateProperty_ isdk_ICandidateProperty;
typedef struct isdk_IDeltaTimeUpdate_ isdk_IDeltaTimeUpdate;
typedef struct isdk_IEnable_ isdk_IEnable;
typedef struct isdk_IFingerPositions_ isdk_IFingerPositions;
typedef struct isdk_IHandDataModifier_ isdk_IHandDataModifier;
typedef struct isdk_IHandDataSource_ isdk_IHandDataSource;
typedef struct isdk_IHandedness_ isdk_IHandedness;
typedef struct isdk_IHandPosition_ isdk_IHandPosition;
typedef struct isdk_IHandPositionFrame_ isdk_IHandPositionFrame;
typedef struct isdk_IHasSelector_ isdk_IHasSelector;
typedef struct isdk_IInteractable_ isdk_IInteractable;
typedef struct isdk_IInteractor_ isdk_IInteractor;
typedef struct isdk_IInteractorView_ isdk_IInteractorView;
typedef struct isdk_IMovementProvider_ isdk_IMovementProvider;
typedef struct isdk_InteractableStateEventQueue_ isdk_InteractableStateEventQueue;
typedef struct isdk_InteractorPayload_ isdk_InteractorPayload;
typedef struct isdk_InteractorStateEventQueue_ isdk_InteractorStateEventQueue;
typedef struct isdk_IPayload_ isdk_IPayload;
typedef struct isdk_IPointable_ isdk_IPointable;
typedef struct isdk_IPointableElement_ isdk_IPointableElement;
typedef struct isdk_IPose_ isdk_IPose;
typedef struct isdk_IPosition_ isdk_IPosition;
typedef struct isdk_ISelector_ isdk_ISelector;
typedef struct isdk_ISurface_ isdk_ISurface;
typedef struct isdk_ISurfacePatch_ isdk_ISurfacePatch;
typedef struct isdk_IThumbPositions_ isdk_IThumbPositions;
typedef struct isdk_ITimeProvider_ isdk_ITimeProvider;
typedef struct isdk_IUpdate_ isdk_IUpdate;
typedef struct isdk_IUpdateDriver_ isdk_IUpdateDriver;
typedef struct isdk_IWristPosition_ isdk_IWristPosition;
typedef struct isdk_OneEuroHandFilter_ isdk_OneEuroHandFilter;
typedef struct isdk_PalmGrabRecognizer_ isdk_PalmGrabRecognizer;
typedef struct isdk_PointableAxisAlignedBox_ isdk_PointableAxisAlignedBox;
typedef struct isdk_PointableOrientedBox_ isdk_PointableOrientedBox;
typedef struct isdk_PointablePlane_ isdk_PointablePlane;
typedef struct isdk_PointableTransformableUnitSquare_ isdk_PointableTransformableUnitSquare;
typedef struct isdk_PointerEventQueue_ isdk_PointerEventQueue;
typedef struct isdk_PokeButtonVisual_ isdk_PokeButtonVisual;
typedef struct isdk_PokeInteractable_ isdk_PokeInteractable;
typedef struct isdk_PokeInteractor_ isdk_PokeInteractor;
typedef struct isdk_RayInteractable_ isdk_RayInteractable;
typedef struct isdk_RayInteractor_ isdk_RayInteractor;
typedef struct isdk_ScaledTimeProvider_ isdk_ScaledTimeProvider;
typedef struct isdk_Selector_ isdk_Selector;
typedef struct isdk_SyntheticHand_ isdk_SyntheticHand;
typedef struct isdk_ThumbRecognizer_ isdk_ThumbRecognizer;

typedef struct isdk_DigitRangeParams_ {
  float lowerLimit;
  float lowerTolerance;
  float upperLimit;
  float upperTolerance;
  float minTimeToTransition;
} isdk_DigitRangeParams;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_DigitRangeParams_init(isdk_DigitRangeParams* instance);

typedef struct isdk_DigitRecognizer_ExpectedAngleValueRange_ {
  float minValue;
  float maxValue;
} isdk_DigitRecognizer_ExpectedAngleValueRange;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_ExpectedAngleValueRange_init(
    isdk_DigitRecognizer_ExpectedAngleValueRange* instance);

typedef struct isdk_DigitRecognizer_ExpectedDistanceValueRange_ {
  float minValue;
  float maxValue;
} isdk_DigitRecognizer_ExpectedDistanceValueRange;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_ExpectedDistanceValueRange_init(
    isdk_DigitRecognizer_ExpectedDistanceValueRange* instance);

typedef struct isdk_ExternalHandPositionFrame_FingerJointMapping_ {
  isdk_FingerType finger;
  isdk_FingerJoint joint;
  int index;
} isdk_ExternalHandPositionFrame_FingerJointMapping;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_FingerJointMapping_init(
    isdk_ExternalHandPositionFrame_FingerJointMapping* instance);

typedef struct isdk_ExternalHandPositionFrame_ThumbJointMapping_ {
  isdk_ThumbJoint joint;
  int index;
} isdk_ExternalHandPositionFrame_ThumbJointMapping;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_ThumbJointMapping_init(
    isdk_ExternalHandPositionFrame_ThumbJointMapping* instance);

typedef struct isdk_FilterPropertyBlock_ {
  float minCutOff;
  float beta;
  float deltaMinCutOff;
} isdk_FilterPropertyBlock;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_FilterPropertyBlock_init(isdk_FilterPropertyBlock* instance);

typedef struct isdk_FingerRecognizer_ExpectedFingerValueRanges_ {
  isdk_DigitRecognizer_ExpectedAngleValueRange curl;
  isdk_DigitRecognizer_ExpectedAngleValueRange flexion;
  isdk_DigitRecognizer_ExpectedAngleValueRange abduction;
  isdk_DigitRecognizer_ExpectedDistanceValueRange opposition;
  isdk_DigitRecognizer_ExpectedAngleValueRange grab;
  isdk_DigitRecognizer_ExpectedDistanceValueRange oppositionTangentPlane;
  isdk_DigitRecognizer_ExpectedDistanceValueRange oppositionNormal;
  isdk_DigitRecognizer_ExpectedDistanceValueRange oppositionTopTwo;
  isdk_DigitRecognizer_ExpectedDistanceValueRange oppositionTopThree;
} isdk_FingerRecognizer_ExpectedFingerValueRanges;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_ExpectedFingerValueRanges_init(
    isdk_FingerRecognizer_ExpectedFingerValueRanges* instance);

typedef struct isdk_InteractableStateChangeArgs_ {
  isdk_InteractableState previousState;
  isdk_InteractableState newState;
} isdk_InteractableStateChangeArgs;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_InteractableStateChangeArgs_init(isdk_InteractableStateChangeArgs* instance);

typedef struct isdk_InteractorStateChangeArgs_ {
  isdk_InteractorState previousState;
  isdk_InteractorState newState;
} isdk_InteractorStateChangeArgs;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_InteractorStateChangeArgs_init(isdk_InteractorStateChangeArgs* instance);

typedef struct isdk_PokeInteractable_DragThresholdingConfig_ {
  unsigned char enabled;
  float dragNormal;
  float dragTangent;
} isdk_PokeInteractable_DragThresholdingConfig;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_DragThresholdingConfig_init(
    isdk_PokeInteractable_DragThresholdingConfig* instance);

typedef struct isdk_PokeInteractable_MinThresholdsConfig_ {
  unsigned char enabled;
  float minNormal;
} isdk_PokeInteractable_MinThresholdsConfig;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_MinThresholdsConfig_init(isdk_PokeInteractable_MinThresholdsConfig* instance);

typedef struct isdk_PokeInteractable_PositionPinningConfig_ {
  unsigned char enabled;
  float maxPinDistance;
} isdk_PokeInteractable_PositionPinningConfig;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_PositionPinningConfig_init(
    isdk_PokeInteractable_PositionPinningConfig* instance);

typedef struct isdk_PokeInteractable_RecoilAssistConfig_ {
  unsigned char enabled;
  unsigned char useVelocityExpansion;
  float velocityExpansionMinSpeed;
  float velocityExpansionMaxSpeed;
  float velocityExpansionDistance;
  float velocityExpansionDecayRate;
  unsigned char useVelocityRetractExpansion;
  float velocityExpansionRetractMinSpeed;
  float velocityExpansionRetractMaxSpeed;
  float velocityExpansionRetractDistance;
  float velocityExpansionRetractDecayRate;
  unsigned char useDynamicDecay;
  float exitDistance;
  float reEnterDistance;
} isdk_PokeInteractable_RecoilAssistConfig;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_RecoilAssistConfig_init(isdk_PokeInteractable_RecoilAssistConfig* instance);

typedef struct isdk_PokeInteractable_Config_ {
  float enterHoverNormal;
  float enterHoverTangent;
  float exitHoverNormal;
  float exitHoverTangent;
  float cancelSelectNormal;
  float cancelSelectTangent;
  isdk_PokeInteractable_PositionPinningConfig positionPinningConfig;
  isdk_PokeInteractable_DragThresholdingConfig dragThresholdsConfig;
  isdk_PokeInteractable_MinThresholdsConfig minThresholdsConfig;
  isdk_PokeInteractable_RecoilAssistConfig recoilAssistConfig;
} isdk_PokeInteractable_Config;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_Config_init(isdk_PokeInteractable_Config* instance);

typedef struct isdk_PokeInteractor_Config_ {
  float touchReleaseThreshold;
  float maxDeltaFromTouchPoint;
  float equalDistanceThreshold;
  float radius;
  unsigned char ignorePositionPinning;
} isdk_PokeInteractor_Config;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_Config_init(isdk_PokeInteractor_Config* instance);

typedef struct isdk_PokeInteractor_SurfaceHitCache_ {
} isdk_PokeInteractor_SurfaceHitCache;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_SurfaceHitCache_init(isdk_PokeInteractor_SurfaceHitCache* instance);

typedef struct isdk_RayInteractor_Config_ {
  float maxRayLength;
  float equalDistanceThreshold;
} isdk_RayInteractor_Config;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_Config_init(isdk_RayInteractor_Config* instance);

typedef struct isdk_RayInteractor_RayCandidateProperties_ {
} isdk_RayInteractor_RayCandidateProperties;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_RayCandidateProperties_init(isdk_RayInteractor_RayCandidateProperties* instance);

typedef struct isdk_SyntheticHand_Config_ {
} isdk_SyntheticHand_Config;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_Config_init(isdk_SyntheticHand_Config* instance);

typedef struct isdk_ThumbRecognizer_ExpectedThumbValueRanges_ {
  isdk_DigitRecognizer_ExpectedAngleValueRange curl;
  isdk_DigitRecognizer_ExpectedAngleValueRange flexion;
} isdk_ThumbRecognizer_ExpectedThumbValueRanges;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_ExpectedThumbValueRanges_init(
    isdk_ThumbRecognizer_ExpectedThumbValueRanges* instance);

// A quaternion rotation.
typedef struct ovrpQuatf_ {
  float x;
  float y;
  float z;
  float w;
} ovrpQuatf;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
ovrpQuatf_init(ovrpQuatf* instance);

// A 2D vector with float components.
typedef struct ovrpVector2f_ {
  float x;
  float y;
} ovrpVector2f;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
ovrpVector2f_init(ovrpVector2f* instance);

// A 3D vector with float components.
typedef struct ovrpVector3f_ {
  float x;
  float y;
  float z;
} ovrpVector3f;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
ovrpVector3f_init(ovrpVector3f* instance);

typedef struct isdk_AxisAlignedBox_ {
  ovrpVector3f lowerBound;
  ovrpVector3f upperBound;
  ovrpVector3f centroid;
  ovrpVector3f halfSize;
} isdk_AxisAlignedBox;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_AxisAlignedBox_init(isdk_AxisAlignedBox* instance);

typedef struct isdk_BoundsClipper_ {
  isdk_IPose* poseProvider;
  ovrpVector3f position;
  ovrpVector3f size;
} isdk_BoundsClipper;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_BoundsClipper_init(isdk_BoundsClipper* instance);

typedef struct isdk_HandPinchData_ {
  ovrpVector3f jointPositions[24];
} isdk_HandPinchData;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_HandPinchData_init(isdk_HandPinchData* instance);

typedef struct isdk_Ray_ {
  ovrpVector3f origin;
  ovrpVector3f direction;
} isdk_Ray;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_Ray_init(isdk_Ray* instance);

typedef struct isdk_SurfaceHit_ {
  ovrpVector3f point;
  ovrpVector3f normal;
  float distance;
} isdk_SurfaceHit;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_SurfaceHit_init(isdk_SurfaceHit* instance);

typedef struct isdk_OptionalSurfaceHit_ {
  unsigned char hasValue;
  isdk_SurfaceHit value;
} isdk_OptionalSurfaceHit;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_OptionalSurfaceHit_init(isdk_OptionalSurfaceHit* instance);

// A quaternion rotation.
typedef struct ovrpPosef_ {
  ovrpQuatf Orientation;
  ovrpVector3f Position;
} ovrpPosef;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
ovrpPosef_init(ovrpPosef* instance);

typedef struct isdk_HandData_ {
  ovrpQuatf joints[24];
  ovrpPosef root;
} isdk_HandData;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_HandData_init(isdk_HandData* instance);

typedef struct isdk_HandPalmData_ {
  ovrpPosef joints[24];
  ovrpPosef root;
  int handedness;
} isdk_HandPalmData;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_HandPalmData_init(isdk_HandPalmData* instance);

typedef struct isdk_PointerEvent_ {
  int identifier;
  isdk_PointerEventType type;
  ovrpPosef pose;
  isdk_IPayload* payload;
} isdk_PointerEvent;

// Sets all fields to their default values
INTERACTIONSDK_EXPORT(void)
isdk_PointerEvent_init(isdk_PointerEvent* instance);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ActiveStateBool_castToIActiveState(isdk_ActiveStateBool* instance, isdk_IActiveState** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ActiveStateBool_castFromIActiveState(
    isdk_IActiveState* instance,
    isdk_ActiveStateBool** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ActiveStateBool_create(isdk_ActiveStateBool** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ActiveStateBool_destroy(isdk_ActiveStateBool* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ActiveStateBool_isActive(isdk_ActiveStateBool* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ActiveStateBool_setActive(isdk_ActiveStateBool* instance, const bool value);

INTERACTIONSDK_EXPORT(void)
isdk_ApiContext_destroy(isdk_ApiContext* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ApiContext_getVersion(char* outString, const int outStringCount, unsigned long long* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_castToIPose(isdk_ClippedPlaneSurface* instance, isdk_IPose** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_castFromIPose(isdk_IPose* instance, isdk_ClippedPlaneSurface** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_castToIBounds(isdk_ClippedPlaneSurface* instance, isdk_IBounds** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_castFromIBounds(isdk_IBounds* instance, isdk_ClippedPlaneSurface** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_castToISurface(isdk_ClippedPlaneSurface* instance, isdk_ISurface** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_castFromISurface(
    isdk_ISurface* instance,
    isdk_ClippedPlaneSurface** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_castToISurfacePatch(
    isdk_ClippedPlaneSurface* instance,
    isdk_ISurfacePatch** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_castFromISurfacePatch(
    isdk_ISurfacePatch* instance,
    isdk_ClippedPlaneSurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_create(
    isdk_PointablePlane* planeSurface,
    isdk_ClippedPlaneSurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_createWithClippers(
    isdk_PointablePlane* planeSurface,
    isdk_BoundsClipper* clippers,
    const int clippersCount,
    isdk_ClippedPlaneSurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_destroy(isdk_ClippedPlaneSurface* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_raycast(
    isdk_ClippedPlaneSurface* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_closestSurfacePoint(
    isdk_ClippedPlaneSurface* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_setPose(isdk_ClippedPlaneSurface* instance, const ovrpPosef* pose);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_getNormal(isdk_ClippedPlaneSurface* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_setSize(isdk_ClippedPlaneSurface* instance, const ovrpVector2f* newSize);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_getPose(isdk_ClippedPlaneSurface* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_getBounds(isdk_ClippedPlaneSurface* instance, isdk_AxisAlignedBox* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_getBackingSurface(
    isdk_ClippedPlaneSurface* instance,
    isdk_ISurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_getClippers(
    isdk_ClippedPlaneSurface* instance,
    isdk_BoundsClipper* retVal,
    const int retValCount,
    int* retValCountOut);

INTERACTIONSDK_EXPORT(void)
isdk_ClippedPlaneSurface_setClippers(
    isdk_ClippedPlaneSurface* instance,
    isdk_BoundsClipper* clippers,
    const int clippersCount);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_castToIPose(isdk_CylinderSurface* instance, isdk_IPose** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_castFromIPose(isdk_IPose* instance, isdk_CylinderSurface** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_castToIBounds(isdk_CylinderSurface* instance, isdk_IBounds** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_castFromIBounds(isdk_IBounds* instance, isdk_CylinderSurface** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_castToISurface(isdk_CylinderSurface* instance, isdk_ISurface** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_castFromISurface(isdk_ISurface* instance, isdk_CylinderSurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_create(isdk_CylinderSurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_createWithCylinder(
    const ovrpPosef* pose,
    float height,
    float radius,
    isdk_NormalFacing normalFacing,
    isdk_CylinderSurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_destroy(isdk_CylinderSurface* instance);

INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_setCylinder(
    isdk_CylinderSurface* instance,
    const ovrpPosef* pose,
    float height,
    float radius,
    isdk_NormalFacing normalFacing);

INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_raycast(
    isdk_CylinderSurface* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_closestSurfacePoint(
    isdk_CylinderSurface* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_getPose(isdk_CylinderSurface* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_CylinderSurface_getBounds(isdk_CylinderSurface* instance, isdk_AxisAlignedBox* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_castToIActiveState(isdk_DigitRecognizer* instance, isdk_IActiveState** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_castFromIActiveState(
    isdk_IActiveState* instance,
    isdk_DigitRecognizer** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_castToIDeltaTimeUpdate(
    isdk_DigitRecognizer* instance,
    isdk_IDeltaTimeUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_castFromIDeltaTimeUpdate(
    isdk_IDeltaTimeUpdate* instance,
    isdk_DigitRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_destroy(isdk_DigitRecognizer* instance);

INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_update(isdk_DigitRecognizer* instance, float deltaTime);

INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_getUpdateResult(
    isdk_DigitRecognizer* instance,
    isdk_DigitRecognizer_UpdateResult* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_isActive(isdk_DigitRecognizer* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_DigitRecognizer_getRawValueNormalized(isdk_DigitRecognizer* instance, float* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_DummyHandSource_castToIHandDataSource(
    isdk_DummyHandSource* instance,
    isdk_IHandDataSource** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_DummyHandSource_castFromIHandDataSource(
    isdk_IHandDataSource* instance,
    isdk_DummyHandSource** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_DummyHandSource_create(int numJoints, isdk_DummyHandSource** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_DummyHandSource_destroy(isdk_DummyHandSource* instance);

INTERACTIONSDK_EXPORT(void)
isdk_DummyHandSource_getData(isdk_DummyHandSource* instance, isdk_HandData* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_DummyHandSource_getJoints(
    isdk_DummyHandSource* instance,
    ovrpQuatf* retVal,
    const int retValCount,
    int* retValCountOut);

INTERACTIONSDK_EXPORT(void)
isdk_DummyHandSource_getRootPose(isdk_DummyHandSource* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_DummyHandSource_updateData(
    isdk_DummyHandSource* instance,
    isdk_DataSourceUpdateDataResult* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castToIFingerPositions(
    isdk_ExternalHandPositionFrame* instance,
    isdk_IFingerPositions** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castFromIFingerPositions(
    isdk_IFingerPositions* instance,
    isdk_ExternalHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castToIThumbPositions(
    isdk_ExternalHandPositionFrame* instance,
    isdk_IThumbPositions** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castFromIThumbPositions(
    isdk_IThumbPositions* instance,
    isdk_ExternalHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castToIWristPosition(
    isdk_ExternalHandPositionFrame* instance,
    isdk_IWristPosition** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castFromIWristPosition(
    isdk_IWristPosition* instance,
    isdk_ExternalHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castToIHandedness(
    isdk_ExternalHandPositionFrame* instance,
    isdk_IHandedness** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castFromIHandedness(
    isdk_IHandedness* instance,
    isdk_ExternalHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castToIHandPosition(
    isdk_ExternalHandPositionFrame* instance,
    isdk_IHandPosition** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castFromIHandPosition(
    isdk_IHandPosition* instance,
    isdk_ExternalHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castToIHandPositionFrame(
    isdk_ExternalHandPositionFrame* instance,
    isdk_IHandPositionFrame** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_castFromIHandPositionFrame(
    isdk_IHandPositionFrame* instance,
    isdk_ExternalHandPositionFrame** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_create(
    isdk_Handedness handedness,
    const isdk_ExternalHandPositionFrame_ThumbJointMapping* thumbMappings,
    const int thumbMappingsCount,
    const isdk_ExternalHandPositionFrame_FingerJointMapping* fingerMappings,
    const int fingerMappingsCount,
    unsigned long long jointDataSize,
    isdk_ExternalHandPositionFrame** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_destroy(isdk_ExternalHandPositionFrame* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_setJointData(
    isdk_ExternalHandPositionFrame* instance,
    ovrpVector3f* jointData,
    const int jointDataCount);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_setWristData(
    isdk_ExternalHandPositionFrame* instance,
    const ovrpVector3f* wristPositionWorld);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_getFingerPosition(
    isdk_ExternalHandPositionFrame* instance,
    isdk_FingerType type,
    isdk_FingerJoint joint,
    ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_getThumbPosition(
    isdk_ExternalHandPositionFrame* instance,
    isdk_ThumbJoint joint,
    ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_getWristPosition(
    isdk_ExternalHandPositionFrame* instance,
    ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandPositionFrame_getHandedness(
    isdk_ExternalHandPositionFrame* instance,
    isdk_Handedness* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_castToIHandDataSource(
    isdk_ExternalHandSource* instance,
    isdk_IHandDataSource** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_castFromIHandDataSource(
    isdk_IHandDataSource* instance,
    isdk_ExternalHandSource** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_create(int numJoints, isdk_ExternalHandSource** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_destroy(isdk_ExternalHandSource* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_getData(isdk_ExternalHandSource* instance, isdk_HandData* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_getJoints(
    isdk_ExternalHandSource* instance,
    ovrpQuatf* retVal,
    const int retValCount,
    int* retValCountOut);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_getRootPose(isdk_ExternalHandSource* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_updateData(
    isdk_ExternalHandSource* instance,
    isdk_DataSourceUpdateDataResult* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ExternalHandSource_setData(
    isdk_ExternalHandSource* instance,
    const isdk_HandData* data,
    isdk_DataSourceUpdateDataResult* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_castToIActiveState(
    isdk_FingerPinchGrabRecognizer* instance,
    isdk_IActiveState** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_castFromIActiveState(
    isdk_IActiveState* instance,
    isdk_FingerPinchGrabRecognizer** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_castToIDeltaTimeUpdate(
    isdk_FingerPinchGrabRecognizer* instance,
    isdk_IDeltaTimeUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_castFromIDeltaTimeUpdate(
    isdk_IDeltaTimeUpdate* instance,
    isdk_FingerPinchGrabRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_create(
    isdk_IHandPositionFrame* handPositionFrame,
    isdk_FingerType fingerType,
    isdk_FingerPinchGrabRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_destroy(isdk_FingerPinchGrabRecognizer* instance);

INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_setInputTrackingConfidence(
    isdk_FingerPinchGrabRecognizer* instance,
    unsigned char isHighConfidence);

INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_setInputTrackingConfidenceWithData(
    isdk_FingerPinchGrabRecognizer* instance,
    ovrpVector3f wristForward,
    ovrpVector3f hmdForward,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_update(isdk_FingerPinchGrabRecognizer* instance, float deltaTime);

INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_isActive(
    isdk_FingerPinchGrabRecognizer* instance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_FingerPinchGrabRecognizer_getPinchStrength(
    isdk_FingerPinchGrabRecognizer* instance,
    float* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_castToIActiveState(
    isdk_FingerRecognizer* instance,
    isdk_IActiveState** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_castFromIActiveState(
    isdk_IActiveState* instance,
    isdk_FingerRecognizer** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_castToIDeltaTimeUpdate(
    isdk_FingerRecognizer* instance,
    isdk_IDeltaTimeUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_castFromIDeltaTimeUpdate(
    isdk_IDeltaTimeUpdate* instance,
    isdk_FingerRecognizer** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_castToDigitRecognizer(
    isdk_FingerRecognizer* instance,
    isdk_DigitRecognizer** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_castFromDigitRecognizer(
    isdk_DigitRecognizer* instance,
    isdk_FingerRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_create(
    isdk_IHandPositionFrame* handPositionFrame,
    const isdk_DigitRangeParams* parameters,
    isdk_FingerType fingerType,
    isdk_Detection_FingerCalcType calcType,
    isdk_FingerRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_destroy(isdk_FingerRecognizer* instance);

INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_getExpectedFingerRawValueRanges(
    isdk_FingerType fingerType,
    isdk_FingerRecognizer_ExpectedFingerValueRanges* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_FingerRecognizer_getFingerRangeParamsFromNormalized(
    const isdk_DigitRangeParams* normalizedParams,
    isdk_FingerType fingerType,
    isdk_Detection_FingerCalcType calcType,
    isdk_DigitRangeParams* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_HandPokeLimiterVisual_castToIUpdate(
    isdk_HandPokeLimiterVisual* instance,
    isdk_IUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_HandPokeLimiterVisual_castFromIUpdate(
    isdk_IUpdate* instance,
    isdk_HandPokeLimiterVisual** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_HandPokeLimiterVisual_create(
    isdk_IHandDataSource* fromDataSource,
    isdk_PokeInteractor* pokeInteractor,
    isdk_SyntheticHand* syntheticHand,
    isdk_HandPokeLimiterVisual** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_HandPokeLimiterVisual_destroy(isdk_HandPokeLimiterVisual* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IActiveState_destroy(isdk_IActiveState* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IActiveState_isActive(isdk_IActiveState* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IBounds_destroy(isdk_IBounds* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IBounds_getBounds(isdk_IBounds* instance, isdk_AxisAlignedBox* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ICandidateProperty_destroy(isdk_ICandidateProperty* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IDeltaTimeUpdate_destroy(isdk_IDeltaTimeUpdate* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IDeltaTimeUpdate_update(isdk_IDeltaTimeUpdate* instance, float deltaTime);

INTERACTIONSDK_EXPORT(void)
isdk_IEnable_destroy(isdk_IEnable* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IEnable_onEnable(isdk_IEnable* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IEnable_onDisable(isdk_IEnable* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IFingerPositions_destroy(isdk_IFingerPositions* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IFingerPositions_getFingerPosition(
    isdk_IFingerPositions* instance,
    isdk_FingerType type,
    isdk_FingerJoint joint,
    ovrpVector3f* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandDataModifier_castToIHandDataSource(
    isdk_IHandDataModifier* instance,
    isdk_IHandDataSource** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandDataModifier_castFromIHandDataSource(
    isdk_IHandDataSource* instance,
    isdk_IHandDataModifier** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataModifier_destroy(isdk_IHandDataModifier* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataModifier_setFromDataSource(
    isdk_IHandDataModifier* instance,
    isdk_IHandDataSource* fromDataSource);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataModifier_getFromDataSource(
    isdk_IHandDataModifier* instance,
    isdk_IHandDataSource** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataModifier_setRecursiveUpdate(
    isdk_IHandDataModifier* instance,
    unsigned char recursiveUpdate);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataModifier_getRecursiveUpdate(isdk_IHandDataModifier* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataSource_destroy(isdk_IHandDataSource* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataSource_getData(isdk_IHandDataSource* instance, isdk_HandData* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataSource_getJoints(
    isdk_IHandDataSource* instance,
    ovrpQuatf* retVal,
    const int retValCount,
    int* retValCountOut);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataSource_getRootPose(isdk_IHandDataSource* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHandDataSource_updateData(
    isdk_IHandDataSource* instance,
    isdk_DataSourceUpdateDataResult* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHandedness_destroy(isdk_IHandedness* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IHandedness_getHandedness(isdk_IHandedness* instance, isdk_Handedness* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_castToIFingerPositions(
    isdk_IHandPosition* instance,
    isdk_IFingerPositions** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_castFromIFingerPositions(
    isdk_IFingerPositions* instance,
    isdk_IHandPosition** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_castToIThumbPositions(
    isdk_IHandPosition* instance,
    isdk_IThumbPositions** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_castFromIThumbPositions(
    isdk_IThumbPositions* instance,
    isdk_IHandPosition** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_castToIWristPosition(isdk_IHandPosition* instance, isdk_IWristPosition** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_castFromIWristPosition(
    isdk_IWristPosition* instance,
    isdk_IHandPosition** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_castToIHandedness(isdk_IHandPosition* instance, isdk_IHandedness** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_castFromIHandedness(isdk_IHandedness* instance, isdk_IHandPosition** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHandPosition_destroy(isdk_IHandPosition* instance);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castToIFingerPositions(
    isdk_IHandPositionFrame* instance,
    isdk_IFingerPositions** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castFromIFingerPositions(
    isdk_IFingerPositions* instance,
    isdk_IHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castToIThumbPositions(
    isdk_IHandPositionFrame* instance,
    isdk_IThumbPositions** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castFromIThumbPositions(
    isdk_IThumbPositions* instance,
    isdk_IHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castToIWristPosition(
    isdk_IHandPositionFrame* instance,
    isdk_IWristPosition** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castFromIWristPosition(
    isdk_IWristPosition* instance,
    isdk_IHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castToIHandedness(
    isdk_IHandPositionFrame* instance,
    isdk_IHandedness** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castFromIHandedness(
    isdk_IHandedness* instance,
    isdk_IHandPositionFrame** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castToIHandPosition(
    isdk_IHandPositionFrame* instance,
    isdk_IHandPosition** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_castFromIHandPosition(
    isdk_IHandPosition* instance,
    isdk_IHandPositionFrame** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHandPositionFrame_destroy(isdk_IHandPositionFrame* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IHasSelector_destroy(isdk_IHasSelector* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IHasSelector_getSelector(isdk_IHasSelector* instance, isdk_ISelector** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IHasSelector_setSelector(isdk_IHasSelector* instance, isdk_ISelector* newSelector);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_destroy(isdk_IInteractable* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_enable(isdk_IInteractable* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_disable(isdk_IInteractable* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_getState(isdk_IInteractable* instance, isdk_InteractableState* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_getInteractors(
    isdk_IInteractable* instance,
    isdk_IInteractor** interactors,
    const int interactorsCount,
    unsigned long long* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_getSelectingInteractors(
    isdk_IInteractable* instance,
    isdk_IInteractor** interactors,
    const int interactorsCount,
    unsigned long long* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_getInteractorsCount(isdk_IInteractable* instance, int* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_getSelectingInteractorsCount(isdk_IInteractable* instance, int* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_getPayload(isdk_IInteractable* instance, isdk_IPayload** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_getPayloadSharedPtr(isdk_IInteractable* instance, isdk_IPayload** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractable_setPayload(isdk_IInteractable* instance, isdk_IPayload* payload);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_castToIInteractorView(isdk_IInteractor* instance, isdk_IInteractorView** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_castFromIInteractorView(isdk_IInteractorView* instance, isdk_IInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_castToIUpdate(isdk_IInteractor* instance, isdk_IUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_castFromIUpdate(isdk_IUpdate* instance, isdk_IInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_castToIUpdateDriver(isdk_IInteractor* instance, isdk_IUpdateDriver** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_castFromIUpdateDriver(isdk_IUpdateDriver* instance, isdk_IInteractor** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_destroy(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_preprocess(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_process(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_postprocess(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_processCandidate(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_enable(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_disable(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_hover(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_unhover(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_select(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_unselect(isdk_IInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_shouldHover(isdk_IInteractor* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_shouldUnhover(isdk_IInteractor* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_shouldSelect(isdk_IInteractor* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_shouldUnselect(isdk_IInteractor* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_getInteractablePtr(isdk_IInteractor* instance, isdk_IInteractable** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractor_getSelectedInteractablePtr(
    isdk_IInteractor* instance,
    isdk_IInteractable** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_destroy(isdk_IInteractorView* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_getIdentifier(isdk_IInteractorView* instance, int* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_getPayload(isdk_IInteractorView* instance, isdk_IPayload** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_getPayloadSharedPtr(isdk_IInteractorView* instance, isdk_IPayload** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_setPayload(isdk_IInteractorView* instance, isdk_IPayload* payload);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_hasCandidate(isdk_IInteractorView* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_getCandidateProperties(
    isdk_IInteractorView* instance,
    isdk_ICandidateProperty** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_hasInteractable(isdk_IInteractorView* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_hasSelectedInteractable(isdk_IInteractorView* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IInteractorView_getState(isdk_IInteractorView* instance, isdk_InteractorState* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IMovementProvider_destroy(isdk_IMovementProvider* instance);

INTERACTIONSDK_EXPORT(void)
isdk_InteractableStateEventQueue_create(
    isdk_IInteractable* target,
    isdk_InteractableStateEventQueue** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractableStateEventQueue_destroy(isdk_InteractableStateEventQueue* instance);

INTERACTIONSDK_EXPORT(void)
isdk_InteractableStateEventQueue_isEmpty(
    isdk_InteractableStateEventQueue* instance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractableStateEventQueue_getCount(
    isdk_InteractableStateEventQueue* instance,
    unsigned long long* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractableStateEventQueue_pop(
    isdk_InteractableStateEventQueue* instance,
    isdk_InteractableStateChangeArgs* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractableStateEventQueue_clear(isdk_InteractableStateEventQueue* instance);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_InteractorPayload_castToIPayload(isdk_InteractorPayload* instance, isdk_IPayload** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_InteractorPayload_castFromIPayload(isdk_IPayload* instance, isdk_InteractorPayload** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorPayload_create(
    isdk_IInteractor* interactor,
    void* externalData,
    isdk_InteractorPayload** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorPayload_destroy(isdk_InteractorPayload* instance);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorPayload_getInteractor(isdk_InteractorPayload* instance, isdk_IInteractor** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorPayload_setExternalData(isdk_InteractorPayload* instance, void* externalData);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorPayload_getExternalData(isdk_InteractorPayload* instance, void** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorStateEventQueue_create(
    isdk_IInteractor* target,
    isdk_InteractorStateEventQueue** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorStateEventQueue_destroy(isdk_InteractorStateEventQueue* instance);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorStateEventQueue_isEmpty(
    isdk_InteractorStateEventQueue* instance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorStateEventQueue_getCount(
    isdk_InteractorStateEventQueue* instance,
    unsigned long long* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorStateEventQueue_pop(
    isdk_InteractorStateEventQueue* instance,
    isdk_InteractorStateChangeArgs* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorStateEventQueue_clear(isdk_InteractorStateEventQueue* instance);

INTERACTIONSDK_EXPORT(void)
isdk_InteractorStateEventQueue_setEventTypeHint(
    isdk_InteractorStateEventQueue* instance,
    const char* interactorTypeHint);

INTERACTIONSDK_EXPORT(void)
isdk_IPayload_destroy(isdk_IPayload* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IPointable_destroy(isdk_IPointable* instance);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IPointableElement_castToIPointable(isdk_IPointableElement* instance, isdk_IPointable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_IPointableElement_castFromIPointable(
    isdk_IPointable* instance,
    isdk_IPointableElement** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IPointableElement_destroy(isdk_IPointableElement* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IPointableElement_processPointerEvent(
    isdk_IPointableElement* instance,
    const isdk_PointerEvent* paramVar0);

INTERACTIONSDK_EXPORT(void)
isdk_IPose_destroy(isdk_IPose* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IPose_getPose(isdk_IPose* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IPosition_destroy(isdk_IPosition* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IPosition_getPosition(isdk_IPosition* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ISelector_destroy(isdk_ISelector* instance);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurface_castToIPose(isdk_ISurface* instance, isdk_IPose** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurface_castFromIPose(isdk_IPose* instance, isdk_ISurface** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurface_castToIBounds(isdk_ISurface* instance, isdk_IBounds** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurface_castFromIBounds(isdk_IBounds* instance, isdk_ISurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ISurface_destroy(isdk_ISurface* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ISurface_raycast(
    isdk_ISurface* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ISurface_closestSurfacePoint(
    isdk_ISurface* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurfacePatch_castToIPose(isdk_ISurfacePatch* instance, isdk_IPose** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurfacePatch_castFromIPose(isdk_IPose* instance, isdk_ISurfacePatch** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurfacePatch_castToIBounds(isdk_ISurfacePatch* instance, isdk_IBounds** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurfacePatch_castFromIBounds(isdk_IBounds* instance, isdk_ISurfacePatch** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurfacePatch_castToISurface(isdk_ISurfacePatch* instance, isdk_ISurface** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ISurfacePatch_castFromISurface(isdk_ISurface* instance, isdk_ISurfacePatch** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ISurfacePatch_destroy(isdk_ISurfacePatch* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ISurfacePatch_getBackingSurface(isdk_ISurfacePatch* instance, isdk_ISurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IThumbPositions_destroy(isdk_IThumbPositions* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IThumbPositions_getThumbPosition(
    isdk_IThumbPositions* instance,
    isdk_ThumbJoint joint,
    ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ITimeProvider_destroy(isdk_ITimeProvider* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IUpdate_destroy(isdk_IUpdate* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IUpdate_update(isdk_IUpdate* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IUpdateDriver_destroy(isdk_IUpdateDriver* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IUpdateDriver_getIsRootDriver(isdk_IUpdateDriver* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_IUpdateDriver_setIsRootDriver(isdk_IUpdateDriver* instance, unsigned char value);

INTERACTIONSDK_EXPORT(void)
isdk_IUpdateDriver_drive(isdk_IUpdateDriver* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IWristPosition_destroy(isdk_IWristPosition* instance);

INTERACTIONSDK_EXPORT(void)
isdk_IWristPosition_getWristPosition(isdk_IWristPosition* instance, ovrpVector3f* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_OneEuroHandFilter_castToIHandDataSource(
    isdk_OneEuroHandFilter* instance,
    isdk_IHandDataSource** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_OneEuroHandFilter_castFromIHandDataSource(
    isdk_IHandDataSource* instance,
    isdk_OneEuroHandFilter** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_OneEuroHandFilter_castToIHandDataModifier(
    isdk_OneEuroHandFilter* instance,
    isdk_IHandDataModifier** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_OneEuroHandFilter_castFromIHandDataModifier(
    isdk_IHandDataModifier* instance,
    isdk_OneEuroHandFilter** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_OneEuroHandFilter_create(
    isdk_IHandDataSource* fromDataSource,
    unsigned char recursiveUpdate,
    isdk_OneEuroHandFilter** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_OneEuroHandFilter_destroy(isdk_OneEuroHandFilter* instance);

INTERACTIONSDK_EXPORT(void)
isdk_OneEuroHandFilter_setFromDataSource(
    isdk_OneEuroHandFilter* instance,
    isdk_IHandDataSource* fromDataSource);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_castToIActiveState(
    isdk_PalmGrabRecognizer* instance,
    isdk_IActiveState** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_castFromIActiveState(
    isdk_IActiveState* instance,
    isdk_PalmGrabRecognizer** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_castToIDeltaTimeUpdate(
    isdk_PalmGrabRecognizer* instance,
    isdk_IDeltaTimeUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_castFromIDeltaTimeUpdate(
    isdk_IDeltaTimeUpdate* instance,
    isdk_PalmGrabRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_create(
    isdk_IHandPositionFrame* handPositionFrame,
    isdk_PalmGrabRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_destroy(isdk_PalmGrabRecognizer* instance);

INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_setInputTrackingConfidence(
    isdk_PalmGrabRecognizer* instance,
    unsigned char isHighConfidence);

INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_update(isdk_PalmGrabRecognizer* instance, float deltaTime);

INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_isActive(isdk_PalmGrabRecognizer* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_setFingerMinCurlValue(
    isdk_PalmGrabRecognizer* instance,
    isdk_FingerType finger,
    float angle);

INTERACTIONSDK_EXPORT(void)
isdk_PalmGrabRecognizer_setFingerMinCount(isdk_PalmGrabRecognizer* instance, int count);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_castToIPose(
    isdk_PointableAxisAlignedBox* instance,
    isdk_IPose** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_castFromIPose(
    isdk_IPose* instance,
    isdk_PointableAxisAlignedBox** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_castToIBounds(
    isdk_PointableAxisAlignedBox* instance,
    isdk_IBounds** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_castFromIBounds(
    isdk_IBounds* instance,
    isdk_PointableAxisAlignedBox** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_castToISurface(
    isdk_PointableAxisAlignedBox* instance,
    isdk_ISurface** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_castFromISurface(
    isdk_ISurface* instance,
    isdk_PointableAxisAlignedBox** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_create(isdk_PointableAxisAlignedBox** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_createWithPose(
    const ovrpVector3f* centroid,
    const ovrpVector3f* size,
    isdk_PointableAxisAlignedBox** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_destroy(isdk_PointableAxisAlignedBox* instance);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_raycast(
    isdk_PointableAxisAlignedBox* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_closestSurfacePoint(
    isdk_PointableAxisAlignedBox* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_setBounds(
    isdk_PointableAxisAlignedBox* instance,
    const ovrpVector3f* lowerBound,
    const ovrpVector3f* upperBound);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_setCentroidAndSize(
    isdk_PointableAxisAlignedBox* instance,
    const ovrpVector3f* centroid,
    const ovrpVector3f* size);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_setLowerBound(
    isdk_PointableAxisAlignedBox* instance,
    const ovrpVector3f* lowerBound);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_setUpperBound(
    isdk_PointableAxisAlignedBox* instance,
    const ovrpVector3f* upperBound);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_setCentroid(
    isdk_PointableAxisAlignedBox* instance,
    const ovrpVector3f* centroid);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_setSize(
    isdk_PointableAxisAlignedBox* instance,
    const ovrpVector3f* size);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_getLowerBound(
    isdk_PointableAxisAlignedBox* instance,
    ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_getUpperBound(
    isdk_PointableAxisAlignedBox* instance,
    ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_getCenter(
    isdk_PointableAxisAlignedBox* instance,
    ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_getSize(isdk_PointableAxisAlignedBox* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_getPose(isdk_PointableAxisAlignedBox* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableAxisAlignedBox_getBounds(
    isdk_PointableAxisAlignedBox* instance,
    isdk_AxisAlignedBox* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_castToIPose(isdk_PointableOrientedBox* instance, isdk_IPose** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_castFromIPose(isdk_IPose* instance, isdk_PointableOrientedBox** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_castToIBounds(isdk_PointableOrientedBox* instance, isdk_IBounds** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_castFromIBounds(
    isdk_IBounds* instance,
    isdk_PointableOrientedBox** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_castToISurface(
    isdk_PointableOrientedBox* instance,
    isdk_ISurface** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_castFromISurface(
    isdk_ISurface* instance,
    isdk_PointableOrientedBox** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_create(isdk_PointableOrientedBox** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_createWithPose(
    const ovrpPosef* pose,
    const ovrpVector3f* size,
    isdk_PointableOrientedBox** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_destroy(isdk_PointableOrientedBox* instance);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_raycast(
    isdk_PointableOrientedBox* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_closestSurfacePoint(
    isdk_PointableOrientedBox* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_setPoseAndSize(
    isdk_PointableOrientedBox* instance,
    const ovrpPosef* centroid,
    const ovrpVector3f* size);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_setPose(isdk_PointableOrientedBox* instance, const ovrpPosef* centroid);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_setSize(isdk_PointableOrientedBox* instance, const ovrpVector3f* size);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_getPose(isdk_PointableOrientedBox* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableOrientedBox_getBounds(
    isdk_PointableOrientedBox* instance,
    isdk_AxisAlignedBox* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_castToIPose(isdk_PointablePlane* instance, isdk_IPose** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_castFromIPose(isdk_IPose* instance, isdk_PointablePlane** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_castToIBounds(isdk_PointablePlane* instance, isdk_IBounds** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_castFromIBounds(isdk_IBounds* instance, isdk_PointablePlane** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_castToISurface(isdk_PointablePlane* instance, isdk_ISurface** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_castFromISurface(isdk_ISurface* instance, isdk_PointablePlane** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_create(isdk_PointablePlane** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_createWithPose(
    const ovrpPosef* pose,
    const ovrpVector2f* size,
    isdk_PointablePlane** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_destroy(isdk_PointablePlane* instance);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_raycast(
    isdk_PointablePlane* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_closestSurfacePoint(
    isdk_PointablePlane* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_setPose(isdk_PointablePlane* instance, const ovrpPosef* pose);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_setSize(isdk_PointablePlane* instance, const ovrpVector2f* newSize);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_getNormal(isdk_PointablePlane* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_getSize(isdk_PointablePlane* instance, ovrpVector2f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_getPose(isdk_PointablePlane* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointablePlane_getBounds(isdk_PointablePlane* instance, isdk_AxisAlignedBox* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_castToIPose(
    isdk_PointableTransformableUnitSquare* instance,
    isdk_IPose** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_castFromIPose(
    isdk_IPose* instance,
    isdk_PointableTransformableUnitSquare** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_castToIBounds(
    isdk_PointableTransformableUnitSquare* instance,
    isdk_IBounds** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_castFromIBounds(
    isdk_IBounds* instance,
    isdk_PointableTransformableUnitSquare** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_castToISurface(
    isdk_PointableTransformableUnitSquare* instance,
    isdk_ISurface** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_castFromISurface(
    isdk_ISurface* instance,
    isdk_PointableTransformableUnitSquare** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_create(
    ovrpVector3f position,
    ovrpVector3f size,
    isdk_PointableTransformableUnitSquare** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_destroy(isdk_PointableTransformableUnitSquare* instance);

INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_raycast(
    isdk_PointableTransformableUnitSquare* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_closestSurfacePoint(
    isdk_PointableTransformableUnitSquare* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_getPose(
    isdk_PointableTransformableUnitSquare* instance,
    ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_getBounds(
    isdk_PointableTransformableUnitSquare* instance,
    isdk_AxisAlignedBox* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointableTransformableUnitSquare_setTransform(
    isdk_PointableTransformableUnitSquare* instance,
    const ovrpPosef* centroid,
    const ovrpVector3f* scale);

INTERACTIONSDK_EXPORT(void)
isdk_PointerEventQueue_create(isdk_IPointable* pointable, isdk_PointerEventQueue** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointerEventQueue_destroy(isdk_PointerEventQueue* instance);

INTERACTIONSDK_EXPORT(void)
isdk_PointerEventQueue_isEmpty(isdk_PointerEventQueue* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointerEventQueue_getCount(isdk_PointerEventQueue* instance, unsigned long long* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointerEventQueue_pop(isdk_PointerEventQueue* instance, isdk_PointerEvent* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PointerEventQueue_clear(isdk_PointerEventQueue* instance);

/**
 * \param pokeInteractable Watches this interactable for any interactors that enter the hover
 * state
 * \param basePose The location of the visual at its maximum state of depression - i.e. when the
 * poke interactable has InteractableState::Select
 * \param maxExtent How far along the backward axis of the base pose the visual will extend when
 * there are no interactors hovering.
 */
INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_create(
    isdk_PokeInteractable* pokeInteractable,
    ovrpPosef basePose,
    float maxExtent,
    isdk_PokeButtonVisual** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_destroy(isdk_PokeButtonVisual* instance);

INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_getBasePose(isdk_PokeButtonVisual* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_getMaxExtent(isdk_PokeButtonVisual* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_getCurrentExtent(isdk_PokeButtonVisual* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_getCurrentPose(isdk_PokeButtonVisual* instance, ovrpPosef* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_setPokeInteractable(
    isdk_PokeButtonVisual* instance,
    isdk_PokeInteractable* pokeInteractable,
    ovrpPosef basePose,
    float extentDistance);

INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_updateBasePose(
    isdk_PokeButtonVisual* instance,
    const ovrpPosef* basePose,
    float extentDistance);

/**
 * \brief Determines the best visual pose for the the given interactable by iterating over all
 * interactors that are in the hovering state.
 * \param interactable queried for all PokeInteractors that are in the Hover state
 * \param maxExtent How far along the backward axis of the base pose the visual will extend when
 * there are no interactors hovering.
 * \return Calculated extent for the visual
 */
INTERACTIONSDK_EXPORT(void)
isdk_PokeButtonVisual_calculateExtentFromState(
    isdk_PokeInteractable* interactable,
    float maxExtent,
    float* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_castToIPointable(isdk_PokeInteractable* instance, isdk_IPointable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_castFromIPointable(isdk_IPointable* instance, isdk_PokeInteractable** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_castToIInteractable(
    isdk_PokeInteractable* instance,
    isdk_IInteractable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_castFromIInteractable(
    isdk_IInteractable* instance,
    isdk_PokeInteractable** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_castToIEnable(isdk_PokeInteractable* instance, isdk_IEnable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_castFromIEnable(isdk_IEnable* instance, isdk_PokeInteractable** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_create(
    const isdk_PokeInteractable_Config* config,
    isdk_ISurfacePatch* surfacePatch,
    isdk_IPointableElement* pointableElement,
    isdk_PokeInteractable** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_destroy(isdk_PokeInteractable* instance);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_closestSurfacePatchHit(
    isdk_PokeInteractable* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* hit,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_closestBackingSurfaceHit(
    isdk_PokeInteractable* instance,
    const ovrpVector3f* point,
    isdk_SurfaceHit* hit,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getSurfacePatch(isdk_PokeInteractable* instance, isdk_ISurfacePatch** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getEnterHoverNormal(isdk_PokeInteractable* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getEnterHoverTangent(isdk_PokeInteractable* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getExitHoverNormal(isdk_PokeInteractable* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getExitHoverTangent(isdk_PokeInteractable* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getCancelSelectNormal(isdk_PokeInteractable* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getCancelSelectTangent(isdk_PokeInteractable* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_setSurfacePatch(
    isdk_PokeInteractable* instance,
    isdk_ISurfacePatch* surfacePatch);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getConfig(
    isdk_PokeInteractable* instance,
    isdk_PokeInteractable_Config* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_setConfig(
    isdk_PokeInteractable* instance,
    const isdk_PokeInteractable_Config* config);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getPositionPinning(
    isdk_PokeInteractable* instance,
    isdk_PokeInteractable_PositionPinningConfig* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_setPositionPinning(
    isdk_PokeInteractable* instance,
    const isdk_PokeInteractable_PositionPinningConfig* positionPinningConfig);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getDragThresholds(
    isdk_PokeInteractable* instance,
    isdk_PokeInteractable_DragThresholdingConfig* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getMinThresholds(
    isdk_PokeInteractable* instance,
    isdk_PokeInteractable_MinThresholdsConfig* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getCloseDistanceThreshold(isdk_PokeInteractable* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_setCloseDistanceThreshold(isdk_PokeInteractable* instance, float paramVar0);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_getTiebreakerScore(isdk_PokeInteractable* instance, int* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractable_setTiebreakerScore(isdk_PokeInteractable* instance, int score);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castToIInteractorView(
    isdk_PokeInteractor* instance,
    isdk_IInteractorView** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castFromIInteractorView(
    isdk_IInteractorView* instance,
    isdk_PokeInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castToIUpdate(isdk_PokeInteractor* instance, isdk_IUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castFromIUpdate(isdk_IUpdate* instance, isdk_PokeInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castToIUpdateDriver(isdk_PokeInteractor* instance, isdk_IUpdateDriver** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castFromIUpdateDriver(
    isdk_IUpdateDriver* instance,
    isdk_PokeInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castToIInteractor(isdk_PokeInteractor* instance, isdk_IInteractor** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castFromIInteractor(isdk_IInteractor* instance, isdk_PokeInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castToIEnable(isdk_PokeInteractor* instance, isdk_IEnable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castFromIEnable(isdk_IEnable* instance, isdk_PokeInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castToIHasSelector(isdk_PokeInteractor* instance, isdk_IHasSelector** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_castFromIHasSelector(isdk_IHasSelector* instance, isdk_PokeInteractor** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_createWithOptions(
    const isdk_PokeInteractor_Config* config,
    isdk_IActiveState* activeState,
    isdk_IPayload* payload,
    isdk_IPosition* positionProvider,
    isdk_ITimeProvider* timeProvider,
    isdk_PokeInteractor** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_create(isdk_PokeInteractor** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_destroy(isdk_PokeInteractor* instance);

/**
 \brief If state is Disabled, replaces existing config with the given value.

 The interactor must be in state Disabled prior to calling this method.
 If the interactor is not in a disabled state, it will raise an assertion (debug builds) and not
 perform config update.
 */
INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_setConfig(
    isdk_PokeInteractor* instance,
    const isdk_PokeInteractor_Config* config);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_setPointTransform(
    isdk_PokeInteractor* instance,
    const ovrpPosef* pointTransform);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_setPointPosition(
    isdk_PokeInteractor* instance,
    const ovrpVector3f* pointPosition);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_getClosestPoint(isdk_PokeInteractor* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_getTouchPoint(isdk_PokeInteractor* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_getTouchNormal(isdk_PokeInteractor* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_getRadius(isdk_PokeInteractor* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_getOrigin(isdk_PokeInteractor* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_isPassedSurface(isdk_PokeInteractor* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_isDebouncing(isdk_PokeInteractor* instance, unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_computeDistanceAbove(
    isdk_PokeInteractable* interactable,
    const ovrpVector3f* point,
    float radius,
    float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_computeDepth(
    isdk_PokeInteractable* interactable,
    const ovrpVector3f* point,
    float radius,
    float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_computeDistanceFrom(
    isdk_PokeInteractable* interactable,
    const ovrpVector3f* point,
    float radius,
    float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_computeTangentDistance(
    isdk_PokeInteractable* interactable,
    const ovrpVector3f* point,
    float radius,
    float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_PokeInteractor_computeLateralDistanceToClosestPoint(
    const ovrpVector3f* point,
    float radius,
    const ovrpVector3f* closestProximityPoint,
    const ovrpVector3f* closestSurfaceNormal,
    float* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_castToIPointable(isdk_RayInteractable* instance, isdk_IPointable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_castFromIPointable(isdk_IPointable* instance, isdk_RayInteractable** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_castToIInteractable(
    isdk_RayInteractable* instance,
    isdk_IInteractable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_castFromIInteractable(
    isdk_IInteractable* instance,
    isdk_RayInteractable** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_castToIEnable(isdk_RayInteractable* instance, isdk_IEnable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_castFromIEnable(isdk_IEnable* instance, isdk_RayInteractable** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_create(
    isdk_ISurface* surface,
    isdk_IPointableElement* pointableElement,
    isdk_ISurface* selectSurface,
    isdk_IMovementProvider* movementProvider,
    isdk_ISurface* magnetismSurface,
    isdk_RayInteractable** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_destroy(isdk_RayInteractable* instance);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_getSurface(isdk_RayInteractable* instance, isdk_ISurface** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_raycastSelectSurface(
    isdk_RayInteractable* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_raycastMagnetismSurface(
    isdk_RayInteractable* instance,
    const isdk_Ray* ray,
    isdk_SurfaceHit* outHit,
    float maxDistance,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_setSurface(isdk_RayInteractable* instance, isdk_ISurface* surface);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_setSelectSurface(isdk_RayInteractable* instance, isdk_ISurface* surface);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_getTiebreakerScore(isdk_RayInteractable* instance, int* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractable_setTiebreakerScore(isdk_RayInteractable* instance, int score);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castToIInteractorView(
    isdk_RayInteractor* instance,
    isdk_IInteractorView** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castFromIInteractorView(
    isdk_IInteractorView* instance,
    isdk_RayInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castToIUpdate(isdk_RayInteractor* instance, isdk_IUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castFromIUpdate(isdk_IUpdate* instance, isdk_RayInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castToIUpdateDriver(isdk_RayInteractor* instance, isdk_IUpdateDriver** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castFromIUpdateDriver(isdk_IUpdateDriver* instance, isdk_RayInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castToIInteractor(isdk_RayInteractor* instance, isdk_IInteractor** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castFromIInteractor(isdk_IInteractor* instance, isdk_RayInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castToIEnable(isdk_RayInteractor* instance, isdk_IEnable** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castFromIEnable(isdk_IEnable* instance, isdk_RayInteractor** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castToIHasSelector(isdk_RayInteractor* instance, isdk_IHasSelector** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_castFromIHasSelector(isdk_IHasSelector* instance, isdk_RayInteractor** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_create(
    const isdk_RayInteractor_Config* config,
    isdk_IPose* poseProvider,
    isdk_ISelector* selector,
    isdk_IActiveState* activeState,
    isdk_IPayload* payload,
    isdk_RayInteractor** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_destroy(isdk_RayInteractor* instance);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getOrigin(isdk_RayInteractor* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getRotation(isdk_RayInteractor* instance, ovrpQuatf* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getMaxRayLength(isdk_RayInteractor* instance, float* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getForward(isdk_RayInteractor* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getUp(isdk_RayInteractor* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getEnd(isdk_RayInteractor* instance, ovrpVector3f* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getCollisionInfo(isdk_RayInteractor* instance, isdk_OptionalSurfaceHit* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_setOrigin(isdk_RayInteractor* instance, ovrpVector3f origin);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_setRotation(isdk_RayInteractor* instance, ovrpQuatf rotation);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_setMaxRayLength(isdk_RayInteractor* instance, float maxRayLength);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_setRayOrigin(isdk_RayInteractor* instance, const ovrpPosef* rayOrigin);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_setLocalForwardVector(isdk_RayInteractor* instance, const ovrpVector3f vec);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_setLocalUpVector(isdk_RayInteractor* instance, const ovrpVector3f vec);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getConfig(isdk_RayInteractor* instance, isdk_RayInteractor_Config* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_setConfig(isdk_RayInteractor* instance, const isdk_RayInteractor_Config* config);

INTERACTIONSDK_EXPORT(void)
isdk_RayInteractor_getCandidateProperties(
    isdk_RayInteractor* instance,
    isdk_ICandidateProperty** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ScaledTimeProvider_castToITimeProvider(
    isdk_ScaledTimeProvider* instance,
    isdk_ITimeProvider** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ScaledTimeProvider_castFromITimeProvider(
    isdk_ITimeProvider* instance,
    isdk_ScaledTimeProvider** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ScaledTimeProvider_create(isdk_ScaledTimeProvider** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ScaledTimeProvider_destroy(isdk_ScaledTimeProvider* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ScaledTimeProvider_setScaledTimeSeconds(
    isdk_ScaledTimeProvider* instance,
    double scaledTimeSeconds);

INTERACTIONSDK_EXPORT(void)
isdk_ScaledTimeProvider_getScaledTimeSeconds(isdk_ScaledTimeProvider* instance, double* retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_Selector_castToISelector(isdk_Selector* instance, isdk_ISelector** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_Selector_castFromISelector(isdk_ISelector* instance, isdk_Selector** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_Selector_create(isdk_Selector** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_Selector_destroy(isdk_Selector* instance);

INTERACTIONSDK_EXPORT(void)
isdk_Selector_select(isdk_Selector* instance);

INTERACTIONSDK_EXPORT(void)
isdk_Selector_unselect(isdk_Selector* instance);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_castToIHandDataSource(
    isdk_SyntheticHand* instance,
    isdk_IHandDataSource** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_castFromIHandDataSource(
    isdk_IHandDataSource* instance,
    isdk_SyntheticHand** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_castToIHandDataModifier(
    isdk_SyntheticHand* instance,
    isdk_IHandDataModifier** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_castFromIHandDataModifier(
    isdk_IHandDataModifier* instance,
    isdk_SyntheticHand** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_create(
    isdk_IHandDataSource* fromDataSource,
    unsigned char recursiveUpdate,
    isdk_SyntheticHand_Config config,
    isdk_SyntheticHand** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_destroy(isdk_SyntheticHand* instance);

INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_isFlagSet(
    isdk_SyntheticHand_WristLockMode statusFlags,
    isdk_SyntheticHand_WristLockMode flagToLookFor,
    unsigned char* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_lockWristPose(
    isdk_SyntheticHand* instance,
    const ovrpPosef* wristPose,
    const float overrideFactor,
    isdk_SyntheticHand_WristLockMode lockMode,
    unsigned char skipAnimation);

INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_lockWristPosition(
    isdk_SyntheticHand* instance,
    const ovrpVector3f* position,
    const float overrideFactor,
    unsigned char skipAnimation);

INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_lockWristRotation(
    isdk_SyntheticHand* instance,
    const ovrpQuatf* rotation,
    const float overrideFactor,
    unsigned char skipAnimation);

INTERACTIONSDK_EXPORT(void)
isdk_SyntheticHand_freeWrist(
    isdk_SyntheticHand* instance,
    isdk_SyntheticHand_WristLockMode lockMode);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_castToIActiveState(isdk_ThumbRecognizer* instance, isdk_IActiveState** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_castFromIActiveState(
    isdk_IActiveState* instance,
    isdk_ThumbRecognizer** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_castToIDeltaTimeUpdate(
    isdk_ThumbRecognizer* instance,
    isdk_IDeltaTimeUpdate** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_castFromIDeltaTimeUpdate(
    isdk_IDeltaTimeUpdate* instance,
    isdk_ThumbRecognizer** retVal);

// Performs a type cast to an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_castToDigitRecognizer(
    isdk_ThumbRecognizer* instance,
    isdk_DigitRecognizer** retVal);

// Performs a type cast from an interface type. Returns null if the cast is invalid.
INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_castFromDigitRecognizer(
    isdk_DigitRecognizer* instance,
    isdk_ThumbRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_create(
    isdk_IHandPositionFrame* handPositionFrame,
    const isdk_DigitRangeParams* parameters,
    isdk_Detection_ThumbCalcType calcType,
    isdk_ThumbRecognizer** retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_destroy(isdk_ThumbRecognizer* instance);

INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_getExpectedThumbRawValueRanges(
    isdk_ThumbRecognizer_ExpectedThumbValueRanges* retVal);

INTERACTIONSDK_EXPORT(void)
isdk_ThumbRecognizer_getThumbRangeParamsFromNormalized(
    const isdk_DigitRangeParams* normalizedParams,
    isdk_Detection_ThumbCalcType calcType,
    isdk_DigitRangeParams* retVal);

#endif // ISDK_API_H
