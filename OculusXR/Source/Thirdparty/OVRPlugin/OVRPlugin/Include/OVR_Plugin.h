/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 *
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OVR_Plugin_h
#define OVR_Plugin_h

#include "OVR_Plugin_Types.h"

// OVR_Plugin.h: Minimal high-level plugin proxy to LibOVR. Use instead of OVR_CAPI.h.
// All functions must be called from the same thread as your graphics device, except as noted.

#ifdef __cplusplus
extern "C" {
#endif

// The following macros are only defined when building in UE4 and UE5
#if (                                                                                     \
    defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT) || defined(UE_BUILD_TEST) || \
    defined(UE_BUILD_SHIPPING)) &&                                                        \
    defined(PLATFORM_ANDROID)
// OVRPlugin does not support Android system callbacks in UE4 and UE5.
// Please use Optional Mobile Features Blueprint Library Plugin or include FAndroidMisc.h in your project
#define OVRPLUGIN_JNI_LIB_EXCLUDED
#endif

/// Initializes the Oculus display driver before graphics initialization, if applicable.
OVRP_EXPORT ovrpResult
ovrp_PreInitialize5(void* activity, ovrpRenderAPIType apiType, ovrpPreinitializeFlags preinitializeFlags);

/// Gets the current initialization state of the Oculus runtime, VR tracking, and graphics
/// resources.
OVRP_EXPORT ovrpBool ovrp_GetInitialized();

/// Sets up the Oculus runtime, VR tracking, and graphics resources.
/// You must call this before any other function except ovrp_PreInitialize() or
/// ovrp_GetInitialized().
/// @note In case of D3D12, d3dDevice is an ID3D12CommandQueue object
OVRP_EXPORT ovrpResult ovrp_Initialize7(
    ovrpRenderAPIType apiType,
    ovrpLogCallback logCallback,
    void* activity,
    void* vkInstance,
    void* vkPhysicalDevice,
    void* vkDevice,
    void* vkQueue,
    void* vkGetInstanceProcAddr, // PFN_vkGetInstanceProcAddr
    unsigned int vkQueueFamilyIndex,
    void* d3dDevice,
    int initializeFlags,
    OVRP_CONSTREF(ovrpVersion) version);

/// Tears down the Oculus runtime, VR tracking, and graphics resources.
OVRP_EXPORT ovrpResult ovrp_Shutdown2();

/// Set a function pointer which captures the OVRPlugin log lines.
OVRP_EXPORT ovrpResult ovrp_SetLogCallback2(ovrpLogCallback2 logFunc);

/// Gets the version of OVRPlugin currently in use. Format: "major.minor.release"
OVRP_EXPORT ovrpResult ovrp_GetVersion2(char const** version);

/// Gets the version of the underlying VR SDK currently in use.
OVRP_EXPORT ovrpResult ovrp_GetNativeSDKVersion2(char const** nativeSDKVersion);

/// Returns a pointer that can be used to access the underlying VR SDK
/// (e.g. ovrSession in CAPI, ovrMobile* in VRAPI, XrSession* in OpenXR).
OVRP_EXPORT ovrpResult ovrp_GetNativeSDKPointer2(void** nativeSDKPointer);

/// Retrieve the current XR API being used by OVRPlugin
OVRP_EXPORT ovrpResult ovrp_GetNativeXrApiType(ovrpXrApi* xrApi);

/// Retrive XrInstance / XrSession when OpenXR is being used
OVRP_EXPORT ovrpResult ovrp_GetNativeOpenXRHandles(ovrpUInt64* xrInstance, ovrpUInt64* xrSession);

/// Retrieves the expected Display Adapter ID associated with the Oculus HMD.
/// On Windows systems, this will return a DX11 LUID, otherwise nullptr.
/// @note ovrp_PreInitialize must be called and return a successful result before calling this
/// function.
OVRP_EXPORT ovrpResult ovrp_GetDisplayAdapterId2(void const** displayAdapterId);

/// Retrieves the expected audio device ID associated with the Oculus HMD's headphones.
/// On Windows systems, this will return the GUID* for the IMMDevice of an audio endpoint.
/// @note ovrp_PreInitialize must be called and return a successful result before calling this
/// function.
OVRP_EXPORT ovrpResult ovrp_GetAudioOutId2(void const** audioOutId);

/// Retrieves the expected audio device ID associated with the Oculus HMD's headphones.
/// On Windows systems, this will return a LPCWSTR containing the device identifier.
/// @note ovrp_PreInitialize must be called and return a successful result before calling this
/// function.
OVRP_EXPORT ovrpResult ovrp_GetAudioOutDeviceId2(void const** audioOutDeviceId);

/// Retrieves the expected audio device ID associated with the Oculus HMD's microphone.
/// On Windows systems, this will return the GUID* for the IMMDevice of an audio endpoint.
/// @note ovrp_PreInitialize must be called and return a successful result before calling this
/// function.
OVRP_EXPORT ovrpResult ovrp_GetAudioInId2(void const** audioInId);

/// Retrieves the expected audio device ID associated with the Oculus HMD's microphone.
/// On Windows systems, this will return a LPCWSTR containing the device identifier.
/// @note ovrp_PreInitialize must be called and return a successful result before calling this
/// function.
OVRP_EXPORT ovrpResult ovrp_GetAudioInDeviceId2(void const** audioInDeviceId);

/// Returns an array of pointers to extension names which need to be enabled for the instance
/// in order for the VR runtime to support Vulkan-based applications.
/// @note ovrp_PreInitialize must be called and return a successful result before calling this
/// function.
OVRP_EXPORT ovrpResult ovrp_GetInstanceExtensionsVk(char const** instanceExtensions, int* instanceExtensionCount);

/// Returns an array of pointers to extension names which need to be enabled for the device
/// in order for the VR runtime to support Vulkan-based applications.
/// @note ovrp_PreInitialize must be called and return a successful result before calling this
/// function.
OVRP_EXPORT ovrpResult ovrp_GetDeviceExtensionsVk(char const** deviceExtensions, int* deviceExtensionCount);

/// Creates a dedicated window for rendering 3D content to the VR display.
OVRP_EXPORT ovrpResult ovrp_SetupDistortionWindow3(int flags);

/// Destroys the dedicated VR window.
OVRP_EXPORT ovrpResult ovrp_DestroyDistortionWindow2();

// Returns handedness as specified in the mobile device
OVRP_EXPORT ovrpResult ovrp_GetDominantHand(ovrpHandedness* dominantHand);

/// Used by System Activities application for setting the Remote Handedness.
OVRP_EXPORT ovrpResult ovrp_SetRemoteHandedness(ovrpHandedness handedness);

// Sets color scale parameters; can be used for effects like fade-to-black. Final pixel color will be multiplied by
// colorScale and added to offset. If applyToAllLayers is false, this applies only for the eyefov layer. If it's true,
// it's for every layer submitted.
OVRP_EXPORT ovrpResult ovrp_SetColorScaleAndOffset(
    const ovrpVector4f colorScale,
    const ovrpVector4f colorOffset,
    const ovrpBool applyToAllLayers);

/// Creates a layer.
/// The desc remains constant for the lifetime of the layer.
/// @note In case of D3D12, device is an ID3D12CommandQueue object
OVRP_EXPORT ovrpResult ovrp_SetupLayer(void* device, OVRP_CONSTREF(ovrpLayerDesc) desc, int* layerId);

/// Create depth swap chain for a layer
OVRP_EXPORT ovrpResult ovrp_SetupLayerDepth(void* device, ovrpTextureFormat depthFormat, int layerId);

/// Get Eye Fov layer index if created
/// Otherwise return fail
OVRP_EXPORT ovrpResult ovrp_GetEyeFovLayerId(int* layerId);

/// Set blending mode of Eye Fov layer to use premultiplied alpha or not
OVRP_EXPORT ovrpResult ovrp_SetEyeFovPremultipliedAlphaMode(const ovrpBool enabled);

/// Get premultiplied alpha mode of the Eye Fov layer
OVRP_EXPORT ovrpResult ovrp_GetEyeFovPremultipliedAlphaMode(ovrpBool* enabled);

/// Gets the number of texture stages in the layer.
/// Layers have multiple stages, unless the ovrpLayer_Static flag was specified.
OVRP_EXPORT ovrpResult ovrp_GetLayerTextureStageCount(int layerId, int* layerTextureStageCount);

/// Gets the texture handle for a specific layer stage and eye.
OVRP_EXPORT ovrpResult ovrp_GetLayerTexture2(
    int layerId,
    int stage,
    ovrpEye eyeId,
    ovrpTextureHandle* textureHandle,
    ovrpTextureHandle* depthTextureHandle);

