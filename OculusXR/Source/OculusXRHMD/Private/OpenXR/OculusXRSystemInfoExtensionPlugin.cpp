// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRSystemInfoExtensionPlugin.h"
#include "IOpenXRHMDModule.h"
#include "OculusXRAssetDirectory.h"
#include "OculusXRXRFunctions.h"
#include "OculusXROpenXRUtilities.h"
#include "OculusXRHMDRuntimeSettings.h"

DEFINE_LOG_CATEGORY(LogOculusSystemInfoExtensionPlugin);

namespace OculusXR
{
	enum EOculusAsset
	{
		LeftTouchRiftS,
		RightTouchRiftS,
		LeftTouchQuest2,
		RightTouchQuest2,
		LeftTouchQuestPro,
		RightTouchQuestPro,
		LeftTouchQuest3,
		RightTouchQuest3,
		OculusAssetTotal
	};

	struct FRenderableDevice
	{
		FString Device;
		EOculusXRDeviceType MinDeviceRange;
		EOculusXRDeviceType MaxDeviceRange;
		FSoftObjectPath MeshAssetRef;
	};

	static FRenderableDevice RenderableDevices[] = {
#if PLATFORM_ANDROID
		// Quest 1 & 2
		{ "/user/hand/left", EOculusXRDeviceType::OculusQuest_Deprecated, EOculusXRDeviceType::OculusQuest2, FOculusAssetDirectory::AssetListing[LeftTouchQuest2] },
		{ "/user/hand/right", EOculusXRDeviceType::OculusQuest_Deprecated, EOculusXRDeviceType::OculusQuest2, FOculusAssetDirectory::AssetListing[RightTouchQuest2] },

		// Quest Pro
		{ "/user/hand/left", EOculusXRDeviceType::MetaQuestPro, EOculusXRDeviceType::MetaQuestPro, FOculusAssetDirectory::AssetListing[LeftTouchQuestPro] },
		{ "/user/hand/right", EOculusXRDeviceType::MetaQuestPro, EOculusXRDeviceType::MetaQuestPro, FOculusAssetDirectory::AssetListing[RightTouchQuestPro] },

		// Quest 3 & 3s
		{ "/user/hand/left", EOculusXRDeviceType::MetaQuest3, EOculusXRDeviceType::MetaQuest3S, FOculusAssetDirectory::AssetListing[LeftTouchQuest3] },
		{ "/user/hand/right", EOculusXRDeviceType::MetaQuest3, EOculusXRDeviceType::MetaQuest3S, FOculusAssetDirectory::AssetListing[RightTouchQuest3] },
#else
		// PC - Rift S
		{ "/user/hand/left", EOculusXRDeviceType::Rift_S, EOculusXRDeviceType::Rift_S, FOculusAssetDirectory::AssetListing[LeftTouchRiftS] },
		{ "/user/hand/right", EOculusXRDeviceType::Rift_S, EOculusXRDeviceType::Rift_S, FOculusAssetDirectory::AssetListing[RightTouchRiftS] },

		// PC - Quest 1 & 2
		{ "/user/hand/left", EOculusXRDeviceType::Quest_Link_Deprecated, EOculusXRDeviceType::Quest2_Link, FOculusAssetDirectory::AssetListing[LeftTouchQuest2] },
		{ "/user/hand/right", EOculusXRDeviceType::Quest_Link_Deprecated, EOculusXRDeviceType::Quest2_Link, FOculusAssetDirectory::AssetListing[RightTouchQuest2] },

		// PC - Quest Pro
		{ "/user/hand/left", EOculusXRDeviceType::MetaQuestProLink, EOculusXRDeviceType::MetaQuestProLink, FOculusAssetDirectory::AssetListing[LeftTouchQuestPro] },
		{ "/user/hand/right", EOculusXRDeviceType::MetaQuestProLink, EOculusXRDeviceType::MetaQuestProLink, FOculusAssetDirectory::AssetListing[RightTouchQuestPro] },

		// Quest 3 & 3s
		{ "/user/hand/left", EOculusXRDeviceType::MetaQuest3Link, EOculusXRDeviceType::MetaQuest3SLink, FOculusAssetDirectory::AssetListing[LeftTouchQuest3] },
		{ "/user/hand/right", EOculusXRDeviceType::MetaQuest3Link, EOculusXRDeviceType::MetaQuest3SLink, FOculusAssetDirectory::AssetListing[RightTouchQuest3] },
#endif
	};
	static uint32 RenderableDeviceCount = sizeof(RenderableDevices) / sizeof(RenderableDevices[0]);

