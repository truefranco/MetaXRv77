// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.

#include "OculusXRSimulator.h"

#if PLATFORM_WINDOWS
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "libzip/zip.h"

#include "HAL/FileManager.h"
#include "OculusXRHMDRuntimeSettings.h"
#include "OculusXRTelemetryEvents.h"
#include "Misc/MessageDialog.h"
#include "OpenXR/OculusXROpenXRUtilities.h"
#include "Internationalization/Regex.h"

#include "Windows/WindowsPlatformMisc.h"
#include "Interfaces/IPluginManager.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#if WITH_EDITOR
#include "UnrealEdMisc.h"
#endif // WITH_EDITOR

const FString OpenXrRuntimeEnvKey = "XR_RUNTIME_JSON";
const FString PreviousOpenXrRuntimeEnvKey = "XR_RUNTIME_JSON_PREV";

namespace
{
	class FZipArchiveReader
	{
	public:
		FZipArchiveReader(IFileHandle* InFileHandle);
		~FZipArchiveReader();

		bool IsValid() const;
		TArray<FString> GetFileNames() const;
		bool TryReadFile(FStringView FileName, TArray<uint8>& OutData) const;

	private:
		TMap<FString, zip_int64_t> EmbeddedFileToIndex;
		IFileHandle* FileHandle = nullptr;
		zip_source_t* ZipFileSource = nullptr;
		zip_t* ZipFile = nullptr;
		uint64 FilePos = 0;
		uint64 FileSize = 0;

		void Destruct();
		zip_int64_t ZipSourceFunctionReader(void* OutData, zip_uint64_t DataLen, zip_source_cmd_t Command);

		static zip_int64_t ZipSourceFunctionReaderStatic(void* InUserData, void* OutData, zip_uint64_t DataLen,
			zip_source_cmd_t Command);
	};

	FZipArchiveReader::FZipArchiveReader(IFileHandle* InFileHandle)
		: FileHandle(InFileHandle)
	{
		if (!FileHandle)
		{
			Destruct();
			return;
		}

		if (FileHandle->Tell() != 0)
		{
			FileHandle->Seek(0);
		}
		FilePos = 0;
		FileSize = FileHandle->Size();
		zip_error_t ZipError;
		zip_error_init(&ZipError);
		ZipFileSource = zip_source_function_create(ZipSourceFunctionReaderStatic, this, &ZipError);
		if (!ZipFileSource)
		{
			zip_error_fini(&ZipError);
			Destruct();
			return;
		}

		zip_error_init(&ZipError);
		ZipFile = zip_open_from_source(ZipFileSource, ZIP_RDONLY, &ZipError);
		if (!ZipFile)
		{
			zip_error_fini(&ZipError);
			Destruct();
			return;
		}

		zip_int64_t NumberOfFiles = zip_get_num_entries(ZipFile, 0);
		if (NumberOfFiles < 0 || MAX_int32 < NumberOfFiles)
		{
			Destruct();
			return;
		}
		EmbeddedFileToIndex.Reserve(NumberOfFiles);

		// produce the manifest file first in case the operation gets canceled while unzipping
		for (zip_int64_t i = 0; i < NumberOfFiles; i++)
		{
			zip_stat_t ZipFileStat;
			if (zip_stat_index(ZipFile, i, 0, &ZipFileStat) != 0)
			{
				Destruct();
				return;
			}
			zip_uint64_t ValidStat = ZipFileStat.valid;
			if (!(ValidStat & ZIP_STAT_NAME))
			{
				Destruct();
				return;
			}
			EmbeddedFileToIndex.Add(FString(ANSI_TO_TCHAR(ZipFileStat.name)), i);
		}
	}

	FZipArchiveReader::~FZipArchiveReader()
	{
		Destruct();
	}

	void FZipArchiveReader::Destruct()
	{
		EmbeddedFileToIndex.Empty();
		if (ZipFile)
		{
			zip_close(ZipFile);
			ZipFile = nullptr;
		}
		if (ZipFileSource)
		{
			zip_source_close(ZipFileSource);
			ZipFileSource = nullptr;
		}
		delete FileHandle;
		FileHandle = nullptr;
	}

	bool FZipArchiveReader::IsValid() const
	{
		return ZipFile != nullptr;
	}

	TArray<FString> FZipArchiveReader::GetFileNames() const
	{
		TArray<FString> Result;
		EmbeddedFileToIndex.GenerateKeyArray(Result);
		return Result;
	}

