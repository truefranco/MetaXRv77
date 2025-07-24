// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once
#include "CoreMinimal.h"
#include "OculusXRHMDTypes.h"
#include "OpenXR/IOculusXRExtensionPlugin.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOculusPerformanceExtensionPlugin, Log, All);

namespace OculusXR
{
	enum EPerformanceMetricsType
	{
		AppGPUTimeFloat,
		CompositorGPUTimeFloat,
		CompositorDroppedFramesInt,
		CompositorSpaceWarpModeInt,
		GPUUtilizationFloat,
		GPUUtilizationAverageFloat,
		GPUUtilizationWorstFloat,
		CPUCoreStart,
		CPUCore0UtilizationFloat,
		CPUCore1UtilizationFloat,
		CPUCore2UtilizationFloat,
		CPUCore3UtilizationFloat,
		CPUCore4UtilizationFloat,
		CPUCore5UtilizationFloat,
		CPUCore6UtilizationFloat,
		CPUCore7UtilizationFloat,
		CPUCoreLast = CPUCore7UtilizationFloat,
	};

	class FPerformanceExtensionPlugin : public IOculusXRExtensionPlugin
	{
	private:
		XrInstance Instance;
		bool bPerfSettingsInitialized;
		bool bPerfLevelsChanged;
		EOculusXRProcessorPerformanceLevel CpuPerfLevel;
		EOculusXRProcessorPerformanceLevel GpuPerfLevel;
		uint64_t MetricsUpdateMask;

		TMap<XrPath, EPerformanceMetricsType> PathPerformanceMetricsMap;
		FOculusXRPerformanceMetrics PerformanceMetrics;
		uint64 PerformanceMetricsMask;

	public:
		FPerformanceExtensionPlugin();

		// IOpenXRExtensionPlugin
		virtual bool GetRequiredExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual bool GetOptionalExtensions(TArray<const ANSICHAR*>& OutExtensions) override;
		virtual void PostCreateSession(XrSession InSession) override;
		virtual void PostCreateInstance(XrInstance InInstance) override;
		virtual void* OnWaitFrame(XrSession InSession, void* InNext) override;

		void UpdatePerformanceLevels(XrSession InSession);
		void GetSuggestedCpuAndGpuPerformanceLevels(EOculusXRProcessorPerformanceLevel& CpuPerfLevel, EOculusXRProcessorPerformanceLevel& GpuPerfLevel);
		void SetSuggestedCpuAndGpuPerformanceLevels(EOculusXRProcessorPerformanceLevel CpuPerfLevel, EOculusXRProcessorPerformanceLevel GpuPerfLevel);
		void LoadFromSettings();

		void InitializePerformanceMetrics(XrSession InSession);
		bool IsPerformanceMetricsSupported(EPerformanceMetricsType Metric) const;
		void UpdatePerformanceMetrics(XrSession InSession);
		const FOculusXRPerformanceMetrics& GetPerformanceMetrics() const;
	};

} // namespace OculusXR