/// Gets the texture handle for a specific layer stage and eye.
OVRP_EXPORT ovrpResult ovrp_GetLayerTextureFoveation(
    int layerId,
    int stage,
    ovrpEye eyeId,
    ovrpTextureHandle* foveationTextureHandle,
    ovrpSizei* foveationResultSize);

/// Gets the space warp texture handles for a specific layer stage and eye.
OVRP_EXPORT ovrpResult ovrp_GetLayerTextureSpaceWarp(
    int layerId,
    int stage,
    ovrpEye eyeId,
    ovrpTextureHandle* motionVectorTextureHandle,
    ovrpSizei* motionVectorResultSize,
    ovrpTextureHandle* depthTextureHandle,
    ovrpSizei* depthResultSize);

/// Gets the space warp texture handles for a specific layer stage and eye.
OVRP_EXPORT ovrpResult ovrp_GetLayerTextureSpaceWarp2(
    int layerId,
    int stage,
    ovrpEye eyeId,
    ovrpTextureHandle* motionVectorTextureHandle,
    ovrpSizei* motionVectorResultSize,
    ovrpTextureHandle* depthTextureHandle,
    ovrpSizei* depthResultSize,
    ovrpTextureHandle* foveationTextureHandle,
    ovrpSizei* foveationResultSize);

/// Gets the motion vector texture handles for a specific layer stage and eye.
OVRP_EXPORT ovrpResult ovrp_GetLayerMotionVectorTextures(
    int layerId,
    int stage,
    ovrpEye eyeId,
    ovrpTextureHandle* motionVectorTextureHandle,
    ovrpSizei* motionVectorResultSize,
    ovrpTextureHandle* depthTextureHandle,
    ovrpSizei* depthResultSize,
    ovrpTextureHandle* foveationTextureHandle,
    ovrpSizei* foveationResultSize);

/// Gets the texture handle for a specific layer stage and eye.
OVRP_EXPORT ovrpResult ovrp_GetLayerAndroidSurfaceObject(int layerId, void** surfaceObject);

/// Return the vertices and indices for the eye occlusion mesh.
OVRP_EXPORT ovrpResult ovrp_GetLayerOcclusionMesh(
    int layerId,
    ovrpEye eyeId,
    ovrpVector2f const** vertices,
    int* vertexCount,
    int const** indices,
    int* indexCount);

/// Destroys a layer
OVRP_EXPORT ovrpResult ovrp_DestroyLayer(int layerId);

/// Calculates layer description
OVRP_EXPORT ovrpResult ovrp_CalculateLayerDesc(
    ovrpShape shape,
    ovrpLayout layout,
    OVRP_CONSTREF(ovrpSizei) textureSize,
    int mipLevels,
    int sampleCount,
    ovrpTextureFormat format,
    int layerFlags,
    ovrpLayerDescUnion* layerDesc);

/// Calculates eye layer description
OVRP_EXPORT ovrpResult ovrp_CalculateEyeLayerDesc2(
    ovrpLayout layout,
    float textureScale,
    int mipLevels,
    int sampleCount,
    ovrpTextureFormat format,
    ovrpTextureFormat depthFormat,
    int layerFlags,
    ovrpLayerDesc_EyeFov* layerDesc);

/// Calculates eye layer description
OVRP_EXPORT ovrpResult ovrp_CalculateEyeLayerDesc3(
    ovrpLayout layout,
    float textureScale,
    int mipLevels,
    int sampleCount,
    ovrpTextureFormat format,
    ovrpTextureFormat depthFormat,
    ovrpTextureFormat motionVectorFormat,
    ovrpTextureFormat motionVectorDepthFormat,
    float motionVectorTextureScale,
    int layerFlags,
    ovrpLayerDesc_EyeFov* layerDesc);

/// Calculates the recommended viewport rect for the specified eye
OVRP_EXPORT ovrpResult ovrp_CalculateEyeViewportRect(
    OVRP_CONSTREF(ovrpLayerDesc_EyeFov) layerDesc,
    ovrpEye eyeId,
    float viewportScale,
    ovrpRecti* viewportRect);

/// Calculates the area of the viewport unobstructed by the occlusion mesh
OVRP_EXPORT ovrpResult ovrp_CalculateEyePreviewRect(
    OVRP_CONSTREF(ovrpLayerDesc_EyeFov) layerDesc,
    ovrpEye eyeId,
    OVRP_CONSTREF(ovrpRecti) viewportRect,
    ovrpRecti* previewRect);

/// Allocates mirror texture
/// If you called ovrp_Initialize with ovrpRenderAPI_D3D11, pass device argument
/// here to have the texture allocated by that device.
/// If you called ovrp_Initialize with ovrpRenderAPI_OpenGL, you can pass
/// a texture ID allocated by glGenTextures in the device argument and the texture will be
/// associated with that ID.
/// @note In case of D3D12, device is an ID3D12CommandQueue object
OVRP_EXPORT ovrpResult ovrp_SetupMirrorTexture2(
    void* device,
    int height,
    int width,
    ovrpTextureFormat format,
    ovrpTextureHandle* textureHandle);

/// Destroys mirror texture.
OVRP_EXPORT ovrpResult ovrp_DestroyMirrorTexture2();

/// Returns the recommended amount to scale GPU work in order to maintain framerate.
/// Can be used to adjust viewportScale and textureScale
OVRP_EXPORT ovrpResult ovrp_GetAdaptiveGpuPerformanceScale2(float* adapiveGpuPerformanceScale);

/// Returns the time from CPU start to GPU end, a meaningful performance metric under OVR
OVRP_EXPORT ovrpResult ovrp_GetAppCpuStartToGpuEndTime2(float* appCpuStartToGpuEndTime);

/// Return how many display pixels will fit in tan(angle) = 1
OVRP_EXPORT ovrpResult ovrp_GetEyePixelsPerTanAngleAtCenter2(int eyeIndex, ovrpVector2f* pixelsPerTanAngleAtCenter);

/// Return the offset HMD to the eye, in meters
OVRP_EXPORT ovrpResult ovrp_GetHmdToEyeOffset2(int eyeIndex, ovrpVector3f* hmdToEyeOffset);

/// Ensures VR rendering is configured and updates tracking to reflect the latest reported poses.
/// You must call ovrp_Update before calling ovrp_GetNode* for a new frame.
/// Call with ovrpStep_Render from end of frame on Game thread, to hand off state to Render thread
/// Call with ovrpStep_Physics from any thread, using predictionSeconds specify offset from start of
/// frame.
OVRP_EXPORT ovrpResult ovrp_Update3(ovrpStep step, int frameIndex, double predictionSeconds);

/// Marks the beginning of a frame. Call this before issuing any graphics commands in a given frame.
OVRP_EXPORT ovrpResult ovrp_WaitToBeginFrame(int frameIndex);

/// Marks the beginning of a frame. Call this before issuing any graphics commands in a given frame.
OVRP_EXPORT ovrpResult ovrp_BeginFrame4(int frameIndex, void* commandQueue);

/// Late update of foveation parameters, both GL and Vulkan
OVRP_EXPORT ovrpResult ovrp_UpdateFoveation(int frameIndex);

/// Marks the end of a frame and performs TimeWarp. Call this before Present or SwapBuffers to
/// update the VR window.
OVRP_EXPORT ovrpResult
ovrp_EndFrame4(int frameIndex, ovrpLayerSubmit const* const* layerSubmitPtrs, int layerSubmitCount, void* commandQueue);

/// If true, the HMD supports orientation tracking.
OVRP_EXPORT ovrpResult ovrp_GetTrackingOrientationSupported2(ovrpBool* trackingOrientationSupported);

/// If true, head tracking affects the rotation reported by ovrp_GetEyePose.
OVRP_EXPORT ovrpResult ovrp_GetTrackingOrientationEnabled2(ovrpBool* trackingOrientationEnabled);

/// If true, head tracking affects the rotation reported by ovrp_GetEyePose.
OVRP_EXPORT ovrpResult ovrp_SetTrackingOrientationEnabled2(ovrpBool trackingOrientationEnabled);

/// If true, the HMD supports position tracking
OVRP_EXPORT ovrpResult ovrp_GetTrackingPositionSupported2(ovrpBool* trackingPositionSupported);

/// If true, head tracking affects the position reported by ovrp_GetEyePose.
OVRP_EXPORT ovrpResult ovrp_GetTrackingPositionEnabled2(ovrpBool* trackingPositionEnabled);