	FSoftObjectPath FindDeviceMesh(FString DevicePath, EOculusXRDeviceType SystemDeviceType)
	{
		if (DevicePath == "")
		{
			return NULL;
		}

		for (uint32 DeviceIndex = 0; DeviceIndex < RenderableDeviceCount; ++DeviceIndex)
		{
			const FRenderableDevice& RenderableDevice = RenderableDevices[DeviceIndex];
			if (RenderableDevice.Device == DevicePath)
			{
				// If we have information about the current headset, load the model based of the headset information, otherwise load defaults.
				if (SystemDeviceType != EOculusXRDeviceType::OculusUnknown)
				{
					if (SystemDeviceType >= RenderableDevice.MinDeviceRange && SystemDeviceType <= RenderableDevice.MaxDeviceRange)
					{
						return RenderableDevice.MeshAssetRef;
						break;
					}
				}
				else
				{
					return RenderableDevice.MeshAssetRef;
				}
			}
		}

		return NULL;
	}

	namespace // anonymous
	{
		void AppendToChain(XrBaseOutStructure* ToAppend, XrBaseOutStructure* Chain)
		{
			while (Chain->next != XR_NULL_HANDLE)
			{
				if (Chain->next == ToAppend)
				{
					return;
				}
				Chain = Chain->next;
			}
			Chain->next = ToAppend;
		}

		bool IsEqualUuid(const XrUuidEXT& a, const XrUuidEXT& b)
		{
			return FMemory::Memcmp(&a, &b, sizeof(XrUuidEXT)) == 0;
		}

		XrColorSpaceFB ToXrColorSpace(EOculusXRColorSpace ColorSpace)
		{
			XrColorSpaceFB XrColorSpace = XR_COLOR_SPACE_UNMANAGED_FB;
			switch (ColorSpace)
			{
				case EOculusXRColorSpace::Unmanaged:
					XrColorSpace = XR_COLOR_SPACE_UNMANAGED_FB;
					break;
				case EOculusXRColorSpace::Rec_2020:
					XrColorSpace = XR_COLOR_SPACE_REC2020_FB;
					break;
				case EOculusXRColorSpace::Rec_709:
					XrColorSpace = XR_COLOR_SPACE_REC709_FB;
					break;
				case EOculusXRColorSpace::Rift_CV1:
					XrColorSpace = XR_COLOR_SPACE_RIFT_CV1_FB;
					break;
				case EOculusXRColorSpace::Rift_S:
					XrColorSpace = XR_COLOR_SPACE_RIFT_S_FB;
					break;
				case EOculusXRColorSpace::Quest:
					XrColorSpace = XR_COLOR_SPACE_QUEST_FB;
					break;
				case EOculusXRColorSpace::P3:
					XrColorSpace = XR_COLOR_SPACE_P3_FB;
					break;
				case EOculusXRColorSpace::Adobe_RGB:
					XrColorSpace = XR_COLOR_SPACE_ADOBE_RGB_FB;
					break;
			}
			return XrColorSpace;
		}

