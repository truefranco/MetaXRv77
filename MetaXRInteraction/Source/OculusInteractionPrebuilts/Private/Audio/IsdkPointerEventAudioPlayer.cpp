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

#include "Audio/IsdkPointerEventAudioPlayer.h"

#include "Components/AudioComponent.h"

// Sets default values for this component's properties
UIsdkPointerEventAudioPlayer::UIsdkPointerEventAudioPlayer()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UIsdkPointerEventAudioPlayer::SetPointable(
    const TScriptInterface<IIsdkIPointable>& InPointable)
{
  if (IsValid(Pointable.GetObject()))
  {
    Pointable.GetInterface()->GetInteractionPointerEventDelegate().RemoveDynamic(
        this, &UIsdkPointerEventAudioPlayer::HandlePointerEvent);
  }
  Pointable = InPointable;
  if (IsValid(Pointable.GetObject()))
  {
    Pointable.GetInterface()->GetInteractionPointerEventDelegate().AddUniqueDynamic(
        this, &UIsdkPointerEventAudioPlayer::HandlePointerEvent);
  }
}

void UIsdkPointerEventAudioPlayer::BeginPlay()
{
  Super::BeginPlay();
  if (IsValid(Pointable.GetObject()))
  {
    Pointable.GetInterface()->GetInteractionPointerEventDelegate().AddUniqueDynamic(
        this, &UIsdkPointerEventAudioPlayer::HandlePointerEvent);
  }
}

void UIsdkPointerEventAudioPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
  if (IsValid(Pointable.GetObject()))
  {
    Pointable.GetInterface()->GetInteractionPointerEventDelegate().RemoveDynamic(
        this, &UIsdkPointerEventAudioPlayer::HandlePointerEvent);
  }
}

void UIsdkPointerEventAudioPlayer::HandlePointerEvent(
    const FIsdkInteractionPointerEvent& PointerEvent)
{
  UAudioComponent* AudioToPlay = nullptr;
  switch (PointerEvent.Type)
  {
    case EIsdkPointerEventType::Hover:
      AudioToPlay = HoverAudio;
      break;
    case EIsdkPointerEventType::Unhover:
      AudioToPlay = UnhoverAudio;
      break;
    case EIsdkPointerEventType::Select:
      AudioToPlay = SelectAudio;
      break;
    case EIsdkPointerEventType::Unselect:
      AudioToPlay = UnselectAudio;
      break;
    case EIsdkPointerEventType::Move:
      AudioToPlay = MoveAudio;
      break;
    case EIsdkPointerEventType::Cancel:
      AudioToPlay = CancelAudio;
      break;
  }

  if (IsValid(AudioToPlay))
  {
    AudioToPlay->Play();
  }
}
