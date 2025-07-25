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
#include "UObject/Interface.h"
#include "IsdkIRootPoseLocker.generated.h"

/**
 * Fixes the location and/or rotation of the device's "root pose", forcing those to be used in place
 * of its own tracking data.
 *
 * Lerp factors can be provided, so that the device can be instructed to lerp between its current
 * tracked data, and the provided locked transform.
 */
UINTERFACE(Blueprintable, Category = InteractionSDK)
class OCULUSINTERACTION_API UIsdkIRootPoseLocker : public UInterface
{
  GENERATED_BODY()
};

UENUM(BlueprintType, Category = InteractionSDK)
enum class EIsdkRootPoseLockMode : uint8
{
  None = 0,
  Position = 1 << 0,
  Rotation = 1 << 1,
  Full = Position | Rotation
};

class OCULUSINTERACTION_API IIsdkIRootPoseLocker
{
  GENERATED_BODY()

 public:
  static bool IsFlagSet(EIsdkRootPoseLockMode StatusFlags, EIsdkRootPoseLockMode FlagToLookFor)
  {
    return (static_cast<int>(FlagToLookFor) & static_cast<int>(StatusFlags)) != 0;
  }

  /// Fixes the location and/or rotation of the device's "root pose".
  /// @param Transform Locks both Location and Rotation, relative to tracking space
  /// @param OverrideFactor Lerp factor, between 0.0 (unlocked value) and 1.0 (provided, locked,
  /// value.
  /// @param LockMode Specify which of Location, Rotation to lock. Modes not present in this bitmask
  /// will not be freed - FreeRootTransform must be explicitly called to unlock.
  /// @param bSkipAnimation When false, the implementation will animate from the previous state to
  /// the new state.
  UFUNCTION(BlueprintNativeEvent, Category = InteractionSDK)
  void LockRootTransform(
      const FTransform& Transform,
      const float OverrideFactor = 1.f,
      EIsdkRootPoseLockMode LockMode = EIsdkRootPoseLockMode::Full,
      bool bSkipAnimation = false);
  virtual void LockRootTransform_Implementation(
      const FTransform& Transform,
      const float OverrideFactor = 1.f,
      EIsdkRootPoseLockMode LockMode = EIsdkRootPoseLockMode::Full,
      bool bSkipAnimation = false) PURE_VIRTUAL(IIsdkIRootPoseLocker::LockRootTransform);

  /// Fixes the location of the device's "root pose".
  /// @param Position relative to tracking space.
  /// @param OverrideFactor Lerp factor, between 0.0 (unlocked value) and 1.0 (provided, locked,
  /// value.
  /// @param bSkipAnimation When false, the implementation will animate from the previous state to
  /// the new state.
  UFUNCTION(BlueprintNativeEvent, Category = InteractionSDK)
  void LockRootLocation(
      const FVector& Position,
      const float OverrideFactor = 1.f,
      bool bSkipAnimation = false);
  virtual void LockRootLocation_Implementation(
      const FVector& Position,
      const float OverrideFactor = 1.f,
      bool bSkipAnimation = false) PURE_VIRTUAL(IIsdkIRootPoseLocker::LockRootLocation);

  /// Fixes the roation of the device's "root pose".
  /// @param Rotation relative to tracking space.
  /// @param OverrideFactor Lerp factor, between 0.0 (unlocked value) and 1.0 (provided, locked,
  /// value.
  /// @param bSkipAnimation When false, the implementation will animate from the previous state to
  /// the new state.
  UFUNCTION(BlueprintNativeEvent, Category = InteractionSDK)
  void LockRootRotation(
      const FQuat& Rotation,
      const float OverrideFactor = 1.f,
      bool bSkipAnimation = false);
  virtual void LockRootRotation_Implementation(
      const FQuat& Rotation,
      const float OverrideFactor = 1.f,
      bool bSkipAnimation = false) PURE_VIRTUAL(IIsdkIRootPoseLocker::LockRootRotation);

  UFUNCTION(BlueprintNativeEvent, Category = InteractionSDK)
  void FreeRootTransform(EIsdkRootPoseLockMode LockMode = EIsdkRootPoseLockMode::Full);
  virtual void FreeRootTransform_Implementation(
      EIsdkRootPoseLockMode LockMode = EIsdkRootPoseLockMode::Full)
      PURE_VIRTUAL(IIsdkIRootPoseLocker::FreeRootTransform);
};