/// If true, head tracking affects the position reported by ovrp_GetEyePose.
OVRP_EXPORT ovrpResult ovrp_SetTrackingPositionEnabled2(ovrpBool trackingPositionEnabled);

/// If true, head tracking affects the pose reported by ovrp_GetEyePose even for invisible sessions.
OVRP_EXPORT ovrpResult ovrp_GetTrackingPoseEnabledForInvisibleSession(ovrpBool* trackingPoseEnabled);

/// If true, head tracking affects the pose reported by ovrp_GetEyePose even for invisible sessions.
/// If false, last visible frame's head pose is used.
OVRP_EXPORT ovrpResult ovrp_SetTrackingPoseEnabledForInvisibleSession(ovrpBool trackingPoseEnabled);

/// If true, the inter-pupillary distance affects the position reported by ovrp_GetEyePose.
OVRP_EXPORT ovrpResult ovrp_GetTrackingIPDEnabled2(ovrpBool* trackingIPDEnabled);

/// If true, the inter-pupillary distance affects the position reported by ovrp_GetEyePose.
OVRP_EXPORT ovrpResult ovrp_SetTrackingIPDEnabled2(ovrpBool trackingIPDEnabled);

/// Gets the calibrated origin pose.
OVRP_EXPORT ovrpResult ovrp_GetTrackingCalibratedOrigin2(ovrpPosef* trackingCalibratedOrigin);

/// Oculus Internal. Sets the system-wide calibrated origin for the currently active tracking origin
/// type.
OVRP_EXPORT ovrpResult ovrp_SetTrackingCalibratedOrigin2();

/// Gets the currently active tracking origin type.
OVRP_EXPORT ovrpResult ovrp_GetTrackingOriginType2(ovrpTrackingOrigin* trackingOriginType);

/// Sets the currently active tracking origin type.
OVRP_EXPORT ovrpResult ovrp_SetTrackingOriginType2(ovrpTrackingOrigin trackingOriginType);

/// Changes the frame of reference used by tracking.
/// See the ovrpRecenterFlag enum for details about available flags.
OVRP_EXPORT ovrpResult ovrp_RecenterTrackingOrigin2(unsigned int flags);

/// If true, the node is considered present and available.
OVRP_EXPORT ovrpResult ovrp_GetNodePresent2(ovrpNode nodeId, ovrpBool* nodePresent);

/// If true, the node's orientation is tracked.
OVRP_EXPORT ovrpResult ovrp_GetNodeOrientationTracked2(ovrpNode nodeId, ovrpBool* nodeOrientationTracked);

/// If true, the node's orientation is valid.
OVRP_EXPORT ovrpResult ovrp_GetNodeOrientationValid(ovrpNode nodeId, ovrpBool* nodeOrientationValid);

/// If true, the node's position is tracked.
OVRP_EXPORT ovrpResult ovrp_GetNodePositionTracked2(ovrpNode nodeId, ovrpBool* nodePositionTracked);

/// If true, the node's position is valid.
OVRP_EXPORT ovrpResult ovrp_GetNodePositionValid(ovrpNode nodeId, ovrpBool* nodePositionValid);

/// Force a node position to be tracked or not. Return false if the node's position tracking cannot
/// be changed.
OVRP_EXPORT ovrpResult ovrp_SetNodePositionTracked2(ovrpNode nodeId, ovrpBool nodePositionTracked);

/// Gets the current pose, acceleration, and velocity of the given node on the given update cadence.
OVRP_EXPORT ovrpResult
ovrp_GetNodePoseState3(ovrpStep step, int frameIndex, ovrpNode nodeId, ovrpPoseStatef* nodePoseState);

/// Gets the current pose, acceleration, and velocity of the given node on the given update cadence, without applying
/// any modifier (e.g. HeadPoseModifier)
OVRP_EXPORT ovrpResult
ovrp_GetNodePoseStateRaw(ovrpStep step, int frameIndex, ovrpNode nodeId, ovrpPoseStatef* nodePoseState);

/// Gets the current frustum for the given node, if available.
OVRP_EXPORT ovrpResult ovrp_GetNodeFrustum2(ovrpNode nodeId, ovrpFrustum2f* nodeFrustum);





















/// Set relative rotation/translation to the eye pose
OVRP_EXPORT ovrpResult
ovrp_SetHeadPoseModifier(const ovrpQuatf* relativeRotation, const ovrpVector3f* relativeTranslation);

/// Get current relative rotation/translation to the eye pose
OVRP_EXPORT ovrpResult ovrp_GetHeadPoseModifier(ovrpQuatf* relativeRotation, ovrpVector3f* relativeTranslation);

/// Gets the controller state for the given controllers.
OVRP_EXPORT ovrpResult ovrp_GetControllerState4(ovrpController controllerMask, ovrpControllerState4* controllerState);

/// Gets the controller state for the given controllers.
OVRP_EXPORT ovrpResult ovrp_GetControllerState5(ovrpController controllerMask, ovrpControllerState5* controllerState);

/// Gets the controller state for the given controllers.
OVRP_EXPORT ovrpResult ovrp_GetControllerState6(ovrpController controllerMask, ovrpControllerState6* controllerState);

/// Gets the currently active controller type.
OVRP_EXPORT ovrpResult ovrp_GetActiveController2(ovrpController* activeController);

/// Gets the currently connected controller types as a bitmask.
OVRP_EXPORT ovrpResult ovrp_GetConnectedControllers2(ovrpController* connectedControllers);

/// Sets the vibration state for the given controllers.
OVRP_EXPORT ovrpResult ovrp_SetControllerVibration2(ovrpController controllerMask, float frequency, float amplitude);

/// Sets the vibration state for the specified haptics locations on the given controllers.
OVRP_EXPORT ovrpResult ovrp_SetControllerLocalizedVibration(
    ovrpController controllerMask,
    ovrpHapticsLocation hapticsLocationMask,
    float frequency,
    float amplitude);

OVRP_EXPORT ovrpResult ovrp_SetControllerHapticsAmplitudeEnvelope(
    ovrpController controllerMask,
    ovrpHapticsAmplitudeEnvelopeVibration hapticsVibration);

OVRP_EXPORT ovrpResult
ovrp_SetControllerHapticsPcm(ovrpController controllerMask, ovrpHapticsPcmVibration hapticsVibration);















/// Gets the current haptics desc for the given controllers.
OVRP_EXPORT ovrpResult
ovrp_GetControllerHapticsDesc2(ovrpController controllerMask, ovrpHapticsDesc* controllerHapticsDesc);

/// Gets the current haptics state for the given controllers.
OVRP_EXPORT ovrpResult
ovrp_GetControllerHapticsState2(ovrpController controllerMask, ovrpHapticsState* controllerHapticsState);

/// Gets the preferred sample rate (in Hz) for the given controller.
OVRP_EXPORT ovrpResult ovrp_GetControllerSampleRateHz(ovrpController controller, float* sampleRateHz);

/// Sets the haptics buffer state for the given controllers.
OVRP_EXPORT ovrpResult ovrp_SetControllerHaptics2(ovrpController controllerMask, ovrpHapticsBuffer hapticsBuffer);

// Action State Functions
OVRP_EXPORT ovrpResult ovrp_GetActionStateBoolean(const char* actionName, ovrpBool* result);
OVRP_EXPORT ovrpResult ovrp_GetActionStateFloat(const char* actionName, float* result);
OVRP_EXPORT ovrpResult ovrp_GetActionStateVector2f(const char* actionName, ovrpVector2f* value);
OVRP_EXPORT ovrpResult ovrp_GetActionStatePose(const char* actionName, ovrpPosef* result);
OVRP_EXPORT ovrpResult ovrp_GetActionStatePose2(const char* actionName, ovrpHand hand, ovrpPosef* result);
OVRP_EXPORT ovrpResult ovrp_GetCurrentInteractionProfileName(ovrpHand hand, char* interactionProfile);
OVRP_EXPORT ovrpResult
ovrp_TriggerVibrationAction(const char* actionName, ovrpHand hand, float duration, float amplitude);

/// Sets the suggested CPU performance level
OVRP_EXPORT ovrpResult ovrp_SetSuggestedCpuPerformanceLevel(ovrpProcessorPerformanceLevel perfLevel);

/// Gets the suggested CPU performance level
OVRP_EXPORT ovrpResult ovrp_GetSuggestedCpuPerformanceLevel(ovrpProcessorPerformanceLevel* perfLevel);

/// Sets the suggested CPU performance level
OVRP_EXPORT ovrpResult ovrp_SetSuggestedGpuPerformanceLevel(ovrpProcessorPerformanceLevel perfLevel);

