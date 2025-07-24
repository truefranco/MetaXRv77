#ifndef METAX1_FACE_TRACKING_VISEMES_H_
#define METAX1_FACE_TRACKING_VISEMES_H_ 1

/**********************
This file is @generated from the OpenXR XML API registry.
Language    :   C99
Copyright   :   (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
***********************/

#include <openxr/openxr.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef XR_METAX1_face_tracking_visemes

// XR_METAX1_face_tracking_visemes is a preprocessor guard. Do not pass it to API calls.
#define XR_METAX1_face_tracking_visemes 1
#define XR_FACE_TRACKING_VISEME_COUNT_METAX1 15
#define XR_METAX1_face_tracking_visemes_SPEC_VERSION 1
#define XR_METAX1_FACE_TRACKING_VISEMES_EXTENSION_NAME "XR_METAX1_face_tracking_visemes"
static const XrStructureType XR_TYPE_FACE_TRACKING_VISEMES_METAX1 = (XrStructureType) 1000541000;
static const XrStructureType XR_TYPE_SYSTEM_FACE_TRACKING_VISEMES_PROPERTIES_METAX1 = (XrStructureType) 1000541001;

typedef enum XrFaceTrackingVisemeMETAX1 {
    XR_FACE_TRACKING_VISEME_SIL_METAX1 = 0,
    XR_FACE_TRACKING_VISEME_PP_METAX1 = 1,
    XR_FACE_TRACKING_VISEME_FF_METAX1 = 2,
    XR_FACE_TRACKING_VISEME_TH_METAX1 = 3,
    XR_FACE_TRACKING_VISEME_DD_METAX1 = 4,
    XR_FACE_TRACKING_VISEME_KK_METAX1 = 5,
    XR_FACE_TRACKING_VISEME_CH_METAX1 = 6,
    XR_FACE_TRACKING_VISEME_SS_METAX1 = 7,
    XR_FACE_TRACKING_VISEME_NN_METAX1 = 8,
    XR_FACE_TRACKING_VISEME_RR_METAX1 = 9,
    XR_FACE_TRACKING_VISEME_AA_METAX1 = 10,
    XR_FACE_TRACKING_VISEME_E_METAX1 = 11,
    XR_FACE_TRACKING_VISEME_IH_METAX1 = 12,
    XR_FACE_TRACKING_VISEME_OH_METAX1 = 13,
    XR_FACE_TRACKING_VISEME_OU_METAX1 = 14,
    XR_FACE_TRACKING_VISEME_METAX1_MAX_ENUM = 0x7FFFFFFF
} XrFaceTrackingVisemeMETAX1;
typedef struct XrFaceTrackingVisemesMETAX1 {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrBool32                    isValid;
    float                       visemes[XR_FACE_TRACKING_VISEME_COUNT_METAX1];
} XrFaceTrackingVisemesMETAX1;

typedef struct XrSystemFaceTrackingVisemesPropertiesMETAX1 {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrBool32              supportsVisemes;
} XrSystemFaceTrackingVisemesPropertiesMETAX1;

#endif /* XR_METAX1_face_tracking_visemes */

#ifdef __cplusplus
}
#endif

#endif
