// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRPerformanceExtensionPlugin.h"
#include "OculusXRXRFunctions.h"
#include "OculusXROpenXRUtilities.h"
#include "OculusXRHMDRuntimeSettings.h"
#include "OpenXRCore.h"

DEFINE_LOG_CATEGORY(LogOculusPerformanceExtensionPlugin);

namespace OculusXR
{

	namespace // anonymous
	{
		XrPerfSettingsLevelEXT ToXrPerfSettingsLevel(EOculusXRProcessorPerformanceLevel PerformanceLevel)
		{
			switch (PerformanceLevel)
			{
				case EOculusXRProcessorPerformanceLevel::PowerSavings:
					return XR_PERF_SETTINGS_LEVEL_POWER_SAVINGS_EXT;
				case EOculusXRProcessorPerformanceLevel::SustainedLow:
					return XR_PERF_SETTINGS_LEVEL_SUSTAINED_LOW_EXT;
				case EOculusXRProcessorPerformanceLevel::SustainedHigh:
					return XR_PERF_SETTINGS_LEVEL_SUSTAINED_HIGH_EXT;
				case EOculusXRProcessorPerformanceLevel::Boost:
					return XR_PERF_SETTINGS_LEVEL_BOOST_EXT;
				default:
					check(false);
			}
			return XR_PERF_SETTINGS_LEVEL_POWER_SAVINGS_EXT;
		}

		static constexpr int MaxCPUCores = CPUCoreLast - CPUCoreStart;

	} // namespace

	FPerformanceExtensionPlugin::FPerformanceExtensionPlugin()
		: Instance(XR_NULL_HANDLE)
		, bPerfSettingsInitialized(false)
		, bPerfLevelsChanged(false)
		, CpuPerfLevel(EOculusXRProcessorPerformanceLevel::PowerSavings)
		, GpuPerfLevel(EOculusXRProcessorPerformanceLevel::PowerSavings)
		, MetricsUpdateMask(0)
		, PathPerformanceMetricsMap{}
		, PerformanceMetrics{}
		, PerformanceMetricsMask(0)
	{
		PerformanceMetrics.CpuCoreUtil.Init(0, MaxCPUCores);
	}