	bool FZipArchiveReader::TryReadFile(FStringView FileName, TArray<uint8>& OutData) const
	{
		OutData.Reset();

		const zip_int64_t* Index = EmbeddedFileToIndex.FindByHash(GetTypeHash(FileName), FileName);
		if (!Index)
		{
			return false;
		}

		zip_stat_t ZipFileStat;
		if (zip_stat_index(ZipFile, *Index, 0, &ZipFileStat) != 0)
		{
			return false;
		}

		if (!(ZipFileStat.valid & ZIP_STAT_SIZE))
		{
			return false;
		}

		if (ZipFileStat.size == 0)
		{
			return true;
		}
		if (ZipFileStat.size > MAX_int32)
		{
			return false;
		}

		OutData.SetNumUninitialized(ZipFileStat.size, EAllowShrinking::No);

		zip_file* EmbeddedFile = zip_fopen_index(ZipFile, *Index, 0 /* flags */);
		if (!EmbeddedFile)
		{
			OutData.Reset();
			return false;
		}
		bool bReadSuccess = zip_fread(EmbeddedFile, OutData.GetData(), ZipFileStat.size) == ZipFileStat.size;
		zip_fclose(EmbeddedFile);
		if (!bReadSuccess)
		{
			OutData.Reset();
			return false;
		}
		return true;
	}

	zip_int64_t FZipArchiveReader::ZipSourceFunctionReaderStatic(
		void* InUserData, void* OutData, zip_uint64_t DataLen, zip_source_cmd_t Command)
	{
		return reinterpret_cast<FZipArchiveReader*>(InUserData)->ZipSourceFunctionReader(OutData, DataLen, Command);
	}

	zip_int64_t FZipArchiveReader::ZipSourceFunctionReader(
		void* OutData, zip_uint64_t DataLen, zip_source_cmd_t Command)
	{
		switch (Command)
		{
			case ZIP_SOURCE_OPEN:
				return 0;
			case ZIP_SOURCE_READ:
				if (FilePos == FileSize)
				{
					return 0;
				}
				DataLen = FMath::Min(static_cast<zip_uint64_t>(FileSize - FilePos), DataLen);
				if (!FileHandle->Read(reinterpret_cast<uint8*>(OutData), DataLen))
				{
					return 0;
				}
				FilePos += DataLen;
				return DataLen;
			case ZIP_SOURCE_CLOSE:
				return 0;
			case ZIP_SOURCE_STAT:
			{
				zip_stat_t* OutStat = reinterpret_cast<zip_stat_t*>(OutData);
				zip_stat_init(OutStat);
				OutStat->size = FileSize;
				OutStat->comp_size = FileSize;
				OutStat->comp_method = ZIP_CM_STORE;
				OutStat->encryption_method = ZIP_EM_NONE;
				OutStat->valid = ZIP_STAT_SIZE | ZIP_STAT_COMP_SIZE | ZIP_STAT_COMP_METHOD | ZIP_STAT_ENCRYPTION_METHOD;
				return sizeof(*OutStat);
			}
			case ZIP_SOURCE_ERROR:
			{
				zip_uint32_t* OutLibZipError = reinterpret_cast<zip_uint32_t*>(OutData);
				zip_uint32_t* OutSystemError = OutLibZipError + 1;
				*OutLibZipError = ZIP_ER_INTERNAL;
				*OutSystemError = 0;
				return 2 * sizeof(*OutLibZipError);
			}
			case ZIP_SOURCE_FREE:
				return 0;
			case ZIP_SOURCE_SEEK:
			{
				zip_int64_t NewOffset = zip_source_seek_compute_offset(FilePos, FileSize, OutData, DataLen, nullptr);
				if (NewOffset < 0 || FileSize < static_cast<uint64>(NewOffset))
				{
					return -1;
				}

				if (!FileHandle->Seek(NewOffset))
				{
					return -1;
				}
				FilePos = NewOffset;
				return 0;
			}
			case ZIP_SOURCE_TELL:
				return static_cast<zip_int64_t>(FilePos);
			case ZIP_SOURCE_SUPPORTS:
				return zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_STAT,
					ZIP_SOURCE_ERROR, ZIP_SOURCE_FREE, ZIP_SOURCE_SEEK, ZIP_SOURCE_TELL, ZIP_SOURCE_SUPPORTS, -1);
			default:
				return 0;
		}
	}

	bool Unzip(const FString& Path, const FString& TargetPath, const TSharedPtr<SNotificationItem>& Notification)
	{
		IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

		IFileHandle* ArchiveFileHandle = FileManager.OpenRead(*Path);
		const FZipArchiveReader ZipArchiveReader(ArchiveFileHandle);
		if (!ZipArchiveReader.IsValid())
		{
			return false;
		}

		const TArray<FString> ArchiveFiles = ZipArchiveReader.GetFileNames();
		uint64 Size = ArchiveFiles.Num();
		uint64 Index = 0;
		for (const FString& FileName : ArchiveFiles)
		{
			Index++;
			if (Notification.IsValid())
			{
				Notification->SetText(FText::FromString(FString::Format(TEXT("Unzipping {0} / {1}"), { Index, Size })));
			}

			if (FileName.EndsWith("/") || FileName.EndsWith("\\"))
				continue;
			if (TArray<uint8> FileBuffer; ZipArchiveReader.TryReadFile(FileName, FileBuffer))
			{
				if (!FFileHelper::SaveArrayToFile(FileBuffer, *(TargetPath / FileName)))
				{
					return false;
				}
			}
		}
		return true;
	}
} // namespace
bool FMetaXRSimulator::IsSimulatorActivated()
{
	FString MetaXRSimPath = GetSimulatorJsonPath();
	FString CurRuntimePath = FWindowsPlatformMisc::GetEnvironmentVariable(*OpenXrRuntimeEnvKey);
	return (!MetaXRSimPath.IsEmpty() && MetaXRSimPath == CurRuntimePath);
}

