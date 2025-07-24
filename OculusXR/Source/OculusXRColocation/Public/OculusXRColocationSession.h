// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "OculusXRColocationTypes.h"
#include "OculusXRColocationSession.generated.h"

USTRUCT(BlueprintType)
struct OCULUSXRCOLOCATION_API FOculusXRColocationSession
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Colocation")
	FOculusXRUUID Uuid;

	UPROPERTY(BlueprintReadOnly, Category = "OculusXR|Colocation")
	TArray<uint8> Metadata;
};