	bool FPerformanceExtensionPlugin::GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		return true;
	}

	bool FPerformanceExtensionPlugin::GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions)
	{
		OutExtensions.Add(XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME);
		OutExtensions.Add(XR_META_PERFORMANCE_METRICS_EXTENSION_NAME);
		return true;
	}

	void FPerformanceExtensionPlugin::PostCreateSession(XrSession InSession)
	{
		const UOculusXRHMDRuntimeSettings* HMDSettings = GetDefault<UOculusXRHMDRuntimeSettings>();
		LoadFromSettings();
		InitializePerformanceMetrics(InSession);
	}

	void* FPerformanceExtensionPlugin::OnWaitFrame(XrSession InSession, void* InNext)
	{
		UpdatePerformanceLevels(InSession);
		UpdatePerformanceMetrics(InSession);
		return InNext;
	}

	void FPerformanceExtensionPlugin::PostCreateInstance(XrInstance InInstance)
	{
		Instance = InInstance;
	}

	void FPerformanceExtensionPlugin::UpdatePerformanceLevels(XrSession InSession)
	{
		if (bPerfLevelsChanged && xrPerfSettingsSetPerformanceLevelEXT.IsSet() && xrPerfSettingsSetPerformanceLevelEXT.GetValue() != nullptr)
		{
			UE_LOG(LogOculusPerformanceExtensionPlugin, Log, TEXT("Oculus OpenXR SetPerformanceLevel CPU=%d, GPU=%d"), CpuPerfLevel, GpuPerfLevel);
			ENSURE_XRCMD(xrPerfSettingsSetPerformanceLevelEXT.GetValue()(InSession, XR_PERF_SETTINGS_DOMAIN_CPU_EXT, ToXrPerfSettingsLevel(CpuPerfLevel)));
			ENSURE_XRCMD(xrPerfSettingsSetPerformanceLevelEXT.GetValue()(InSession, XR_PERF_SETTINGS_DOMAIN_GPU_EXT, ToXrPerfSettingsLevel(GpuPerfLevel)));
			bPerfLevelsChanged = false;
		}
	}

	void FPerformanceExtensionPlugin::GetSuggestedCpuAndGpuPerformanceLevels(EOculusXRProcessorPerformanceLevel& CpuLevel, EOculusXRProcessorPerformanceLevel& GpuLevel)
	{
		if (!bPerfSettingsInitialized)
		{
			LoadFromSettings();
		}
		CpuLevel = CpuPerfLevel;
		GpuLevel = GpuPerfLevel;
	}

	void FPerformanceExtensionPlugin::SetSuggestedCpuAndGpuPerformanceLevels(EOculusXRProcessorPerformanceLevel CpuLevel, EOculusXRProcessorPerformanceLevel GpuLevel)
	{
		if (CpuPerfLevel != CpuLevel || GpuPerfLevel != GpuLevel)
		{
			CpuPerfLevel = CpuLevel;
			GpuPerfLevel = GpuLevel;
			bPerfLevelsChanged = true;
		}
		bPerfSettingsInitialized = true;
	}

	void FPerformanceExtensionPlugin::LoadFromSettings()
	{
		const UOculusXRHMDRuntimeSettings* HMDSettings = GetDefault<UOculusXRHMDRuntimeSettings>();
		SetSuggestedCpuAndGpuPerformanceLevels(HMDSettings->SuggestedCpuPerfLevel, HMDSettings->SuggestedGpuPerfLevel);
	}

	void FPerformanceExtensionPlugin::InitializePerformanceMetrics(XrSession InSession)
	{
		if (xrEnumeratePerformanceMetricsCounterPathsMETA.GetValue() == 0 || xrSetPerformanceMetricsStateMETA.GetValue() == 0 || xrQueryPerformanceMetricsCounterMETA.GetValue() == 0)
		{
			return;
		}

		TArray<XrPath> PerfMetricsCounterPaths;
		uint32 NumPerfMetricsCounterPaths = 0;
		XrPerformanceMetricsStateMETA MetricsState = { XR_TYPE_PERFORMANCE_METRICS_STATE_META };
		MetricsState.enabled = XR_TRUE;
		ENSURE_XRCMD(xrSetPerformanceMetricsStateMETA.GetValue()(InSession, &MetricsState));
		ENSURE_XRCMD(xrEnumeratePerformanceMetricsCounterPathsMETA.GetValue()(Instance, 0, &NumPerfMetricsCounterPaths, nullptr));
		PerfMetricsCounterPaths.SetNum(NumPerfMetricsCounterPaths);
		ENSURE_XRCMD(xrEnumeratePerformanceMetricsCounterPathsMETA.GetValue()(Instance, NumPerfMetricsCounterPaths, &NumPerfMetricsCounterPaths, PerfMetricsCounterPaths.GetData()));

		TSet<XrPath> PerfMetricsCounterPathHash;

		for (auto& Path : PerfMetricsCounterPaths)
		{
			PerfMetricsCounterPathHash.Emplace(Path);
		}

		auto AddMetrics = [&](FString Name, EPerformanceMetricsType Metric) {
			const FOpenXRPath MetricsPath(Name);
			if (PerfMetricsCounterPathHash.Contains(MetricsPath))
			{
				PathPerformanceMetricsMap.Add(MetricsPath, Metric);
				PerformanceMetricsMask |= static_cast<uint64>(1) << Metric;
				UE_LOG(LogOculusPerformanceExtensionPlugin, Log, TEXT("Supported Performance Metrics: %s"), *Name);
			}
		};

		AddMetrics(TEXT("/perfmetrics_meta/app/gpu_frametime"), AppGPUTimeFloat);
		AddMetrics(TEXT("/perfmetrics_meta/compositor/gpu_frametime"), CompositorGPUTimeFloat);
		AddMetrics(TEXT("/perfmetrics_meta/compositor/dropped_frame_count"), CompositorDroppedFramesInt);
		AddMetrics(TEXT("/perfmetrics_meta/compositor/spacewarp_mode"), CompositorSpaceWarpModeInt);
		AddMetrics(TEXT("/perfmetrics_meta/device/gpu_utilization"), GPUUtilizationFloat);
		AddMetrics(TEXT("/perfmetrics_meta/device/cpu_utilization_average"), GPUUtilizationAverageFloat);
		AddMetrics(TEXT("/perfmetrics_meta/device/cpu_utilization_worst"), GPUUtilizationWorstFloat);

		for (uint32 CPUId = 0; CPUId < MaxCPUCores; ++CPUId)
		{
			const FString PerCpuCounterPathString = FString::Printf(TEXT("/perfmetrics_meta/device/cpu%d_utilization"), CPUId);
			AddMetrics(PerCpuCounterPathString,
				static_cast<EPerformanceMetricsType>(CPUCore0UtilizationFloat + CPUId));
		}
	}

	bool FPerformanceExtensionPlugin::IsPerformanceMetricsSupported(EPerformanceMetricsType Metric) const
	{
		const uint64_t Val = static_cast<uint64>(1) << Metric;
		return ((PerformanceMetricsMask & Val) != 0);
	}

	void FPerformanceExtensionPlugin::UpdatePerformanceMetrics(XrSession InSession)
	{
		if ((xrQueryPerformanceMetricsCounterMETA.GetValue() == 0) || (PerformanceMetricsMask == 0))
		{
			return;
		}

		for (auto& Pair : PathPerformanceMetricsMap)
		{
			const XrPath Path = Pair.Key;
			const EPerformanceMetricsType Metric = Pair.Value;
			XrPerformanceMetricsCounterMETA Counter = { XR_TYPE_PERFORMANCE_METRICS_COUNTER_META };
			ENSURE_XRCMD(xrQueryPerformanceMetricsCounterMETA.GetValue()(InSession, Path, &Counter));

			switch (Metric)
			{
				case AppGPUTimeFloat:
					PerformanceMetrics.AppGpuTime = Counter.floatValue;
					break;
				case CompositorGPUTimeFloat:
					PerformanceMetrics.ComGpuTime = Counter.floatValue;
					break;
				case CompositorDroppedFramesInt:
					PerformanceMetrics.DroppedFrames = Counter.uintValue;
					break;
				case CompositorSpaceWarpModeInt:
					PerformanceMetrics.ComSpaceWarpMode = Counter.uintValue;
					break;
				case GPUUtilizationFloat:
					PerformanceMetrics.GpuUtil = Counter.floatValue;
					break;
				case GPUUtilizationAverageFloat:
					PerformanceMetrics.CpuUtilAvg = Counter.floatValue;
					break;
				case GPUUtilizationWorstFloat:
					PerformanceMetrics.CpuUtilWorst = Counter.floatValue;
					break;
				default:
				{
					if (Metric >= CPUCore0UtilizationFloat && Metric < CPUCore0UtilizationFloat + MaxCPUCores)
					{
						const int32 CpuCoreIdx = Metric - CPUCore0UtilizationFloat;
						check(CpuCoreIdx < PerformanceMetrics.CpuCoreUtil.Num());
						PerformanceMetrics.CpuCoreUtil[CpuCoreIdx] = Counter.floatValue / 100.0f;
					}
					else
					{
						UE_LOG(LogOculusPerformanceExtensionPlugin, Log, TEXT("Unsupported Metric: %d"), static_cast<uint32>(Metric));
					}
				}
				break;
			}
		}
	}

	const FOculusXRPerformanceMetrics& FPerformanceExtensionPlugin::GetPerformanceMetrics() const
	{
		return PerformanceMetrics;
	}

} // namespace OculusXR