		EOculusXRColorSpace ToOculusXRColorSpace(XrColorSpaceFB InXrColorSpace)
		{
			EOculusXRColorSpace ColorSpace = EOculusXRColorSpace::Unknown;

			switch (InXrColorSpace)
			{
				case XR_COLOR_SPACE_UNMANAGED_FB:
					ColorSpace = EOculusXRColorSpace::Unmanaged;
					break;
				case XR_COLOR_SPACE_REC2020_FB:
					ColorSpace = EOculusXRColorSpace::Rec_2020;
					break;
				case XR_COLOR_SPACE_REC709_FB:
					ColorSpace = EOculusXRColorSpace::Rec_709;
					break;
				case XR_COLOR_SPACE_RIFT_CV1_FB:
					ColorSpace = EOculusXRColorSpace::Rift_CV1;
					break;
				case XR_COLOR_SPACE_RIFT_S_FB:
					ColorSpace = EOculusXRColorSpace::Rift_S;
					break;
				case XR_COLOR_SPACE_QUEST_FB:
					ColorSpace = EOculusXRColorSpace::Quest;
					break;
				case XR_COLOR_SPACE_P3_FB:
					ColorSpace = EOculusXRColorSpace::P3;
					break;
				case XR_COLOR_SPACE_ADOBE_RGB_FB:
					ColorSpace = EOculusXRColorSpace::Adobe_RGB;
					break;
				default:
					ColorSpace = EOculusXRColorSpace::Unknown;
			}
			return ColorSpace;
		}
	} // namespace

	FSystemInfoExtensionPlugin::FSystemInfoExtensionPlugin()
		: Instance(XR_NULL_HANDLE)
		, Session(XR_NULL_HANDLE)
		, SystemId(XR_NULL_SYSTEM_ID)
		, bExtHeadsetIdAvailable(false)
		, SystemHeadsetId{}
		, bSystemHeadsetIdValid(false)
		, SystemProductName{}
		, bExtDisplayRefreshAvailible(false)
		, bExtColorspaceAvailable(false)
		, bExtPassthroughAvailable(false)
		, bExtPassthroughPreferencesAvailable(false)
	{
	}

