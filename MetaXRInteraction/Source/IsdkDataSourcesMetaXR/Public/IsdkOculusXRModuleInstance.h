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

#include "StructTypes.h"
#include "UObject/Object.h"

/**
 * This module calls UFunctions from UOculusXRFunctionLibrary and UOculusXRInputFunctionLibrary,
 * without needing explicit type references. This wrapper layer exists so that the functions
 * can be resolved at module load time (using Unreal's reflection) rather than at build time - which
 * allows this plugin to exist without build-time module references to OculusXR modules.
 */
class ISDKDATASOURCESMETAXR_API FIsdkOculusXRModuleInstance
{
 public:
  FIsdkOculusXRModuleInstance();

  bool IsDeviceTracked_Hmd() const;
  void GetPose(FRotator& Rotator, FVector& Vector, FVector& NeckPosition) const;

  bool Input_IsPointerPoseValid(EIsdkHandedness Handedness) const;
  FTransform Input_GetPointerPose(EIsdkHandedness Handedness) const;
  bool Input_IsTrackingHighConfidence(EIsdkHandedness Handedness) const;
  bool Input_IsHandPositionValid(EIsdkHandedness Handedness) const;
  FQuat Input_GetBoneRotation(EIsdkHandedness Handedness, uint8 OvrBoneIndex) const;
  bool Input_IsHandTrackingEnabled() const;
  EIsdkXRControllerDrivenHandPoseType Input_GetControllerDrivenHandPoses() const;
  void Input_SetControllerDrivenHandPoses(EIsdkXRControllerDrivenHandPoseType Type) const;
  EIsdkXRControllerType GetControllerType(EIsdkHandedness Handedness) const;

 private:
  static void SetHandednessProperty(FProperty* Prop, uint8* Parms, EIsdkHandedness Handedness)
  {
    const FEnumProperty* EnumProp = CastFieldChecked<FEnumProperty>(Prop);
    const FNumericProperty* UnderlyingProp = EnumProp->GetUnderlyingProperty();
    // EOculusXRHandType::HandLeft = 1    (IsdkHandedness::Left = 0)
    // EOculusXRHandType::HandRight = 2   (IsdkHandedness::Right = 1)
    UnderlyingProp->SetIntPropertyValue(
        EnumProp->ContainerPtrToValuePtr<uint8>(Parms), static_cast<int64>(Handedness) + 1);
  }
  static void SetControllerDrivenHandTypeProperty(
      FProperty* Prop,
      uint8* Parms,
      EIsdkXRControllerDrivenHandPoseType PoseType)
  {
    const FEnumProperty* EnumProp = CastFieldChecked<FEnumProperty>(Prop);
    const FNumericProperty* UnderlyingProp = EnumProp->GetUnderlyingProperty();
    // None = 0,
    // Natural = 1,
    // Controller = 2,
    UnderlyingProp->SetIntPropertyValue(
        EnumProp->ContainerPtrToValuePtr<uint8>(Parms), static_cast<int64>(PoseType));
  }
  struct OculusXRInput_DeviceHandControllerIndex_BaseFn_
  {
    UFunction* Function{};
    FEnumProperty* Arg_DeviceHand;
    FNumericProperty* Arg_ControllerIndex;
  };
  void CallDeviceHandFunction(
      const OculusXRInput_DeviceHandControllerIndex_BaseFn_& FunctionDesc,
      EIsdkHandedness Handedness,
      uint8* ReturnValueAddress,
      size_t ReturnValueSize) const;

  template <class TProperty, typename TReturnType>
  static TProperty* CheckFunctionReturnValue(const UFunction* Function)
  {
    TProperty* Prop = CastFieldChecked<TProperty>(Function->GetReturnProperty());
    checkf(
        (Prop->PropertyFlags & CPF_ReturnParm) == CPF_ReturnParm,
        TEXT("%s return value - expected to be a param, and not a return value"),
        *Function->GetName());

    checkf(
        Prop->GetSize() == sizeof(TReturnType),
        TEXT("%s return value - expected return value size to be %d, was %d"),
        *Function->GetName(),
        sizeof(TReturnType),
        sizeof(Prop->GetSize()));

    return Prop;
  }

