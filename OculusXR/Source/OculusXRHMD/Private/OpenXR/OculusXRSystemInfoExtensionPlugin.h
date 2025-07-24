// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "CoreMinimal.h"
#include "OculusXRHMDTypes.h"
#include "OpenXR/IOculusXRExtensionPlugin.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOculusSystemInfoExtensionPlugin, Log, All);

namespace OculusXR
{
	class FSystemInfoExtensionPlugin : public IOculusXRExtensionPlugin
	{
	public:
		FSystemInfoExtensionPlugin();

		// IOpenXRExtensionPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual const void* OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext);
		virtual void PostCreateInstance(XrInstance InInstance) override;
		virtual void PostCreateSession(XrSession InSession) override;
		virtual bool GetControllerModel(XrInstance InInstance, XrPath InInteractionProfile, XrPath InDevicePath, FSoftObjectPath& OutPath) override;
		virtual void GetControllerModelsForCooking(TArray<FSoftObjectPath>& OutPaths) override;

		FString GetSystemProductName();
		EOculusXRDeviceType GetDeviceType();

		TArray<float> GetSystemDisplayAvailableFrequencies();
		float GetSystemDisplayFrequency();
		void SetSystemDisplayFrequency(float DisplayFrequency);
		void SetColorSpace(EOculusXRColorSpace ColorSpace);
		EOculusXRColorSpace GetColorSpace();
		EOculusXRControllerType GetControllerType(EControllerHand DeviceHand);

		bool IsPassthroughSupported();
		bool IsColorPassthroughSupported();
		bool IsPassthroughRecommended();

	private:
		EOculusXRDeviceType GetSystemHeadsetType();

		XrInstance Instance;
		XrSession Session;
		XrSystemId SystemId;
		bool bExtHeadsetIdAvailable;
		XrUuidEXT SystemHeadsetId;
		bool bSystemHeadsetIdValid;
		FString SystemProductName;
		EOculusXRDeviceType SystemDeviceType;
		bool bExtDisplayRefreshAvailible;
		bool bExtColorspaceAvailable;
		bool bExtPassthroughAvailable;
		bool bExtPassthroughPreferencesAvailable;

		struct FControllerPaths
		{
			FControllerPaths()
				: TouchControllerPath(XR_NULL_PATH)
				, TouchControllerProPath(XR_NULL_PATH)
				, TouchControllerPlusPath(XR_NULL_PATH)
				, LeftHandPath(XR_NULL_PATH)
				, RightHandPath(XR_NULL_PATH){};

			XrPath TouchControllerPath;
			XrPath TouchControllerProPath;
			XrPath TouchControllerPlusPath;
			XrPath LeftHandPath;
			XrPath RightHandPath;
		};

		FControllerPaths ControllerPaths;
	};

} // namespace OculusXR
