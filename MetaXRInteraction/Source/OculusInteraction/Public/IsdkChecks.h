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
#include "Interaction/Surfaces/IsdkPointableBox.h"
#include "isdk_api/isdk_api.h"
#include "OculusInteractionLog.h"
#include "GameFramework/Actor.h"

#include "IsdkChecks.generated.h"

UCLASS(Abstract)
class OCULUSINTERACTION_API UIsdkChecks : public UObject
{
  GENERATED_BODY()

 public:
  static bool EnsureMsgfApiInstanceIsValid(
      bool IsValid,
      const UObject* OnObject,
      const TCHAR* InstanceName = TEXT("Instance"))
  {
    return ensureMsgf(
        IsValid,
        TEXT("%s: Failed to create internal %s state"),
        *OnObject->GetFullName(),
        InstanceName);
  }

  static bool EnsureMsgfDependencyIsValid(
      const UObject* Dependency,
      const UObject* OnObject,
      const TCHAR* DependencyFieldName)
  {
    return ensureMsgf(
        IsValid(Dependency),
        TEXT("%s: %s was null but is a required field"),
        *OnObject->GetFullName(),
        DependencyFieldName);
  }

  static bool EnsureMsgfDependencyApiIsInitialized(
      const void* ApiInstance,
      const UObject* OnObject,
      const TCHAR* DependencyFieldName)
  {
    return ensureMsgf(
        ApiInstance != nullptr,
        TEXT("%s: %s dependency could not be initialized"),
        *OnObject->GetFullName(),
        DependencyFieldName);
  }

  /* Checks the validity of the dependency (as a generic pointer), logs a detailed error. Ideal for
   * dependencies where developer experience is prioritized over throwing an ensure */
  static bool ValidateDependency(
      const void* Dependency,
      const UObject* OnObject,
      const TCHAR* DependencyFieldName,
      const TCHAR* CallingFunction,
      const AActor* ParentActor)
  {
    if (Dependency == nullptr)
    {
      const FString ErrorDetail = AssembleErrorDetailString(
          OnObject, ParentActor, DependencyFieldName, CallingFunction, "NULL POINTER!");
      UE_LOG(LogOculusInteraction, Error, TEXT("%s"), *ErrorDetail);
      return false;
    }

    return true;
  }

  /* Checks the validity of the dependency (as a UObject), logs a detailed error. Ideal for
   * dependencies where developer experience is prioritized over throwing an ensure */
  static bool ValidateUObjectDependency(
      const UObject* Dependency,
      const UObject* OnObject,
      const TCHAR* DependencyFieldName,
      const TCHAR* CallingFunction,
      const AActor* ParentActor)
  {
    if (!IsValid(Dependency))
    {
      const FString ErrorDetail = AssembleErrorDetailString(
          OnObject, ParentActor, DependencyFieldName, CallingFunction, "INVALID UOBJECT!");
      UE_LOG(LogOculusInteraction, Error, TEXT("%s"), *ErrorDetail);
      return false;
    }

    return true;
  }

  /* Checks if a passed in condition is false and if so logs a detailed error. Ideal for conditions
   * where developer experience is prioritized over throwing an ensure */
  static bool ValidateCondition(
      const bool bConditionMet,
      const UObject* OnObject,
      const TCHAR* ConditionContext,
      const TCHAR* CallingFunction,
      const AActor* ParentActor)
  {
    if (!bConditionMet)
    {
      const FString ErrorDetail = AssembleErrorDetailString(
          OnObject, ParentActor, ConditionContext, CallingFunction, "CONDITION FALSE!");
      UE_LOG(LogOculusInteraction, Error, TEXT("%s"), *ErrorDetail);
      return false;
    }

    return true;
  }

  static void
  WarnIfApiSizeInvalid(const FVector2D& Size, const UObject* OnObject, const TCHAR* FieldName)
  {
    if (Size.X <= 0.f || Size.Y <= 0.f)
    {
      UE_LOG(
          LogOculusInteraction,
          Warning,
          TEXT("%s: All dimensions of %s must be greater than zero."),
          *OnObject->GetFullName(),
          FieldName);
    }
  }

  static void
  WarnIfApiSizeInvalid(const FVector& Size, const UObject* OnObject, const TCHAR* FieldName)
  {
    if (Size.X <= 0.f || Size.Y <= 0.f || Size.Z <= 0.f)
    {
      UE_LOG(
          LogOculusInteraction,
          Warning,
          TEXT("%s: All dimensions of %s must be greater than zero."),
          *OnObject->GetFullName(),
          FieldName);
    }
  }

 private:
  static FString AssembleErrorDetailString(
      const UObject* OnObject,
      const AActor* ParentActor,
      const TCHAR* ErrorContext,
      const TCHAR* CallingFunction,
      const FString ErrorDetail)
  {
    FString DetailString = "[ISDK] " + ErrorDetail + " Context: " + FString(ErrorContext);

    if (OnObject != nullptr)
    {
      DetailString += " | On Object: " + OnObject->GetFullName();
    }
    if (ParentActor != nullptr)
    {
      DetailString += " | Parent Actor: " + ParentActor->GetFullName();
    }
    DetailString += " | Calling Function : " + FString(CallingFunction);
    return DetailString;
  }
};
