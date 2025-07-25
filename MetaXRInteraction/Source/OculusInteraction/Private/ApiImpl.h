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

#include <functional>

template <typename TApiType, typename TApiTypePtr>
class FApiImpl
{
 public:
  FApiImpl(std::function<TApiTypePtr()> InCreateFn) : CreateFn(InCreateFn) {}

  virtual ~FApiImpl()
  {
    DestroyInstance();
  }

  TApiType* GetOrCreateInstance()
  {
    if (Instance.IsValid())
    {
      return &Instance.Get();
    }
    if (!bCreateInstanceFailed)
    {
      Instance = CreateFn();
      if (Instance.IsValid())
      {
        OnInstanceCreated(&Instance.Get());
        return &Instance.Get();
      }
      bCreateInstanceFailed = true;
    }
    return nullptr;
  }

  TApiType& GetInstanceChecked()
  {
    check(Instance.IsValid());
    return Instance.Get();
  }

  virtual void OnInstanceCreated(TApiType* NewInstance) {}

  void DestroyInstance()
  {
    Instance.Reset();
    bCreateInstanceFailed = false;
  }

  bool IsInstanceValid() const
  {
    return Instance.IsValid();
  }

 private:
  std::function<TApiTypePtr()> CreateFn;
  TApiTypePtr Instance{};
  bool bCreateInstanceFailed{};
};
