// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRPrivacyNotification.h"

#include <regex>

#include "ISettingsModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "OculusXRHMDModule.h"
#include "OculusXRTelemetry.h"
#include "OculusXRToolStyle.h"
#include "Interfaces/IMainFrameModule.h"
#include "Widgets/Notifications/INotificationWidget.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Images/SImage.h"
#include "Framework/Application/SlateApplication.h"
#include "OculusXRTelemetryPrivacySettings.h"

#define LOCTEXT_NAMESPACE "OculusXRTelemetryPrivacySettings"

namespace OculusXRTelemetry
{
	namespace
	{
		constexpr int CONSENT_TITLE_MAX_LENGTH = 256;
		constexpr int CONSENT_TEXT_MAX_LENGTH = 2048;
		constexpr int CONSENT_NOTIFICATION_MAX_LENGTH = 1024;

		void OnBrowserLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata)
		{
			const FString* Url = Metadata.Find(TEXT("href"));
			if (Url)
			{
				FPlatformProcess::LaunchURL(**Url, nullptr, nullptr);
			}
		}

		void UpdateNotificationShown()
		{
			FOculusXRHMDModule::GetPluginWrapper().SetNotificationShown(UNREAL_TOOL_ID);
		}

		std::string MarkdownToRTF(const std::string& markdown)
		{
			const std::regex boldText(R"(\*\*(.*?)\*\*)");
			const std::string boldReplacement = "<NormalText.Important>$1</>";

			std::string rtf = "";
			rtf = std::regex_replace(markdown, boldText, boldReplacement);

			const std::regex linkRegex(R"(\[(.*?)\]\((.*?)\))");
			const std::string linkReplacement = "<a id=\"browser\" style=\"Hyperlink\" href=\"$2\">$1</>";
			rtf = std::regex_replace(rtf, linkRegex, linkReplacement);
			return rtf;
		}
	} // namespace

	class SOculusXRPrivacyNotification : public SCompoundWidget, public INotificationWidget
	{
	public:
		SLATE_BEGIN_ARGS(SOculusXRPrivacyNotification) {}
		SLATE_ARGUMENT(std::string, ConsentText);
		/** Invoked when any button is clicked, needed for fading out notification */
		SLATE_EVENT(FSimpleDelegate, OnClicked)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs)
		{
			OnClicked = InArgs._OnClicked;

			// Container for the text and optional interactive widgets (buttons, check box, and hyperlink)
			TSharedRef<SVerticalBox> InteractiveWidgetsBox = SNew(SVerticalBox);

			InteractiveWidgetsBox->AddSlot()
				.Padding(FMargin(0.0f, 10.0f, 0.0f, 2.0f))
				.AutoHeight()
					[SNew(SRichTextBlock)
							.Text(FText::FromString(MarkdownToRTF(InArgs._ConsentText).data()))
							.AutoWrapText(true)
						+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateStatic(&OnBrowserLinkClicked))
						+ SRichTextBlock::HyperlinkDecorator(TEXT("PrivacySettings"), FSlateHyperlinkRun::FOnClick::CreateRaw(this, &SOculusXRPrivacyNotification::OpenPrivacySettings))];

			ChildSlot
				[SNew(SBox)
						[SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
								  .AutoWidth()
								  .VAlign(VAlign_Top)
								  .HAlign(HAlign_Left)
									  [SNew(SOverlay)
										  + SOverlay::Slot()
												.VAlign(VAlign_Center)
												.HAlign(HAlign_Center)
													[SNew(SImage)
															.Image(FOculusToolStyle::Get().GetBrush("OculusTool.MetaLogo"))
															.DesiredSizeOverride(FVector2D(32, 32))]]
							+ SHorizontalBox::Slot()
								  .Padding(10.f, 0.f, 5.f, 0.f)
									  [InteractiveWidgetsBox]
							+ SHorizontalBox::Slot()
								  .AutoWidth()
								  .VAlign(VAlign_Top)
								  .HAlign(HAlign_Right)
									  [SNew(SButton)
											  .Cursor(EMouseCursor::Default)
											  .ButtonStyle(FAppStyle::Get(), "SimpleButton")
											  .ContentPadding(0.0f)
											  .OnClicked(this, &SOculusXRPrivacyNotification::CloseButtonClicked)
											  .Content()
												  [SNew(SImage)
														  .Image(FAppStyle::GetBrush("Icons.X"))
														  .ColorAndOpacity(FSlateColor::UseForeground())]]]];
		}

	private:
		virtual TSharedRef<SWidget> AsWidget() override
		{
			return AsShared();
		}

		virtual void OnSetCompletionState(SNotificationItem::ECompletionState InState) override
		{
			if (InState == SNotificationItem::ECompletionState::CS_Success)
			{
				UpdateNotificationShown();
			}
		}

		FReply CloseButtonClicked()
		{
			OnClicked.ExecuteIfBound();
			return FReply::Handled();
		}

		void OpenPrivacySettings(const FSlateHyperlinkRun::FMetadata& /*Metadata*/) const
		{
			OnClicked.ExecuteIfBound();
			FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer(FName("Editor"), FName("Privacy"), FName("OculusXR"));
		}

		FSimpleDelegate OnClicked;
	};

	class SOculusTelemetryWindow : public SCompoundWidget
	{
		SLATE_BEGIN_ARGS(SOculusTelemetryWindow) {}
		SLATE_ARGUMENT(std::string, ConsentText);
		SLATE_END_ARGS()

		/** Construct the slate layout for the widget */
		void Construct(const FArguments& InArgs)
		{
			TSharedPtr<SVerticalBox> RootContainer = SNew(SVerticalBox);
			const TSharedPtr<SHorizontalBox> ButtonsWidget = SNew(SHorizontalBox);

			ButtonsWidget->AddSlot()
				[SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
						[SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.ContentPadding(5)
								.OnClicked(this, &SOculusTelemetryWindow::OnNotShareClicked)
								.Text(LOCTEXT("NotShare", "Only share essential data"))]];

			ButtonsWidget->AddSlot()
				[SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
						[SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.ContentPadding(5)
								.OnClicked(this, &SOculusTelemetryWindow::OnShareClicked)
								.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"))
								.Text(LOCTEXT("Share", "Share additional data"))]];

			// Construct the text widget
			const TSharedPtr<SVerticalBox> TextWidget = SNew(SVerticalBox);

			TextWidget->AddSlot()
				.AutoHeight()
					[SNew(SRichTextBlock)
							.AutoWrapText(true)
							.Text(FText::FromString(MarkdownToRTF(InArgs._ConsentText).data()))
							.DecoratorStyleSet(&FAppStyle::Get())
						+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateStatic(&OnBrowserLinkClicked))];

			RootContainer->AddSlot()
				[SNew(SVerticalBox)
					+ SVerticalBox::Slot()
						  .Padding(20, 20, 20, 20)
						  .VAlign(VAlign_Center)
							  [TextWidget.ToSharedRef()]];

			RootContainer->AddSlot()
				[SNew(SVerticalBox)
					+ SVerticalBox::Slot()
						  .Padding(20, 20)
						  .VAlign(VAlign_Bottom)
							  [ButtonsWidget.ToSharedRef()]];
			ChildSlot
				[SNew(SBox)
						.WidthOverride(960)
							[RootContainer.ToSharedRef()]];
		}

	private:
		FReply OnShareClicked()
		{
			FOculusXRHMDModule::GetPluginWrapper().SaveUnifiedConsent(UNREAL_TOOL_ID, ovrpBool_True);
			if (UOculusXRTelemetryPrivacySettings* EditorPrivacySettings = GetMutableDefault<UOculusXRTelemetryPrivacySettings>())
			{
				EditorPrivacySettings->bIsEnabled = true;
			}
			PropagateTelemetryConsent();
			FSlateApplication::Get().FindWidgetWindow(AsShared())->RequestDestroyWindow();
			return FReply::Handled();
		}

		FReply OnNotShareClicked()
		{
			FOculusXRHMDModule::GetPluginWrapper().SaveUnifiedConsent(UNREAL_TOOL_ID, ovrpBool_False);
			if (UOculusXRTelemetryPrivacySettings* EditorPrivacySettings = GetMutableDefault<UOculusXRTelemetryPrivacySettings>())
			{
				EditorPrivacySettings->bIsEnabled = false;
			}
			PropagateTelemetryConsent();
			FSlateApplication::Get().FindWidgetWindow(AsShared())->RequestDestroyWindow();
			return FReply::Handled();
		}
	};

	void SpawnFullConsentWindow()
	{
		if (FSlateApplication::Get().IsRenderingOffScreen())
		{
			return;
		}

		char TelemetryWindowTitle[CONSENT_TITLE_MAX_LENGTH];
		char ConsentText[CONSENT_TEXT_MAX_LENGTH];
		if (FOculusXRHMDModule::GetPluginWrapper().GetConsentTitle(TelemetryWindowTitle) == ovrpFailure || FOculusXRHMDModule::GetPluginWrapper().GetConsentMarkdownText(ConsentText) == ovrpFailure)
		{
			return;
		}

		std::string Title(TelemetryWindowTitle);
		std::string MarkdownText(ConsentText);

		IMainFrameModule::Get().OnMainFrameCreationFinished().AddLambda([Title, MarkdownText](const TSharedPtr<SWindow>& RootWindow, bool /*bIsRunningStartupDialog*/) {
			const TSharedRef<SWindow> Window = SNew(SWindow)
												   .Title(FText::FromString(Title.c_str()))
												   .SizingRule(ESizingRule::Autosized)
												   .SupportsMaximize(false)
												   .SupportsMinimize(false)[SNew(SOculusTelemetryWindow).ConsentText(MarkdownText)];

			FSlateApplication::Get().AddModalWindow(Window, RootWindow);
		});
	}

	void SpawnNotification()
	{
		char NotificationText[CONSENT_NOTIFICATION_MAX_LENGTH];
		if (FOculusXRHMDModule::GetPluginWrapper().GetConsentNotificationMarkdownText("<a id=\"PrivacySettings\">Settings</>", NotificationText) == ovrpFailure)
		{
			return;
		}
		TPromise<TSharedPtr<SNotificationItem>> BtnNotificationPromise;
		const auto OnClicked = [NotificationFuture = BtnNotificationPromise.GetFuture().Share()]() {
			const TSharedPtr<SNotificationItem> Notification = NotificationFuture.Get();
			Notification->SetCompletionState(SNotificationItem::CS_Success);
			Notification->Fadeout();
		};

		FNotificationInfo Info(SNew(SOculusXRPrivacyNotification).OnClicked_Lambda(OnClicked).ConsentText(NotificationText));
		Info.ExpireDuration = 60.0f;

		const TSharedPtr<SNotificationItem> PrivacyNotification = FSlateNotificationManager::Get().AddNotification(Info);
		if (PrivacyNotification.IsValid())
		{
			PrivacyNotification->SetCompletionState(SNotificationItem::CS_Pending);
			BtnNotificationPromise.SetValue(PrivacyNotification);
		}
	}

	void MaybeSpawnTelemetryConsent()
	{
		if (!FOculusXRHMDModule::Get().IsOVRPluginAvailable() || !FOculusXRHMDModule::GetPluginWrapper().IsInitialized())
		{
			return;
		}

		if (!FModuleManager::Get().IsModuleLoaded("MainFrame"))
		{
			return;
		}

		const auto& PluginWrapper = FOculusXRHMDModule::GetPluginWrapper();

		if (PluginWrapper.ShouldShowTelemetryConsentWindow(UNREAL_TOOL_ID))
		{
			SpawnFullConsentWindow();
		}

		if (PluginWrapper.ShouldShowTelemetryNotification(UNREAL_TOOL_ID))
		{
			SpawnNotification();
		}
	}
} // namespace OculusXRTelemetry

#undef LOCTEXT_NAMESPACE