/// Gets the suggested CPU performance level
OVRP_EXPORT ovrpResult ovrp_GetSuggestedGpuPerformanceLevel(ovrpProcessorPerformanceLevel* perfLevel);

/// Gets the current CPU performance level, integer in the range 0 - 3 (deprecated).
OVRP_EXPORT ovrpResult ovrp_GetSystemCpuLevel2(int* systemCpuLevel);

/// Sets the current CPU performance level, integer in the range 0 - 3 (deprecated).
OVRP_EXPORT ovrpResult ovrp_SetSystemCpuLevel2(int systemCpuLevel);

/// Returns true if the application should run at the maximum possible CPU level.
OVRP_EXPORT ovrpResult ovrp_GetAppCPUPriority2(ovrpBool* appCPUPriority);

/// Determines whether the application should run at the maximum possible CPU level.
OVRP_EXPORT ovrpResult ovrp_SetAppCPUPriority2(ovrpBool appCPUPriority);

/// Gets the current GPU performance level, integer in the range 0 - 3 (deprecated).
OVRP_EXPORT ovrpResult ovrp_GetSystemGpuLevel2(int* systemGpuLevel);

/// Sets the current GPU performance level, integer in the range 0 - 3 (deprecated).
OVRP_EXPORT ovrpResult ovrp_SetSystemGpuLevel2(int systemGpuLevel);

/// If true, the system is running in a reduced performance mode to save power.
OVRP_EXPORT ovrpResult ovrp_GetSystemPowerSavingMode2(ovrpBool* systemPowerSavingMode);

/// Gets the current refresh rate of the HMD.
OVRP_EXPORT ovrpResult ovrp_GetSystemDisplayFrequency2(float* systemDisplayFrequency);

/// Gets the available refresh rates of the HMD.
OVRP_EXPORT ovrpResult
ovrp_GetSystemDisplayAvailableFrequencies(float* systemDisplayAvailableFrequencies, int* arraySize);

/// Sets the refresh rate for the HMD
OVRP_EXPORT ovrpResult ovrp_SetSystemDisplayFrequency(float requestedFrequency);

/// Gets the minimum number of vsyncs to wait after each frame.
OVRP_EXPORT ovrpResult ovrp_GetSystemVSyncCount2(int* systemVSyncCount);

/// Sets the minimum number of vsyncs to wait after each frame.
OVRP_EXPORT ovrpResult ovrp_SetSystemVSyncCount2(int systemVSyncCount);

/// OVRPlugin does not support Android system callbacks in UE4 and UE5.
/// Please use Optional Mobile Features Blueprint Library Plugin or include FAndroidMisc.h in your project
#ifndef OVRPLUGIN_JNI_LIB_EXCLUDED
/// Gets the current system volume level.
OVRP_EXPORT ovrpResult ovrp_GetSystemVolume2(float* systemVolume);

/// If true, headphones are currently attached to the device.
OVRP_EXPORT ovrpResult ovrp_GetSystemHeadphonesPresent2(ovrpBool* systemHeadphonesPresent);

/// Gets the status of the system's battery or "Unknown" if there is none.
OVRP_EXPORT ovrpResult ovrp_GetSystemBatteryStatus2(ovrpBatteryStatus* systemBatteryStatus);

/// Gets the current available battery charge, ranging from 0 (empty) to 1 (full).
OVRP_EXPORT ovrpResult ovrp_GetSystemBatteryLevel2(float* systemBatteryLevel);

/// Gets the current battery temperature in degrees Celsius.
OVRP_EXPORT ovrpResult ovrp_GetSystemBatteryTemperature2(float* systemBatteryTemperature);
#endif

/// Gets the current product name for the device, if available.
OVRP_EXPORT ovrpResult ovrp_GetSystemProductName2(char const** systemProductName);

/// Gets the current region for the device, if available.
OVRP_EXPORT ovrpResult ovrp_GetSystemRegion2(ovrpSystemRegion* systemRegion);

/// Shows a given platform user interface.
OVRP_EXPORT ovrpResult ovrp_ShowSystemUI2(ovrpUI ui);

/// If true, the app has VR focus.
OVRP_EXPORT ovrpResult ovrp_GetAppHasVrFocus2(ovrpBool* appHasVrFocus);

/// True if the application is the foreground application and receives input (e.g. Touch
/// controller state). If this is false then the application is in the background (but possibly
/// still visible) should hide any input representations such as hands.
OVRP_EXPORT ovrpResult ovrp_GetAppHasInputFocus(ovrpBool* appHasInputFocus);

/// True if a system overlay is present, such as a dashboard. In this case the application
/// (if visible) should pause while still drawing, avoid drawing near-field graphics so they
/// don't visually fight with the system overlay, and consume fewer CPU and GPU resources.
OVRP_EXPORT ovrpResult ovrp_GetAppHasSystemOverlayPresent(ovrpBool* appHasOverlayPresent);

/// If true, the app should quit as soon as possible.
OVRP_EXPORT ovrpResult ovrp_GetAppShouldQuit2(ovrpBool* appShouldQuit);

/// If true, the app should recenter as soon as possible.
OVRP_EXPORT ovrpResult ovrp_GetAppShouldRecenter2(ovrpBool* appShouldRecenter);

/// If true, the app should recreate the distortion window as soon as possible.
OVRP_EXPORT ovrpResult ovrp_GetAppShouldRecreateDistortionWindow2(ovrpBool* appShouldRecreateDistortionWindow);

/// Returns the the app space.
OVRP_EXPORT ovrpResult ovrp_GetAppSpace(ovrpUInt64* appSpace);

/// Gets the latest measured latency timings.
OVRP_EXPORT ovrpResult ovrp_GetAppLatencyTimings2(ovrpAppLatencyTimings* appLatencyTimings);

/// Sets the engine info for the current app.
OVRP_EXPORT ovrpResult ovrp_SetAppEngineInfo2(const char* engineName, const char* engineVersion, ovrpBool isEditor);

/// If true, the user is currently wearing the VR display and it is not idle.
OVRP_EXPORT ovrpResult ovrp_GetUserPresent2(ovrpBool* userPresent);

/// Gets the physical inter-pupillary distance (IPD) separating the user's eyes in meters.
OVRP_EXPORT ovrpResult ovrp_GetUserIPD2(float* userIPD);

/// Sets the physical inter-pupillary distance (IPD) separating the user's eyes in meters.
OVRP_EXPORT ovrpResult ovrp_SetUserIPD2(float value);

/// Gets the physical height of the player's eyes from the ground in meters.
OVRP_EXPORT ovrpResult ovrp_GetUserEyeHeight2(float* userEyeHeight);

/// Sets the physical height of the player's eyes from the ground in meters.
OVRP_EXPORT ovrpResult ovrp_SetUserEyeHeight2(float userEyeHeight);

/// Gets the physical distance from the base of the neck to the center of the player's eyes in
/// meters.
OVRP_EXPORT ovrpResult ovrp_GetUserNeckEyeDistance2(ovrpVector2f* userEyeNeckDistance);

/// Sets the physical distance from the base of the neck to the center of the player's eyes in
/// meters.
OVRP_EXPORT ovrpResult ovrp_SetUserNeckEyeDistance2(ovrpVector2f userEyeNeckDistance);

/// Setup the current display objects
OVRP_EXPORT ovrpResult ovrp_SetupDisplayObjects2(void* device, void* display, void* window);

/// Return true if the device supports multi-view rendering
OVRP_EXPORT ovrpResult ovrp_GetSystemMultiViewSupported2(ovrpBool* systemMultiViewSupported);

/// Return true is the plugin supports submitting texture arrays
OVRP_EXPORT ovrpResult ovrp_GetEyeTextureArraySupported2(ovrpBool* eyeTextureArraySupported);

/// If true, the boundary system is configured with valid boundary data.
OVRP_EXPORT ovrpResult ovrp_GetBoundaryConfigured2(ovrpBool* boundaryConfigured);

/// Return success if the device supports depth compositing
OVRP_EXPORT ovrpResult ovrp_GetDepthCompositingSupported(ovrpBool* depthCompositingSupported);

/// Performs a boundary test between the specified node and boundary types.
OVRP_EXPORT ovrpResult
ovrp_TestBoundaryNode2(ovrpNode node, ovrpBoundaryType boundaryType, ovrpBoundaryTestResult* boundaryTestResult);

/// Performs a boundary test between the specified point and boundary types.
OVRP_EXPORT ovrpResult
ovrp_TestBoundaryPoint2(ovrpVector3f point, ovrpBoundaryType boundaryType, ovrpBoundaryTestResult* boundaryTestResult);

