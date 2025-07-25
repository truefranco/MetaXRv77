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

#include "Interaction/IsdkISurfacePatch.h"

#include "IsdkChecks.h"

// Helper method that's used in validation of an isdk::api dependency.
bool IIsdkISurfacePatch::EnsureSurfacePatchValid(
    const TScriptInterface<IIsdkISurfacePatch>& InSurfacePatch,
    UObject* OnObject,
    const TCHAR* OnObjectFieldName,
    isdk::api::ISurfacePatch*& ApiSurfacePatch)
{
  // Check that the UObject is not null
  if (!UIsdkChecks::ValidateUObjectDependency(
          InSurfacePatch.GetObject(),
          OnObject,
          OnObjectFieldName,
          ANSI_TO_TCHAR(__FUNCTION__),
          nullptr))
  {
    return false;
  }
  ApiSurfacePatch = InSurfacePatch->GetApiISurfacePatch();
  // also check that we can get a valid native instance.
  if (!UIsdkChecks::ValidateDependency(
          ApiSurfacePatch, OnObject, OnObjectFieldName, ANSI_TO_TCHAR(__FUNCTION__), nullptr))
  {
    return false;
  }
  return true;
}