void FMetaXRSimulator::ToggleOpenXRRuntime()
{
	OculusXRTelemetry::TScopedMarker<OculusXRTelemetry::Events::FSimulator> Event;
	FString MetaXRSimPath = GetSimulatorJsonPath();
	if (!IFileManager::Get().FileExists(*MetaXRSimPath))
	{
		InstallSimulator(ToggleOpenXRRuntime);
		UE_LOG(LogMetaXRSim, Log, TEXT("Meta XR Simulator Not Installed.\nInstalling Meta XR Simulator."));
		return;
	}

#if WITH_EDITOR
	if (OculusXR::IsOpenXRSystem())
	{
		FString ActivationText = IsSimulatorActivated() ? "deactivate" : "activate";
		FString Message = FString::Format(TEXT("A restart is required in order to {0} XR simulator. The restart must be performed from this dialog, opening and closing the editor manually will not work. Restart now?"), { ActivationText });
		if (FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(Message)) == EAppReturnType::Cancel)
		{
			UE_LOG(LogMetaXRSim, Log, TEXT("Meta XR Simulator %s action canceled."), *ActivationText);
			const auto& NotEnd = Event.SetResult(OculusXRTelemetry::EAction::Fail).AddAnnotation("reason", "restart canceled");
			return;
		}
	}
#endif // WITH_EDITOR

	if (IsSimulatorActivated())
	{
		// Deactivate MetaXR Simulator
		FString PrevOpenXrRuntimeEnvKey = FWindowsPlatformMisc::GetEnvironmentVariable(*PreviousOpenXrRuntimeEnvKey);

		FWindowsPlatformMisc::SetEnvironmentVar(*PreviousOpenXrRuntimeEnvKey,
			TEXT(""));
		FWindowsPlatformMisc::SetEnvironmentVar(*OpenXrRuntimeEnvKey, *PrevOpenXrRuntimeEnvKey);

		UE_LOG(LogMetaXRSim, Log, TEXT("Meta XR Simulator is deactivated. (%s : %s)"), *OpenXrRuntimeEnvKey, *PrevOpenXrRuntimeEnvKey);
		const auto& NotEnd = Event.AddAnnotation("action", "deactivated");
	}
	else
	{
		// Activate MetaXR Simulator
		FString CurOpenXrRuntimeEnvKey = FWindowsPlatformMisc::GetEnvironmentVariable(*OpenXrRuntimeEnvKey);

		FWindowsPlatformMisc::SetEnvironmentVar(*PreviousOpenXrRuntimeEnvKey,
			*CurOpenXrRuntimeEnvKey);
		FWindowsPlatformMisc::SetEnvironmentVar(*OpenXrRuntimeEnvKey, *MetaXRSimPath);

		UE_LOG(LogMetaXRSim, Log, TEXT("Meta XR Simulator is activated. (%s : %s)"), *OpenXrRuntimeEnvKey, *MetaXRSimPath);
		const auto& NotEnd = Event.AddAnnotation("action", "activated");
	}

#if WITH_EDITOR
	if (OculusXR::IsOpenXRSystem())
	{
		FUnrealEdMisc::Get().RestartEditor(false);
	}
#endif // WITH_EDITOR
}

FString FMetaXRSimulator::GetSimulatorJsonPath()
{
	return FPaths::Combine(GetPackagePath(), TEXT("meta_openxr_simulator.json"));
}

bool FMetaXRSimulator::IsSimulatorInstalled()
{
	return FPaths::FileExists(GetSimulatorJsonPath());
}