/// Gets the geometry data for the specified boundary type.
OVRP_EXPORT ovrpResult ovrp_GetBoundaryGeometry3(ovrpBoundaryType boundaryType, ovrpVector3f* points, int* pointsCount);

/// Gets the dimensions for the specified boundary type. Returned x,y,z values correspond to width,
/// height, depth.
OVRP_EXPORT ovrpResult ovrp_GetBoundaryDimensions2(ovrpBoundaryType boundaryType, ovrpVector3f* bounaryDimensions);

/// Gets the current visiblity status for the boundary system.
OVRP_EXPORT ovrpResult ovrp_GetBoundaryVisible2(ovrpBool* boundaryVisible);

/// Requests that the boundary system visibility be set to the specified value. Can be overridden by
/// the boundary system or the user.
OVRP_EXPORT ovrpResult ovrp_SetBoundaryVisible2(ovrpBool boundaryVisible);

/// Returns the currently present headset type.
OVRP_EXPORT ovrpResult ovrp_GetSystemHeadsetType2(ovrpSystemHeadset* systemHeadsetType);

/// Returns information useful for performance analysis and dynamic quality adjustments.
OVRP_EXPORT ovrpResult ovrp_GetAppPerfStats2(ovrpAppPerfStats* appPerfStats);

/// Resets internal performance counters to clear previous data from impacting the current reported
/// state.
OVRP_EXPORT ovrpResult ovrp_ResetAppPerfStats2();

/// Return the app FPS, thread safe
OVRP_EXPORT ovrpResult ovrp_GetAppFramerate2(float* appFramerate);

/// Returns if a certain perf metrics is supported
OVRP_EXPORT ovrpResult ovrp_IsPerfMetricsSupported(ovrpPerfMetrics perfMetrics, ovrpBool* supported);

/// Returns if a floating point perf metrics
OVRP_EXPORT ovrpResult ovrp_GetPerfMetricsFloat(ovrpPerfMetrics perfMetrics, float* value);

/// Returns if an integer perf metrics
OVRP_EXPORT ovrpResult ovrp_GetPerfMetricsInt(ovrpPerfMetrics perfMetrics, int* value);

/// Set a latency when getting the hand node poses through ovrp_GetNodePoseState2(ovrpStep_Render, ...)
OVRP_EXPORT ovrpResult ovrp_SetHandNodePoseStateLatency(double latencyInSeconds);

/// Get the current latency when getting the hand node poses through ovrp_GetNodePoseState2(ovrpStep_Render, ...)
OVRP_EXPORT ovrpResult ovrp_GetHandNodePoseStateLatency(double* latencyInSeconds);

/// Returns the recommended multisample antialiasing level for the current device.
OVRP_EXPORT ovrpResult ovrp_GetSystemRecommendedMSAALevel2(int* systemRecommendedMSAALevel);

/// Inhibits system UX behavior.
OVRP_EXPORT ovrpResult ovrp_SetInhibitSystemUX2(ovrpBool inhibitSystemUX);

/// Return true if the device supports tiled multires
OVRP_EXPORT ovrpResult ovrp_GetTiledMultiResSupported(ovrpBool* foveationSupported);

/// Returns the current multires level on the device
OVRP_EXPORT ovrpResult ovrp_GetTiledMultiResLevel(ovrpTiledMultiResLevel* level);

/// Sets MultiRes levels
OVRP_EXPORT ovrpResult ovrp_SetTiledMultiResLevel(ovrpTiledMultiResLevel level);

/// Return if MultiRes is dynamic or not
OVRP_EXPORT ovrpResult ovrp_GetTiledMultiResDynamic(ovrpBool* isDynamic);

/// Sets if MultiRes is dynamic or not
OVRP_EXPORT ovrpResult ovrp_SetTiledMultiResDynamic(ovrpBool isDynamic);

/// Returns the current motion vector multires level on the device
OVRP_EXPORT ovrpResult ovrp_GetMotionVectorTiledMultiResLevel(ovrpTiledMultiResLevel* level);

/// Sets MultiRes motion vector levels
OVRP_EXPORT ovrpResult ovrp_SetMotionVectorTiledMultiResLevel(ovrpTiledMultiResLevel level);

/// Return true if the device supports eye tracked foveation
OVRP_EXPORT ovrpResult ovrp_GetFoveationEyeTrackedSupported(ovrpBool* foveationSupported);

/// Return if foveation is eye tracked
OVRP_EXPORT ovrpResult ovrp_GetFoveationEyeTracked(ovrpBool* isEyeTracked);

/// Sets if foveation is eye tracked
OVRP_EXPORT ovrpResult ovrp_SetFoveationEyeTracked(ovrpBool isEyeTracked);

/// Gets the eye tracked foveation center (used for tile offset)
OVRP_EXPORT ovrpResult ovrp_GetFoveationEyeTrackedCenter(ovrpVector2f fovCenter[2]);















































/// Return true if the device supports GPU Util querying
OVRP_EXPORT ovrpResult ovrp_GetGPUUtilSupported(ovrpBool* gpuUtilSupported);

/// Return the GPU util if the device supports it
OVRP_EXPORT ovrpResult ovrp_GetGPUUtilLevel(float* gpuUtil);

/// Set thread's performance level, for example, put the performance critical thread on golden cores,
/// future policy might change for future hardware
OVRP_EXPORT ovrpResult ovrp_SetThreadPerformance(int threadId, ovrpThreadPerf perf);

/// This is specifically for Unity to fix Core Affinity wrong assignment.
OVRP_EXPORT ovrpResult ovrp_AutoThreadScheduling(
    unsigned int bigCoreMaskFromEngine,
    unsigned int* threadIds,
    ovrpThreadPerf* threadPerfFlags,
    int threadCount);

OVRP_EXPORT ovrpResult ovrp_GetGPUFrameTime(float* gpuTime);

/// This is to request vertices and indices for the triangle mesh
OVRP_EXPORT ovrpResult ovrp_GetViewportStencil(
    ovrpEye eyeId,
    ovrpViewportStencilType type,
    ovrpVector2f* vertices,
    int* vertexCount,
    ovrpUInt16* indices,
    int* indexCount);

OVRP_EXPORT ovrpResult ovrp_SetDeveloperTelemetryConsent(ovrpBool consent);

OVRP_EXPORT ovrpResult ovrp_SendEvent(const char* eventName, const char* param);

OVRP_EXPORT ovrpResult ovrp_SendEvent2(const char* eventName, const char* param, const char* source);

OVRP_EXPORT ovrpResult ovrp_SendUnifiedEvent(
    ovrpBool isEssential,
    const char* productType,
    const char* eventName,
    const char* event_metadata_json,
    const char* project_name,
    const char* event_entrypoint,
    const char* project_guid,
    const char* event_type,
    const char* event_target,
    const char* error_msg,
    const char* is_internal);

OVRP_EXPORT ovrpResult ovrp_AddCustomMetadata(const char* metadataName, const char* metadataParam);

OVRP_EXPORT ovrpResult ovrp_SetDeveloperMode(ovrpBool active);

OVRP_EXPORT ovrpResult ovrp_SetDeveloperModeStrict(ovrpBool active);

OVRP_EXPORT ovrpResult ovrp_SetVrApiPropertyInt(int propertyEnum, int value);

OVRP_EXPORT ovrpResult ovrp_SetVrApiPropertyFloat(int propertyEnum, float value);

OVRP_EXPORT ovrpResult ovrp_GetVrApiPropertyInt(int propertyEnum, int* value);







OVRP_EXPORT ovrpResult ovrp_GetCurrentTrackingTransformPose(ovrpPosef* trackingTransformPose);

OVRP_EXPORT ovrpResult ovrp_GetTrackingTransformRawPose(ovrpPosef* trackingTransformRawPose);

OVRP_EXPORT ovrpResult
ovrp_GetTrackingTransformRelativePose(ovrpPosef* trackingTransformRelativePose, ovrpTrackingOrigin trackingOrigin);

OVRP_EXPORT ovrpResult ovrp_GetTimeInSeconds(double* timeInSeconds);

OVRP_EXPORT ovrpResult ovrp_GetASWVelocityScale(float* aswVelocityScale);
OVRP_EXPORT ovrpResult ovrp_GetASWDepthScale(float* aswDepthScale);
OVRP_EXPORT ovrpResult ovrp_GetASWAdaptiveMode(ovrpBool* aswAdaptiveMode);
OVRP_EXPORT ovrpResult ovrp_SetASWAdaptiveMode(ovrpBool aswAdaptiveMode);
OVRP_EXPORT ovrpResult ovrp_IsRequestingASWData(ovrpBool* needASWData);

