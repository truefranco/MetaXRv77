// Copyright (c) Meta Platforms, Inc. and affiliates.

#include "OculusXRColocationFunctions.h"
#include "OculusXRColocationFunctionsOVR.h"
#include "OculusXRColocationFunctionsOpenXR.h"
#include "IOpenXRHMD.h"
#include "OculusXRHMD.h"

TSharedPtr<IOculusXRColocationFunctions> IOculusXRColocationFunctions::ColocationFunctionsImpl = nullptr;
TSharedPtr<IOculusXRColocationFunctions> IOculusXRColocationFunctions::GetOculusXRColocationFunctionsImpl()
{
	if (ColocationFunctionsImpl == nullptr)
	{
		const FName SystemName(TEXT("OpenXR"));
		const bool IsOpenXR = GEngine->XRSystem.IsValid() && (GEngine->XRSystem->GetSystemName() == SystemName);
		if (OculusXRHMD::FOculusXRHMD::GetOculusXRHMD() != nullptr)
		{
			ColocationFunctionsImpl = MakeShared<FOculusXRColocationFunctionsOVR>();
		}
		else if (IsOpenXR)
		{
			ColocationFunctionsImpl = MakeShared<FOculusXRColocationFunctionsOpenXR>();
		}
	}

	check(ColocationFunctionsImpl);
	return ColocationFunctionsImpl;
}
