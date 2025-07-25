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
#include "IsdkHasDebugSegments.generated.h"

UINTERFACE()
class UIsdkHasDebugSegments : public UInterface
{
  GENERATED_BODY()
};

/**
 * IsdkHasDebugSegments is used to support the drawing of arbitrary geometry that can be represented
 * as a set of segments. In particular, useful if we know we want to draw a shape, but we don't know
 * what the shape is.  Implementations may not specifically be performant, as this is intended for
 * debug purposes.
 */
class OCULUSINTERACTION_API IIsdkHasDebugSegments
{
  GENERATED_BODY()

 public:
  // Get a set of point pairs describing the lines that are needed to draw this
  virtual void GetDebugSegments(TArray<TPair<FVector, FVector>>& OutSegments) const
      PURE_VIRTUAL(ISurfacePatch::DrawDebugVisuals);
};