  static void CheckFunctionNoReturnValue(const UFunction* Function)
  {
    checkf(
        Function->GetReturnProperty() == nullptr,
        TEXT("%s expected to have no return value, but had one."),
        *Function->GetName());
  }

  template <class TProperty, typename TReturnType>
  static TProperty*
  GetFunctionChildPropChecked(const UFunction* Function, FName Name, bool bIsOutParam = false)
  {
    TProperty* Prop = CastFieldChecked<TProperty>(Function->FindPropertyByName(Name));
    checkf(
        (Prop->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm,
        TEXT("%s arg %s - expected to be a param, and not a return value"),
        *Function->GetName(),
        *Name.ToString());
    if (bIsOutParam)
    {
      checkf(
          (Prop->PropertyFlags & CPF_OutParm) == CPF_OutParm,
          TEXT("%s arg %s - expected to be an out param"),
          *Function->GetName(),
          *Name.ToString());
    }

    checkf(
        Prop->GetSize() == sizeof(TReturnType),
        TEXT("%s arg %s - expected return value size to be %d, was %d"),
        *Function->GetName(),
        *Name.ToString(),
        sizeof(TReturnType),
        sizeof(Prop->GetSize()));
    return Prop;
  }

  UClass* OculusXR_Class{};

  UClass* OculusXRInput_Class{};
  struct OculusXRInput_IsPointerPoseValid_Fn_ : OculusXRInput_DeviceHandControllerIndex_BaseFn_
  {
    FBoolProperty* ReturnValue;
  } OculusXRInput_IsPointerPoseValid_Fn;
  struct OculusXRInput_GetPointerPose_Fn_ : OculusXRInput_DeviceHandControllerIndex_BaseFn_
  {
    FStructProperty* ReturnValue;
  } OculusXRInput_GetPointerPose_Fn;
  struct OculusXRInput_IsHandPositionValid_Fn_ : OculusXRInput_DeviceHandControllerIndex_BaseFn_
  {
    FBoolProperty* ReturnValue;
  } OculusXRInput_IsHandPositionValid_Fn;
  struct OculusXRInput_GetTrackingConfidence_Fn_ : OculusXRInput_DeviceHandControllerIndex_BaseFn_
  {
    FEnumProperty* ReturnValue;
  } OculusXRInput_GetTrackingConfidence_Fn;
  struct OculusXRInput_GetBoneRotation_Fn_ : OculusXRInput_DeviceHandControllerIndex_BaseFn_
  {
    FEnumProperty* Arg_BoneId;
    FStructProperty* ReturnValue;
  } OculusXRInput_GetBoneRotation_Fn;
  struct OculusXRInput_IsHandTrackingEnabled_Fn_
  {
    UFunction* Function{};
    FBoolProperty* ReturnValue;
  } OculusXRInput_IsHandTrackingEnabled_Fn;
  struct OculusXRInput_GetControllerDrivenHandPoses_Fn_
  {
    UFunction* Function;
    FEnumProperty* ReturnValue;
  } OculusXRInput_GetControllerDrivenHandPoses_Fn;
  struct OculusXRInput_SetControllerDrivenHandPoses_Fn_
  {
    UFunction* Function{};
    FEnumProperty* Arg_Type;
  } OculusXRInput_SetControllerDrivenHandPoses_Fn;

  struct OculusXR_IsDeviceTracked_Fn_
  {
    UFunction* Function{};
    FEnumProperty* Arg_DeviceType;
    FBoolProperty* ReturnValue;
  } OculusXR_IsDeviceTracked_Fn;
  struct OculusXR_GetPose_Fn_
  {
    UFunction* Function{};
    FStructProperty* Arg_DeviceRotation;
    FStructProperty* Arg_DevicePosition;
    FStructProperty* Arg_NeckPosition;
    FBoolProperty* Arg_UseOrienationForPlayerCamera;
    FBoolProperty* Arg_UsePositionForPlayerCamera;
    FStructProperty* Arg_PositionScale;
  } OculusXR_GetPose_Fn;
  struct OculusXR_GetControllerType_Fn_
  {
    UFunction* Function;
    FEnumProperty* Arg_Hand;
    FEnumProperty* ReturnValue;
  } OculusXR_GetControllerType_Fn;

  const uint32 ExportFlags = PPF_None;
};