	bool FSystemInfoExtensionPlugin::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		return true;
	}

	bool FSystemInfoExtensionPlugin::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_META_HEADSET_ID_EXTENSION_NAME);
		OutExtensions.Add(XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME);
		OutExtensions.Add(XR_FB_COLOR_SPACE_EXTENSION_NAME);
		OutExtensions.Add(XR_META_PASSTHROUGH_PREFERENCES_EXTENSION_NAME);

		return true;
	}

	void FSystemInfoExtensionPlugin::PostCreateInstance(XrInstance InInstance)
	{
		Instance = InInstance;
	}

	const void* FSystemInfoExtensionPlugin::OnCreateInstance(class IOpenXRHMDModule* InModule, const void* InNext)
	{
		if (InModule != nullptr)
		{
			bExtHeadsetIdAvailable = InModule->IsExtensionEnabled(XR_META_HEADSET_ID_EXTENSION_NAME);
			bExtDisplayRefreshAvailible = InModule->IsExtensionEnabled(XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME);
			bExtColorspaceAvailable = InModule->IsExtensionEnabled(XR_FB_COLOR_SPACE_EXTENSION_NAME);
			bExtPassthroughAvailable = InModule->IsExtensionEnabled(XR_FB_PASSTHROUGH_EXTENSION_NAME);
			bExtPassthroughPreferencesAvailable = InModule->IsExtensionEnabled(XR_META_PASSTHROUGH_PREFERENCES_EXTENSION_NAME);
		}
		return IOculusXRExtensionPlugin::OnCreateInstance(InModule, InNext);
	}

	void FSystemInfoExtensionPlugin::PostCreateSession(XrSession InSession)
	{
		Session = InSession;
		XrSystemGetInfo SystemGetInfo = { XR_TYPE_SYSTEM_GET_INFO };
		SystemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

		ENSURE_XRCMD(xrGetSystem(Instance, &SystemGetInfo, &SystemId));

		XrSystemHeadsetIdPropertiesMETA SystemHeadsetIdProperties = { XR_TYPE_SYSTEM_HEADSET_ID_PROPERTIES_META };

		XrSystemProperties SystemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

		if (bExtHeadsetIdAvailable)
		{
			AppendToChain(
				reinterpret_cast<XrBaseOutStructure*>(&SystemHeadsetIdProperties),
				reinterpret_cast<XrBaseOutStructure*>(&SystemProperties));
		}

		ENSURE_XRCMD(xrGetSystemProperties(Instance, SystemId, &SystemProperties));

		if (bExtHeadsetIdAvailable)
		{
			SystemHeadsetId = SystemHeadsetIdProperties.id;
			bSystemHeadsetIdValid = true;
		}

		SystemProductName = SystemProperties.systemName;
		SystemDeviceType = GetSystemHeadsetType();

		const UOculusXRHMDRuntimeSettings* HMDSettings = GetDefault<UOculusXRHMDRuntimeSettings>();
		SetColorSpace(HMDSettings->ColorSpace);

		ControllerPaths.TouchControllerPath = FOpenXRPath("/interaction_profiles/oculus/touch_controller");
		ControllerPaths.TouchControllerProPath = FOpenXRPath("/interaction_profiles/facebook/touch_controller_pro");
		ControllerPaths.TouchControllerPlusPath = FOpenXRPath("/interaction_profiles/meta/touch_controller_plus");
		ControllerPaths.LeftHandPath = FOpenXRPath("/user/hand/left");
		ControllerPaths.RightHandPath = FOpenXRPath("/user/hand/right");
	}

	bool FSystemInfoExtensionPlugin::GetControllerModel(XrInstance InInstance, XrPath InInteractionProfile, XrPath InDevicePath, FSoftObjectPath& OutPath)
	{
		if (InDevicePath == XR_NULL_PATH)
		{
			return false;
		}

		const FString DevicePath = FOpenXRPath(InDevicePath).ToString();
		OutPath = FindDeviceMesh(DevicePath, SystemDeviceType);
		return OutPath.IsValid();
	}

	void FSystemInfoExtensionPlugin::GetControllerModelsForCooking(TArray<FSoftObjectPath>& OutPaths)
	{
		OutPaths.Append(FOculusAssetDirectory::AssetListing, OculusAssetTotal);
	}

	EOculusXRDeviceType FSystemInfoExtensionPlugin::GetDeviceType()
	{
		return SystemDeviceType;
	}

	EOculusXRDeviceType FSystemInfoExtensionPlugin::GetSystemHeadsetType()
	{
		// Magic legacy conversion paths from ovrplugin

#if defined(__ANDROID__)

		constexpr XrUuidEXT XR_HEADSET_ID_QUEST = {
			{ 0x4c, 0xa2, 0xf7, 0x94, 0xaf, 0x6a, 0x46, 0x9d, 0xbb, 0xe5, 0x1f, 0x7f, 0xce, 0xef, 0x7b, 0xad }
		};
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST2 = {
			{ 0x30, 0x64, 0xf2, 0x39, 0x15, 0xed, 0x47, 0x22, 0xa6, 0x59, 0x5e, 0xa1, 0xec, 0xd8, 0x99, 0xc2 }
		};
		// Quest Pro
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST_PRO = {
			{ 0xb5, 0x84, 0x43, 0xc9, 0xdf, 0x86, 0x40, 0xcd, 0x89, 0x18, 0x2a, 0x27, 0x8e, 0x18, 0xab, 0x71 }
		};
		// Quest 3
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST_3 = {
			{ 0x08, 0x27, 0x46, 0xba, 0xa0, 0x71, 0x4c, 0xf7, 0x8f, 0x87, 0xba, 0xde, 0x5c, 0xce, 0x43, 0xce }
		};
		// Quest 3S
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST_3S = {
			{ 0x95, 0x10, 0x49, 0x2b, 0x0d, 0x42, 0x48, 0x72, 0xbc, 0xf4, 0xfb, 0x1a, 0x8d, 0xef, 0x6f, 0x0e }
		};
#else
		// PC Headsets
		constexpr XrUuidEXT XR_HEADSET_ID_RIFT_CV1 = {
			{ 0xbe, 0x4f, 0x8d, 0x7c, 0x8e, 0x33, 0x4d, 0xa2, 0xbf, 0x54, 0xda, 0xd9, 0xea, 0x57, 0x54, 0x9f }
		};
		constexpr XrUuidEXT XR_HEADSET_ID_RIFT_S = {
			{ 0x0d, 0x42, 0xaa, 0xc6, 0x7f, 0x1c, 0x46, 0x27, 0x9c, 0xc5, 0x2c, 0xba, 0x8b, 0x0b, 0xaf, 0xfc }
		};
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST_LINK = {
			{ 0x2f, 0x03, 0x26, 0xc0, 0x09, 0xeb, 0x4d, 0xa9, 0x9a, 0x88, 0x4c, 0xc2, 0x93, 0x53, 0x0a, 0xb2 }
		};
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST2_LINK = {
			{ 0x52, 0x50, 0x78, 0xa5, 0x5a, 0xab, 0x4d, 0xc9, 0xb8, 0x02, 0x1e, 0xbd, 0x6c, 0x7c, 0xff, 0xf4 }
		};
		// Quest Pro
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST_PRO_LINK = {
			{ 0x99, 0x09, 0x86, 0xc7, 0x9b, 0xad, 0x47, 0x5f, 0x89, 0x83, 0xc1, 0xc7, 0xbd, 0x49, 0xad, 0x51 }
		};
		// Quest 3
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST_3_LINK = {
			{ 0x64, 0x09, 0xe3, 0xb3, 0x50, 0x89, 0x4e, 0xd6, 0x86, 0x8e, 0xaa, 0xed, 0x82, 0xda, 0x36, 0x76 }
		};
		// Quest 3S
		constexpr XrUuidEXT XR_HEADSET_ID_QUEST_3S_LINK = {
			{ 0x8c, 0x2e, 0xc4, 0x47, 0xac, 0x73, 0x41, 0x9f, 0x90, 0x96, 0x36, 0xa7, 0x72, 0x33, 0x78, 0x62 }
		};
#endif // defined(__ANDROID__)

#if defined(__ANDROID__)
		if (bSystemHeadsetIdValid)
		{
			UE_LOG(LogOculusSystemInfoExtensionPlugin, Log, TEXT("UID %x %x %x %x - %x %x %x %x - %x %x %x %x - %x %x %x %x"),
				SystemHeadsetId.data[0],
				SystemHeadsetId.data[1],
				SystemHeadsetId.data[2],
				SystemHeadsetId.data[3],

				SystemHeadsetId.data[4],
				SystemHeadsetId.data[5],
				SystemHeadsetId.data[6],
				SystemHeadsetId.data[7],

				SystemHeadsetId.data[8],
				SystemHeadsetId.data[9],
				SystemHeadsetId.data[10],
				SystemHeadsetId.data[11],

				SystemHeadsetId.data[12],
				SystemHeadsetId.data[13],
				SystemHeadsetId.data[14],
				SystemHeadsetId.data[15]);

			if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST))
			{
				return EOculusXRDeviceType::OculusQuest_Deprecated;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST2))
			{
				return EOculusXRDeviceType::OculusQuest2;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST_PRO))
			{
				return EOculusXRDeviceType::MetaQuestPro;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST_3))
			{
				return EOculusXRDeviceType::MetaQuest3;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST_3S))
			{
				return EOculusXRDeviceType::MetaQuest3S;
			}
			else
			{
				return EOculusXRDeviceType::OculusQuest2; // return Quest 2 as the default headset type
			}
		}
		else
		{
			if (SystemProductName == TEXT("Oculus Quest"))
			{
				return EOculusXRDeviceType::OculusQuest_Deprecated;
			}
			else if (SystemProductName == TEXT("Oculus Quest2"))
			{
				return EOculusXRDeviceType::OculusQuest2;
			}
			else if (SystemProductName == TEXT("Meta Quest Pro") || SystemProductName == TEXT("Oculus Headset1"))
			{
				return EOculusXRDeviceType::MetaQuestPro;
			}
			else if (SystemProductName == TEXT("Meta Quest 3") || SystemProductName == TEXT("Oculus Headset2"))
			{
				return EOculusXRDeviceType::MetaQuest3;
			}
			else if (SystemProductName == TEXT("Meta Quest 3S") || SystemProductName == TEXT("Oculus Headset3"))
			{
				return EOculusXRDeviceType::MetaQuest3S;
			}
			else
			{
				return EOculusXRDeviceType::OculusQuest2; // return Quest 2 as the default headset type
			}
		}
