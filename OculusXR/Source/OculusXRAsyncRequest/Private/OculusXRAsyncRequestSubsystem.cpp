// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRAsyncRequestSubsystem.h"
#include "OculusXRAsyncRequestSystem.h"
#include <Engine/World.h>
#include <Engine/GameInstance.h>

#if WITH_EDITOR
#include <Engine/Engine.h>
#endif

UOculusXRAsyncRequestSubsystem* UOculusXRAsyncRequestSubsystem::GetSubsystem()
{
	if (GWorld != nullptr)
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			for (const FWorldContext& Context : GEngine->GetWorldContexts())
			{
				if (Context.WorldType == EWorldType::PIE)
				{
					return Context.World()->GetGameInstance()->GetSubsystem<UOculusXRAsyncRequestSubsystem>();
				}
			}
		}
#endif // WITH_EDITOR

		return GWorld->GetGameInstance()->GetSubsystem<UOculusXRAsyncRequestSubsystem>();
	}

	return nullptr;
}

OculusXR::FAsyncRequestSystem* UOculusXRAsyncRequestSubsystem::GetRequestSystem()
{
	return (GetSubsystem()->RequestSystem).Get();
}

void UOculusXRAsyncRequestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	RequestSystem = MakeShared<OculusXR::FAsyncRequestSystem>();
}

void UOculusXRAsyncRequestSubsystem::Deinitialize()
{
}