OVRP_EXPORT ovrpResult ovrp_GetPredictedDisplayTime(int frameIndex, double* predictedDisplayTime);

/// Set whether the system should be querying for hand poses powered by controller data
OVRP_EXPORT ovrpResult ovrp_SetControllerDrivenHandPoses(ovrpBool controllerDrivenHandPoses);
OVRP_EXPORT ovrpResult ovrp_SetControllerDrivenHandPosesAreNatural(ovrpBool controllerDrivenHandPosesAreNatural);
OVRP_EXPORT ovrpResult ovrp_IsControllerDrivenHandPosesEnabled(ovrpBool* enabled);
OVRP_EXPORT ovrpResult ovrp_AreControllerDrivenHandPosesNatural(ovrpBool* natural);

OVRP_EXPORT ovrpResult ovrp_SetWideMotionModeHandPoses(ovrpBool wideMotionModeHandPoses);
OVRP_EXPORT ovrpResult ovrp_IsWideMotionModeHandPosesEnabled(ovrpBool* enabled);

OVRP_EXPORT ovrpResult ovrp_SetHandSkeletonVersion(ovrHandSkeletonVersion version);






OVRP_EXPORT ovrpResult ovrp_SetMultimodalHandsControllersSupported(ovrpBool supported);
OVRP_EXPORT ovrpResult ovrp_IsMultimodalHandsControllersSupported(ovrpBool* supported);
OVRP_EXPORT ovrpResult
ovrp_GetCurrentDetachedInteractionProfile(ovrpHand hand, ovrpInteractionProfile* interactionProfile);

OVRP_EXPORT ovrpResult ovrp_GetHandTrackingEnabled(ovrpBool* handTrackingEnabled);
OVRP_EXPORT ovrpResult ovrp_GetHandState(ovrpStep step, ovrpHand hand, ovrpHandState* handState);
OVRP_EXPORT ovrpResult ovrp_GetHandState2(ovrpStep step, int frameIndex, ovrpHand hand, ovrpHandState* handState);
OVRP_EXPORT ovrpResult ovrp_GetHandState3(ovrpStep step, int frameIndex, ovrpHand hand, ovrpHandState3* handState);
OVRP_EXPORT ovrpResult
ovrp_GetHandTrackingState(ovrpStep step, int frameIndex, ovrpHand hand, ovrpHandTrackingState* handTrackingState);
OVRP_EXPORT ovrpResult ovrp_GetSkeleton2(ovrpSkeletonType skeletonType, ovrpSkeleton2* skeleton);
OVRP_EXPORT ovrpResult ovrp_GetSkeleton3(ovrpSkeletonType skeletonType, ovrpSkeleton3* skeleton);
OVRP_EXPORT ovrpResult ovrp_GetMesh(ovrpMeshType meshType, ovrpMesh* mesh);

OVRP_EXPORT ovrpResult ovrp_GetBodyState(ovrpStep step, int frameIndex, ovrpBodyState* bodyState);




OVRP_EXPORT ovrpResult ovrp_GetBodyState4(ovrpStep step, int frameIndex, ovrpBodyState4* bodyState);
OVRP_EXPORT ovrpResult ovrp_GetBodyTrackingEnabled(ovrpBool* enabled);
OVRP_EXPORT ovrpResult ovrp_GetBodyTrackingSupported(ovrpBool* supported);
OVRP_EXPORT ovrpResult ovrp_GetFullBodyTrackingEnabled(ovrpBool* enabled);

OVRP_EXPORT ovrpResult ovrp_StartFaceTracking();
OVRP_EXPORT ovrpResult ovrp_StopFaceTracking();
OVRP_EXPORT ovrpResult ovrp_GetFaceTracking2Enabled(ovrpBool* faceTracking2Enabled);
OVRP_EXPORT ovrpResult ovrp_GetFaceTracking2Supported(ovrpBool* faceTracking2Supported);
OVRP_EXPORT ovrpResult ovrp_GetFaceTrackingVisemesEnabled(ovrpBool* faceTrackingVisemesEnabled);
OVRP_EXPORT ovrpResult ovrp_GetFaceTrackingVisemesSupported(ovrpBool* faceTrackingVisemesSupported);
OVRP_EXPORT ovrpResult ovrp_StartFaceTracking2(
    const ovrpFaceTrackingDataSource2* const requestedDataSources,
    unsigned int requestedDataSourcesCount);
OVRP_EXPORT ovrpResult ovrp_StopFaceTracking2();
OVRP_EXPORT ovrpResult ovrp_StartBodyTracking();




OVRP_EXPORT ovrpResult ovrp_StartBodyTracking2(ovrpBodyJointSet jointSet);

OVRP_EXPORT ovrpResult ovrp_RequestBodyTrackingFidelity(ovrpBodyTrackingFidelity2 bodyTrackingFidelity);





OVRP_EXPORT ovrpResult ovrp_SuggestBodyTrackingCalibrationOverride(ovrpBodyTrackingCalibrationInfo calibrationInfo);
OVRP_EXPORT ovrpResult ovrp_ResetBodyTrackingCalibration();

OVRP_EXPORT ovrpResult ovrp_StopBodyTracking();
OVRP_EXPORT ovrpResult ovrp_StartEyeTracking();
OVRP_EXPORT ovrpResult ovrp_StopEyeTracking();

OVRP_EXPORT ovrpResult ovrp_GetLocalTrackingSpaceRecenterCount(int* recenterCount);

// Returns true if the system Hmd is in 3dof mode
OVRP_EXPORT ovrpResult ovrp_GetSystemHmd3DofModeEnabled(ovrpBool* enabled);

OVRP_EXPORT ovrpResult ovrp_SetClientColorDesc(ovrpColorSpace colorSpace);
OVRP_EXPORT ovrpResult ovrp_GetHmdColorDesc(ovrpColorSpace* colorSpace);

OVRP_EXPORT ovrpResult ovrp_SetEyeBufferSharpenType(ovrpLayerSubmitFlags sharpenType);
OVRP_EXPORT ovrpResult ovrp_SetEyeBufferSecure(ovrpBool secure);

// app should call this in a loop until there are no more events, which will return ovrpSuccess_EventUnavailable and an
// event of type ovrpEventType_None ovrp_PollEvent and ovrp_PollEvent2 are both in use, Unity needed ovrp_PollEvent2 due
// to memory allocation issues
OVRP_EXPORT ovrpResult ovrp_PollEvent(ovrpEventDataBuffer* eventBuffer);
OVRP_EXPORT ovrpResult ovrp_PollEvent2(ovrpEventType* eventType, unsigned char** eventBuffer);

OVRP_EXPORT ovrpResult ovrp_SetKeyboardOverlayUV(ovrpVector2f uv);
OVRP_EXPORT ovrpResult ovrp_SetKeyboardOverlayPose(ovrpPosef pose);







OVRP_EXPORT ovrpResult ovrp_CreateVirtualKeyboard(const ovrpVirtualKeyboardCreateInfo* createInfo);
OVRP_EXPORT ovrpResult ovrp_DestroyVirtualKeyboard();
OVRP_EXPORT ovrpResult
ovrp_SendVirtualKeyboardInput(const ovrpVirtualKeyboardInputInfo* info, ovrpPosef* interactorRootPose);
OVRP_EXPORT ovrpResult ovrp_ChangeVirtualKeyboardTextContext(const char* textContext);
OVRP_EXPORT ovrpResult
ovrp_CreateVirtualKeyboardSpace(const ovrpVirtualKeyboardSpaceCreateInfo* createInfo, ovrpUInt64* keyboardSpace);
OVRP_EXPORT ovrpResult ovrp_SuggestVirtualKeyboardLocation(const ovrpVirtualKeyboardLocationInfo* locationInfo);
OVRP_EXPORT ovrpResult ovrp_GetVirtualKeyboardScale(float* scale);
OVRP_EXPORT ovrpResult
ovrp_GetVirtualKeyboardModelAnimationStates(ovrpVirtualKeyboardModelAnimationStates* animationStates);
OVRP_EXPORT ovrpResult ovrp_GetVirtualKeyboardDirtyTextures(ovrpVirtualKeyboardTextureIds* textureIds);
OVRP_EXPORT ovrpResult
ovrp_GetVirtualKeyboardTextureData(ovrpUInt64 textureId, ovrpVirtualKeyboardTextureData* textureData);
OVRP_EXPORT ovrpResult ovrp_SetVirtualKeyboardModelVisibility(const ovrpVirtualKeyboardModelVisibility* visibility);