#else
		if (bSystemHeadsetIdValid)
		{
			if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_RIFT_CV1))
			{
				return EOculusXRDeviceType::Rift;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_RIFT_S))
			{
				return EOculusXRDeviceType::Rift_S;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST_LINK))
			{
				return EOculusXRDeviceType::Quest_Link_Deprecated;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST2_LINK))
			{
				return EOculusXRDeviceType::Quest2_Link;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST_PRO_LINK))
			{
				return EOculusXRDeviceType::MetaQuestProLink;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST_3_LINK))
			{
				return EOculusXRDeviceType::MetaQuest3Link;
			}
			else if (IsEqualUuid(SystemHeadsetId, XR_HEADSET_ID_QUEST_3S_LINK))
			{
				return EOculusXRDeviceType::MetaQuest3SLink;
			}
			else
			{
				return EOculusXRDeviceType::Quest2_Link; // return Quest 2 (over Link) as the default headset type
			}
		}
		else
		{
			if (SystemProductName == TEXT("Oculus Rift S"))
			{
				return EOculusXRDeviceType::Rift_S;
			}
			else if (SystemProductName == TEXT("Oculus Rift CV1"))
			{
				return EOculusXRDeviceType::Rift;
			}
			else
			{
				return EOculusXRDeviceType::Quest2_Link; // return Quest 2 (over Link) as the default headset type
			}
		}
