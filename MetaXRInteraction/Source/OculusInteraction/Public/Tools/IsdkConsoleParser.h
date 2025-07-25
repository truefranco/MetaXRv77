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

#pragma once

#include "CoreMinimal.h"
#include "Interaction/IsdkGrabbableComponent.h"
#include "Interaction/IsdkGrabberComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DefaultValueHelper.h"
#include "IsdkConsoleParser.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnConsoleCommand, FString, bool*);

UINTERFACE(MinimalAPI, Blueprintable)
class UIsdkConsoleReceiver : public UInterface
{
  GENERATED_BODY()
};

class IIsdkConsoleReceiver
{
  GENERATED_BODY()
 public:
  virtual bool ProcessConsoleCommand(const TArray<FString>& TextArgs, UWorld* World) = 0;
};

UCLASS(Blueprintable, ClassGroup = (InteractionSDK), meta = (DisplayName = "ISDK Console Parser"))
class OCULUSINTERACTION_API UIsdkConsoleParser : public UObject
{
  GENERATED_BODY()

 public:
  UIsdkConsoleParser() {}
  virtual ~UIsdkConsoleParser() override {}

  static constexpr auto* IsdkWakeWord = TEXT("isdk");
  inline static TMap<FString, FString> Values;
  inline static TArray<IIsdkConsoleReceiver*> Receivers;
  inline static UIsdkConsoleParser* ConsoleParser = nullptr;
  inline static FConsoleCommandWithWorldAndArgsDelegate Delegate{};

  static void EnsureConsoleParserCreated()
  {
    if (!ConsoleParser)
    {
      Delegate.BindStatic(ParseConsoleCommand);
      ConsoleParser = NewObject<UIsdkConsoleParser>(UIsdkConsoleParser::StaticClass());
      IConsoleManager::Get().RegisterConsoleCommand(
          IsdkWakeWord, TEXT("Interaction SDK commands"), Delegate, ECVF_Default);
    }
  }

 public:
  static void Init()
  {
    EnsureConsoleParserCreated(); // this will handle all the initialization as well
  }

  UFUNCTION(BlueprintCallable, Category = InteractionSDK)
  static UIsdkConsoleParser* Get()
  {
    EnsureConsoleParserCreated();
    return ConsoleParser;
  }

  static void RegisterListener(TScriptInterface<IIsdkConsoleReceiver> Receiver)
  {
    // just to insure we don't call the same receiver twice if it registers multiple times
    UnregisterListener(Receiver);

    Get()->Receivers.Add(Receiver.GetInterface());
  }

  static void UnregisterListener(const TScriptInterface<IIsdkConsoleReceiver> Receiver)
  {
    Get()->Receivers.Remove(Receiver.GetInterface());
  }

  static FString GetFStringValue(const FString& Keyword)
  {
    const auto& V = Get()->Values;
    if (Values.Contains(Keyword))
    {
      return Values[Keyword].ToLower();
    }
    return "";
  }

  static bool GetBoolValue(const FString& Keyword)
  {
    const auto s = GetFStringValue(Keyword);
    return s == "true" || s == "1" || s == "on";
  }

  template <typename T>
  static T GetNumericValue(const FString& Keyword, bool (*func)(const FString&, T&))
  {
    const auto& V = Get()->Values;
    if (V.Contains(Keyword))
    {
      if (T Outval{0}; func(V[Keyword], Outval))
      {
        return Outval;
      }
    }
    return 0;
  }

  static float GetFloatValue(const FString& Keyword)
  {
    return GetNumericValue<float>(Keyword, &FDefaultValueHelper::ParseFloat);
  }

  static int GetIntValue(const FString& Keyword)
  {
    return GetNumericValue<int>(Keyword, &FDefaultValueHelper::ParseInt);
  }

  static int64 GetInt64Value(const FString& Keyword)
  {
    return GetNumericValue<int64>(Keyword, &FDefaultValueHelper::ParseInt64);
  }

  static double GetDoubleValue(const FString& Keyword)
  {
    return GetNumericValue<double>(Keyword, &FDefaultValueHelper::ParseDouble);
  }

  static void ParseConsoleCommand(const TArray<FString>& TextArgs, UWorld* World)
  {
    if (!TextArgs.IsEmpty())
    {
      FString IsdkCommand = TextArgs[0].ToLower();
      FString param = TextArgs.Num() > 1 ? TextArgs[1] : "";
      Get()->Values.Emplace(IsdkCommand, param);
      for (const auto Receiver : Get()->Receivers)
      {
        Receiver->ProcessConsoleCommand(TextArgs, World);
      }
    }
  }
};