OVRP_EXPORT ovrpResult ovrp_SaveUnifiedConsent(int toolId, ovrpBool consentValue);
OVRP_EXPORT ovrpResult ovrp_SaveUnifiedConsentWithOlderVersion(int toolId, ovrpBool consentValue, int consentVersion);
OVRP_EXPORT ovrpOptionalBool ovrp_GetUnifiedConsent(int toolId);
OVRP_EXPORT ovrpResult ovrp_GetConsentTitle(char* consentTitle);
OVRP_EXPORT ovrpResult ovrp_GetConsentMarkdownText(char* consentMarkdownText);
OVRP_EXPORT ovrpResult ovrp_GetConsentNotificationMarkdownText(
    const char* consentChangeLocationMarkdown,
    char* consentNotificationMarkdownText);
OVRP_EXPORT ovrpResult ovrp_GetConsentSettingsChangeText(char* consentSettingsChangeText);

OVRP_EXPORT ovrpBool ovrp_ShouldShowTelemetryConsentWindow(int toolId);
OVRP_EXPORT ovrpBool ovrp_IsConsentSettingsChangeEnabled(int toolId);
OVRP_EXPORT ovrpBool ovrp_ShouldShowTelemetryNotification(int toolId);
OVRP_EXPORT ovrpResult ovrp_SetNotificationShown(int tool);

OVRP_EXPORT ovrpResult ovrp_QplSetConsent(ovrpBool qplConsent);
OVRP_EXPORT ovrpResult ovrp_QplMarkerStart(int markerId, int instanceKey, ovrpInt64 timestampMs);
OVRP_EXPORT ovrpResult ovrp_QplMarkerStartForJoin(
    int markerId,
    const char* joinId,
    ovrpBool cancelMarkerIfAppBackgrounded,
    int instanceKey,
    long long timestampMs);
OVRP_EXPORT ovrpResult ovrp_QplMarkerEnd(int markerId, ovrpInt16 actionId, int instanceKey, ovrpInt64 timestampMs);
OVRP_EXPORT ovrpResult ovrp_QplMarkerPoint(int markerId, const char* name, int instanceKey, ovrpInt64 timestampMs);
OVRP_EXPORT ovrpResult ovrp_QplMarkerPointCached(int markerId, int nameHandle, int instanceKey, ovrpInt64 timestampMs);
OVRP_EXPORT ovrpResult
ovrp_QplMarkerAnnotation(int markerId, const char* annotationKey, const char* annotationValue, int instanceKey);
OVRP_EXPORT ovrpResult ovrp_QplMarkerAnnotationVariant(
    int markerId,
    const char* annotationKey,
    const ovrpQplVariant* annotationValue,
    int instanceKey);
OVRP_EXPORT ovrpResult ovrp_QplMarkerPointData(
    int markerId,
    const char* name,
    const ovrpQplAnnotation* annotations,
    int annotationCount,
    int instanceKey,
    ovrpInt64 timestampMs);

OVRP_EXPORT ovrpResult ovrp_QplCreateMarkerHandle(const char* name, int* nameHandle);
OVRP_EXPORT ovrpResult ovrp_QplDestroyMarkerHandle(int nameHandle);
OVRP_EXPORT ovrpResult ovrp_OnEditorShutdown();

/// Gets the current recent pose, acceleration, and velocity of the given node for the current time without any
/// prediction
OVRP_EXPORT ovrpResult ovrp_GetNodePoseStateImmediate(ovrpNode nodeId, ovrpPoseStatef* nodePoseState);

OVRP_EXPORT ovrpResult ovrp_GetNodePoseStateAtTime(double time, ovrpNode nodeId, ovrpPoseStatef* pose);

OVRP_EXPORT ovrpResult
ovrp_AreHandPosesGeneratedByControllerData(ovrpStep step, ovrpNode nodeId, ovrpBool* generatedByControllerData);


OVRP_EXPORT ovrpResult ovrp_SetSimultaneousHandsAndControllersEnabled(ovrpBool enabled);
OVRP_EXPORT ovrpResult ovrp_GetControllerIsInHand(ovrpStep step, ovrpNode nodeId, ovrpBool* isInHand);

OVRP_EXPORT ovrpResult ovrp_GetRenderModelPaths(unsigned int index, char* path);
OVRP_EXPORT ovrpResult ovrp_GetRenderModelProperties(const char* path, ovrpRenderModelProperties* properties);
OVRP_EXPORT ovrpResult ovrp_GetRenderModelProperties2(
    const char* path,
    ovrpRenderModelFlags renderModelFlags,
    ovrpRenderModelProperties* properties);
OVRP_EXPORT ovrpResult ovrp_LoadRenderModel(
    ovrpUInt64 modelKey,
    ovrpUInt32 bufferInputCapacity,
    ovrpUInt32* bufferCountOutput,
    unsigned char* buffer);

