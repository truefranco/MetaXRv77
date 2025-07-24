// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "OculusXRAsyncRequestSubsystem.generated.h"

namespace OculusXR
{
	class FAsyncRequestSystem;
}

UCLASS()
class OCULUSXRASYNCREQUEST_API UOculusXRAsyncRequestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static UOculusXRAsyncRequestSubsystem* GetSubsystem();
	static OculusXR::FAsyncRequestSystem* GetRequestSystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	TSharedPtr<OculusXR::FAsyncRequestSystem> RequestSystem;
};