#endif // defined(__ANDROID__)
	}

	TArray<float> FSystemInfoExtensionPlugin::GetSystemDisplayAvailableFrequencies()
	{
		TArray<float> DisplayFrequencies;

		if (bExtDisplayRefreshAvailible)
		{
			check(xrEnumerateDisplayRefreshRatesFB.GetValue() != nullptr);

			uint32 TotalRates = 0;
			ENSURE_XRCMD(xrEnumerateDisplayRefreshRatesFB.GetValue()(Session, 0, &TotalRates, nullptr));

			DisplayFrequencies.Init(0, TotalRates);

			ENSURE_XRCMD(xrEnumerateDisplayRefreshRatesFB.GetValue()(Session, TotalRates, &TotalRates, DisplayFrequencies.GetData()));
		}

		return DisplayFrequencies;
	}

	float FSystemInfoExtensionPlugin::GetSystemDisplayFrequency()
	{
		float Frequency = 0.0f;

		if (bExtDisplayRefreshAvailible)
		{
			check(xrGetDisplayRefreshRateFB.GetValue() != nullptr);
			ENSURE_XRCMD(xrGetDisplayRefreshRateFB.GetValue()(Session, &Frequency));
		}
		return Frequency;
	}

	void FSystemInfoExtensionPlugin::SetSystemDisplayFrequency(float DisplayFrequency)
	{
		if (bExtDisplayRefreshAvailible)
		{
			check(xrRequestDisplayRefreshRateFB.GetValue() != nullptr);
			ENSURE_XRCMD(xrRequestDisplayRefreshRateFB.GetValue()(Session, DisplayFrequency));
		}
	}

	void FSystemInfoExtensionPlugin::SetColorSpace(EOculusXRColorSpace ColorSpace)
	{
		if (bExtColorspaceAvailable)
		{
			XrColorSpaceFB XrColorSpace = ToXrColorSpace(ColorSpace);
			check(xrSetColorSpaceFB.GetValue() != nullptr);
			ENSURE_XRCMD(xrSetColorSpaceFB.GetValue()(Session, XrColorSpace));
		}
	}

	EOculusXRColorSpace FSystemInfoExtensionPlugin::GetColorSpace()
	{
		EOculusXRColorSpace ColorSpace = EOculusXRColorSpace::Unknown;
		if (bExtColorspaceAvailable)
		{
			XrSystemColorSpacePropertiesFB ColorSpaceProperties = { XR_TYPE_SYSTEM_COLOR_SPACE_PROPERTIES_FB };
			XrSystemProperties SystemProperties = { XR_TYPE_SYSTEM_PROPERTIES, &ColorSpaceProperties };

			ENSURE_XRCMD(xrGetSystemProperties(Instance, SystemId, &SystemProperties));
			ColorSpace = ToOculusXRColorSpace(ColorSpaceProperties.colorSpace);
		}
		return ColorSpace;
	}

	bool FSystemInfoExtensionPlugin::IsPassthroughSupported()
	{
		if (bExtPassthroughAvailable)
		{
			XrSystemPassthroughProperties2FB PassthroughProperties = { XR_TYPE_SYSTEM_PASSTHROUGH_PROPERTIES2_FB };
			XrSystemProperties SystemProperties = { XR_TYPE_SYSTEM_PROPERTIES, &PassthroughProperties };

			XR_ENSURE(xrGetSystemProperties(Instance, SystemId, &SystemProperties));

			return (PassthroughProperties.capabilities & XR_PASSTHROUGH_CAPABILITY_BIT_FB) == XR_PASSTHROUGH_CAPABILITY_BIT_FB;
		}
		return false;
	}

	bool FSystemInfoExtensionPlugin::IsColorPassthroughSupported()
	{
		if (bExtPassthroughAvailable)
		{
			XrSystemPassthroughProperties2FB PassthroughProperties = { XR_TYPE_SYSTEM_PASSTHROUGH_PROPERTIES2_FB };
			XrSystemProperties SystemProperties = { XR_TYPE_SYSTEM_PROPERTIES, &PassthroughProperties };

			XR_ENSURE(xrGetSystemProperties(Instance, SystemId, &SystemProperties));

			return (PassthroughProperties.capabilities & XR_PASSTHROUGH_CAPABILITY_COLOR_BIT_FB) == XR_PASSTHROUGH_CAPABILITY_COLOR_BIT_FB;
		}
		return false;
	}

	bool FSystemInfoExtensionPlugin::IsPassthroughRecommended()
	{
		if (bExtPassthroughPreferencesAvailable && OculusXR::xrGetPassthroughPreferencesMETA.GetValue())
		{
			XrPassthroughPreferencesMETA PassthroughPreferences = { XR_TYPE_PASSTHROUGH_PREFERENCES_META };

			ENSURE_XRCMD(OculusXR::xrGetPassthroughPreferencesMETA.GetValue()(Session, &PassthroughPreferences));

			return (PassthroughPreferences.flags & XR_PASSTHROUGH_PREFERENCE_DEFAULT_TO_ACTIVE_BIT_META) == XR_PASSTHROUGH_PREFERENCE_DEFAULT_TO_ACTIVE_BIT_META;
		}
		return false;
	}

	EOculusXRControllerType FSystemInfoExtensionPlugin::GetControllerType(EControllerHand DeviceHand)
	{
		EOculusXRControllerType ControllerType = EOculusXRControllerType::Unknown;
		IXRTrackingSystem* TrackingSystem = GetOpenXRTrackingSystem();

		if (TrackingSystem != nullptr)
		{
			if (DeviceHand != EControllerHand::Left || DeviceHand != EControllerHand::Right)
			{
				XrInteractionProfileState Profile;
				Profile.type = XR_TYPE_INTERACTION_PROFILE_STATE;
				Profile.next = nullptr;
				const XrPath TopLevelUserPath = DeviceHand != EControllerHand::Left ? ControllerPaths.LeftHandPath : ControllerPaths.RightHandPath;
				XR_ENSURE(xrGetCurrentInteractionProfile(Session, TopLevelUserPath, &Profile));

				const XrPath InteractionProfile = Profile.interactionProfile;
				if (InteractionProfile == ControllerPaths.TouchControllerPath)
				{
					ControllerType = EOculusXRControllerType::MetaQuestTouch;
				}
				else if (InteractionProfile == ControllerPaths.TouchControllerProPath)
				{
					ControllerType = EOculusXRControllerType::MetaQuestTouchPro;
				}
				else if (InteractionProfile == ControllerPaths.TouchControllerPlusPath)
				{
					ControllerType = EOculusXRControllerType::MetaQuestTouchPlus;
				}
			}
		}
		return ControllerType;
	}

} // namespace OculusXR
