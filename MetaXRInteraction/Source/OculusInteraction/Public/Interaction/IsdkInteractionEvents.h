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

#include "IsdkInteractionEvents.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIsdkIUpdateEventDelegate);

// This class is not used - just here to make things compile properly, since header files don't
// work properly when they only have Delegates definitions in them and nothing else.
UCLASS()
class UIsdkInteractionEvents : public UObject
{
  GENERATED_BODY()
};