void FMetaXRSimulator::TryActivateOnStartup()
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS && WITH_EDITOR
	// If -HMDSimulator is used as the command option to launch UE, use simulator runtime instead of the physical HMD runtime (like PC-Link).
	if (FParse::Param(FCommandLine::Get(), TEXT("HMDSimulator")))
	{
		if (IsSimulatorActivated())
		{
			return;
		}

		ToggleOpenXRRuntime();
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS && WITH_EDITOR
}

FString FMetaXRSimulator::GetPackagePath()
{
	return FPaths::Combine(FPlatformMisc::GetEnvironmentVariable(TEXT("LOCALAPPDATA")), TEXT("MetaXR"), TEXT("MetaXRSimulator"), GetVersion());
}

void FMetaXRSimulator::InstallSimulator(const TFunction<void()>& OnSuccess)
{
	FNotificationInfo Progress(FText::FromString("Installing Meta XR Simulator..."));
	Progress.bFireAndForget = false;
	Progress.FadeInDuration = 0.5f;
	Progress.FadeOutDuration = 0.5f;
	Progress.ExpireDuration = 5.0f;
	Progress.bUseThrobber = true;
	Progress.bUseSuccessFailIcons = true;

	TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Progress);
	if (NotificationItem.IsValid())
	{
		NotificationItem->SetCompletionState(SNotificationItem::CS_Pending);
	}

	auto DestinationFolder = GetPackagePath();
	auto DownloadPath = FPaths::Combine(FPaths::EngineSavedDir(), TEXT("Downloads"), TEXT("MetaXRSimulator"), GetVersion(), TEXT("MetaXRSimulator.zip"));

	if (FPaths::FileExists(DownloadPath))
	{
		UnzipSimulator(DownloadPath, DestinationFolder, NotificationItem, OnSuccess);
		return;
	}

	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->OnProcessRequestComplete().BindLambda([DownloadPath, DestinationFolder, NotificationItem, OnSuccess](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		Request->OnRequestProgress64().Unbind();
		if (Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			// Save the downloaded zip file
			FFileHelper::SaveArrayToFile(Response->GetContent(), *DownloadPath);
			if (NotificationItem.IsValid())
			{
				NotificationItem->SetText(FText::FromString("Unzipping ... "));
			}

			UnzipSimulator(DownloadPath, DestinationFolder, NotificationItem, OnSuccess);
			return;
		}

		UE_LOG(LogMetaXRSim, Error, TEXT("Failed to install Meta XR Simulator."));
		if (NotificationItem.IsValid())
		{
			NotificationItem->SetText(FText::FromString("Installation failed!"));
			NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);
			NotificationItem->ExpireAndFadeout();
		}
	});

	Request->OnRequestProgress64().BindLambda([NotificationItem](const FHttpRequestPtr& Request, uint64 /* BytesSent */, uint64 BytesReceived) {
		uint64 ContentLength = Request->GetResponse()->GetContentLength();
		if (NotificationItem.IsValid())
		{
			NotificationItem->SetText(FText::FromString(FString::Format(TEXT("Downloading {0} / {1}"), { BytesReceived, ContentLength })));
		}
	});

	Request->SetURL("https://www.facebook.com/horizon_devcenter_download?app_id=28549923061320041&sdk_version=" + GetVersion());
	Request->SetVerb(TEXT("GET"));
	Request->ProcessRequest();
}

FString FMetaXRSimulator::GetVersion()
{
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("OculusXR"));
	if (Plugin.IsValid())
	{
		FString VersionName = Plugin->GetDescriptor().VersionName;
		TArray<FString> ParsedParts;
		VersionName.ParseIntoArray(ParsedParts, TEXT("."), true);
		return FString::FromInt(FCString::Atoi(*ParsedParts[1]) - 32);
	}
	return "0";
}

void FMetaXRSimulator::UnzipSimulator(const FString& Path, const FString& TargetPath, const TSharedPtr<SNotificationItem>& Notification,
	const TFunction<void()>& OnSuccess)
{
	bool bSuccess = Unzip(Path, TargetPath, Notification);

	if (!bSuccess || !IsSimulatorInstalled())
	{
		UE_LOG(LogMetaXRSim, Error, TEXT("Failed to unzip the file."));
		if (Notification.IsValid())
		{
			Notification->SetText(FText::FromString("Installation failed!"));
			Notification->SetCompletionState(SNotificationItem::CS_Fail);
			Notification->ExpireAndFadeout();
		}
		return;
	}
	if (Notification.IsValid())
	{
		Notification->SetText(FText::FromString("Installation succeeded!"));
		Notification->SetCompletionState(SNotificationItem::CS_Success);
		Notification->ExpireAndFadeout();
	}

	if (OnSuccess)
	{
		OnSuccess();
	}
}
#endif // PLATFORM_WINDOWS