OVRP_EXPORT ovrpResult ovrp_LocateSpace(ovrpPosef* location, const ovrpSpace* space, ovrpTrackingOrigin baseSpaceType);
OVRP_EXPORT ovrpResult
ovrp_LocateSpace2(ovrpSpaceLocationf* location, const ovrpSpace* space, ovrpTrackingOrigin baseSpaceType);
OVRP_EXPORT ovrpResult ovrp_CreateSpatialAnchor(const ovrpSpatialAnchorCreateInfo* createInfo, ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult ovrp_DestroySpace(ovrpSpace* space);
OVRP_EXPORT ovrpResult ovrp_SetSpaceComponentStatus(
    const ovrpSpace* space,
    ovrpSpaceComponentType componentType,
    const ovrpBool enable,
    const double timeout,
    ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult ovrp_GetSpaceComponentStatus(
    const ovrpSpace* space,
    ovrpSpaceComponentType componentType,
    ovrpBool* enabled,
    ovrpBool* changePending);
OVRP_EXPORT ovrpResult ovrp_EnumerateSpaceSupportedComponents(
    const ovrpSpace* space,
    ovrpUInt32 componentTypesCapacityInput,
    ovrpUInt32* componentTypesCountOutput,
    ovrpSpaceComponentType* componentTypes);
OVRP_EXPORT ovrpResult ovrp_QuerySpaces(const ovrpSpaceQueryInfo* queryInfo, ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult ovrp_QuerySpaces2(const ovrpSpaceQueryInfo2* queryInfo, ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult ovrp_RetrieveSpaceQueryResults(
    ovrpUInt64* requestId,
    ovrpUInt32 resultCapacityInput,
    ovrpUInt32* resultCountOutput,
    ovrpSpaceQueryResult* results);
OVRP_EXPORT ovrpResult ovrp_SaveSpace(
    const ovrpSpace* space,
    ovrpSpaceStorageLocation location,
    ovrpSpaceStoragePersistenceMode mode,
    ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult
ovrp_EraseSpace(const ovrpSpace* space, ovrpSpaceStorageLocation location, ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult ovrp_GetSpaceUuid(const ovrpSpace* space, ovrpUuid* uuid);
OVRP_EXPORT ovrpResult ovrp_GetSpaceUserId(const ovrpUser* spaceUser, ovrpUInt64* spaceUserId);
OVRP_EXPORT ovrpResult ovrp_CreateSpaceUser(const ovrpUInt64* spaceUserId, ovrpUser* spaceUser);
OVRP_EXPORT ovrpResult ovrp_DestroySpaceUser(const ovrpUser* spaceUser);
OVRP_EXPORT ovrpResult ovrp_SaveSpaceList(
    const ovrpSpace* spaces,
    ovrpUInt32 numSpaces,
    ovrpSpaceStorageLocation location,
    ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult ovrp_ShareSpaces(
    const ovrpSpace* spaces,
    ovrpUInt32 numSpaces,
    const ovrpUser* users,
    ovrpUInt32 numUsers,
    ovrpUInt64* requestId);

// XR_META_spatial_entity_sharing
OVRP_EXPORT ovrpResult ovrp_ShareSpaces2(const ovrpShareSpacesInfo* info, ovrpUInt64* requestId);

OVRP_EXPORT ovrpResult ovrp_GetSpaceContainer(const ovrpSpace* space, ovrpSpaceContainer* container);

OVRP_EXPORT ovrpResult ovrp_GetSpaceBoundingBox2D(const ovrpSpace* space, ovrpRectf* rect);

OVRP_EXPORT ovrpResult ovrp_GetSpaceBoundingBox3D(const ovrpSpace* space, ovrpBoundsf* bounds);

OVRP_EXPORT ovrpResult ovrp_GetSpaceSemanticLabels(const ovrpSpace* space, ovrpSemanticLabels* labels);

OVRP_EXPORT ovrpResult ovrp_GetSpaceRoomLayout(const ovrpSpace* space, ovrpRoomLayout* layout);

OVRP_EXPORT ovrpResult ovrp_GetSpaceBoundary2D(const ovrpSpace* space, ovrpBoundary2D* boundary);

OVRP_EXPORT ovrpResult ovrp_RequestSceneCapture(const ovrpSceneCaptureRequest* request, ovrpUInt64* requestId);


















OVRP_EXPORT ovrpResult ovrp_GetSpaceTriangleMesh(const ovrpSpace* space, ovrpTriangleMesh* triangleMeshOutput);

















OVRP_EXPORT ovrpResult ovrp_GetFaceTrackingEnabled(ovrpBool* faceTrackingEnabled);

OVRP_EXPORT ovrpResult ovrp_GetFaceTrackingSupported(ovrpBool* faceTrackingSupported);

OVRP_EXPORT ovrpResult ovrp_GetFaceState(ovrpStep step, int frameIndex, ovrpFaceState* faceState);

OVRP_EXPORT ovrpResult ovrp_GetFaceState2(ovrpStep step, int frameIndex, ovrpFaceState2* faceState);

OVRP_EXPORT ovrpResult ovrp_GetFaceVisemesState(ovrpStep step, int frameIndex, ovrpFaceVisemesState* faceVisemesState);
OVRP_EXPORT ovrpResult ovrp_SetFaceTrackingVisemesEnabled(ovrpBool enabled);

OVRP_EXPORT ovrpResult ovrp_GetEyeTrackingEnabled(ovrpBool* eyeTrackingEnabled);

OVRP_EXPORT ovrpResult ovrp_GetEyeTrackingSupported(ovrpBool* eyeTrackingSupported);

OVRP_EXPORT ovrpResult ovrp_GetEyeGazesState(ovrpStep step, int frameIndex, ovrpEyeGazesState* eyeGazesState);








// XR_META_colocation_discovery
OVRP_EXPORT ovrpResult ovrp_StartColocationDiscovery(ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult ovrp_StopColocationDiscovery(ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult
ovrp_StartColocationAdvertisement(const ovrpColocationAdvertisementStartInfo* info, ovrpUInt64* requestId);
OVRP_EXPORT ovrpResult ovrp_StopColocationAdvertisement(ovrpUInt64* requestId);

OVRP_EXPORT ovrpResult
ovrp_FeatureFidelitySetFeatureEnable(ovrpFeatureType feature, ovrpFeatureEnableState featureEnableState);
OVRP_EXPORT ovrpResult
ovrp_FeatureFidelitySetFeatureFidelity(ovrpFeatureType feature, ovrpFeatureFidelity featureFidelity);
OVRP_EXPORT ovrpResult ovrp_FeatureFidelityGetFeatureState(
    ovrpFeatureType feature,
    ovrpFeatureState* outIdealState,
    ovrpFeatureState* outCurrentState);
















OVRP_EXPORT ovrpResult ovrp_GetLocalDimmingSupported(ovrpBool* localDimmingSupported);
OVRP_EXPORT ovrpResult ovrp_SetLocalDimming(ovrpBool localDimmingMode);
OVRP_EXPORT ovrpResult ovrp_GetLocalDimming(ovrpBool* localDimmingMode);

OVRP_EXPORT ovrpResult ovrp_GetCurrentInteractionProfile(ovrpHand hand, ovrpInteractionProfile* interactionProfile);













OVRP_EXPORT ovrpResult ovrp_GetLayerRecommendedResolution(int layerId, ovrpSizei* recommendedDimensions);
OVRP_EXPORT ovrpResult ovrp_GetEyeLayerRecommendedResolution(ovrpSizei* recommendedDimensions);





OVRP_EXPORT ovrpResult ovrp_RegisterOpenXREventHandler(ovrpOpenXrEventHandler eventHandler, void* context);
OVRP_EXPORT ovrpResult ovrp_UnregisterOpenXREventHandler(ovrpOpenXrEventHandler eventHandler);

OVRP_EXPORT ovrpResult ovrp_GetOpenXRInstanceProcAddrFunc(void** func);

OVRP_EXPORT ovrpResult ovrp_DiscoverSpaces(const ovrpSpaceDiscoveryInfo* info, ovrpUInt64* requestId);

OVRP_EXPORT ovrpResult ovrp_RetrieveSpaceDiscoveryResults(ovrpUInt64 requestId, ovrpSpaceDiscoveryResults* results);

OVRP_EXPORT ovrpResult ovrp_SaveSpaces(ovrpUInt32 spaceCount, const ovrpSpace* spaces, ovrpUInt64* requestId);

OVRP_EXPORT ovrpResult ovrp_EraseSpaces(
    ovrpUInt32 spaceCount,
    const ovrpSpace* spaces,
    ovrpUInt32 uuidCount,
    const ovrpUuid* uuids,
    ovrpUInt64* requestId);






OVRP_EXPORT ovrpResult ovrp_RequestBoundaryVisibility(ovrpBoundaryVisibility boundaryVisibility);
OVRP_EXPORT ovrpResult ovrp_GetBoundaryVisibility(ovrpBoundaryVisibility* boundaryVisibility);








OVRP_EXPORT ovrpResult ovrp_IsLayerShapeSupported(ovrpShape shape, ovrpBool* isLayerShapeSupported);

OVRP_EXPORT ovrpResult ovrp_GetEnvironmentDepthSupported(ovrpBool* supported);
OVRP_EXPORT ovrpResult ovrp_GetEnvironmentDepthHandRemovalSupported(ovrpBool* supported);
OVRP_EXPORT ovrpResult ovrp_InitializeEnvironmentDepth(int createFlags);
OVRP_EXPORT ovrpResult ovrp_DestroyEnvironmentDepth();
OVRP_EXPORT ovrpResult ovrp_GetEnvironmentDepthTextureDesc(ovrpEnvironmentDepthTextureDesc* desc);
OVRP_EXPORT ovrpResult ovrp_GetEnvironmentDepthTextureStageCount(int* stageCount);
OVRP_EXPORT ovrpResult ovrp_GetEnvironmentDepthTexture(int stage, ovrpEye eyeId, ovrpTextureHandle* texture);
OVRP_EXPORT ovrpResult ovrp_SetEnvironmentDepthHandRemoval(ovrpBool enabled);
OVRP_EXPORT ovrpResult ovrp_StartEnvironmentDepth();
OVRP_EXPORT ovrpResult ovrp_StopEnvironmentDepth();
OVRP_EXPORT ovrpResult ovrp_GetEnvironmentDepthFrameDesc(ovrpEye eyeId, ovrpEnvironmentDepthFrameDesc* frameDesc);

OVRP_EXPORT ovrpResult ovrp_CreateDynamicObjectTracker(ovrpDynamicObjectTracker* trackerOut);
OVRP_EXPORT ovrpResult ovrp_DestroyDynamicObjectTracker(ovrpDynamicObjectTracker tracker);
OVRP_EXPORT ovrpResult ovrp_SetDynamicObjectTrackedClasses(
    ovrpDynamicObjectTracker tracker,
    const ovrpDynamicObjectTrackedClassesSetInfo* setInfo);
OVRP_EXPORT ovrpResult ovrp_GetSpaceDynamicObjectData(const ovrpSpace* space, ovrpDynamicObjectData* data);
OVRP_EXPORT ovrpResult ovrp_GetDynamicObjectTrackerSupported(ovrpBool* supported);
OVRP_EXPORT ovrpResult ovrp_GetDynamicObjectKeyboardSupported(ovrpBool* supported);


















OVRP_EXPORT ovrpResult ovrp_SetDualCoreBoostEnabled(ovrpBool enabled);
OVRP_EXPORT ovrpResult ovrp_SetProcessorFavor(int favorEnum);
OVRP_EXPORT ovrpResult ovrp_GetLowPowerModeEnabled(ovrpBool* enabled);

































































// XR_EXT_future
OVRP_EXPORT ovrpResult ovrp_PollFuture(ovrpFuture future, ovrpFutureState* state);
OVRP_EXPORT ovrpResult ovrp_CancelFuture(ovrpFuture future);

OVRP_EXPORT ovrpResult ovrp_GetGroupId(ovrpUuid* groupId);
OVRP_EXPORT ovrpResult ovrp_SetExternalLayerDynresEnabled(ovrpBool enabled);











OVRP_EXPORT ovrpResult ovrp_SendMicrogestureHint();









OVRP_EXPORT ovrpResult ovrp_GetStationaryReferenceSpaceId(ovrpUuid* generationId);

OVRP_EXPORT void ovrp_AllowVisibilityMask(ovrpBool enabled);
#ifdef __cplusplus
}
#endif

#endif
