// Copyright (c) Meta Platforms, Inc. and affiliates.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Texture2D.h"

#include "OculusXRPassthroughColorLut.generated.h"

class UOculusXRPassthroughLayerBase;

enum EColorLutChannels
{
	ColorLutChannels_RGB,
	ColorLutChannels_RGBA
};

USTRUCT()
struct FLutTextureData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<uint8> Data;

	UPROPERTY()
	uint32 Resolution;

	FLutTextureData()
		: Data{}, Resolution(0) {}

	FLutTextureData(const TArray<uint8>& InData, uint32 InResolution)
		: Data(InData), Resolution(InResolution) {}
};

UENUM(BlueprintType)
enum class EColorLutType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	TextureLUT = 1 UMETA(DisplayName = "Texture"),
	Array = 2 UMETA(Hidden)
};

/**
 * @brief This class represents a color look-up table (LUT) that can be applied to an \ref UOculusXRPassthroughLayerBase
 * in order to change the color reproduction of that passthrough layer.
 *
 * Color LUTs map each RGB input color to a new RGB(A) color. It unlocks a number of effects:
 * - Color grading and color correction
 * - Stylizations, such as posterization or hue rotation
 * - Color filtering and chroma keying.
 *
 * To apply a color LUT to a Passthrough layer, create a new instance of `UOculusXRPassthroughLayerComponent`
 * and call \ref UOculusXRPassthroughLayerBase::SetColorLUTSource()"/>.
 *
 * @see https://developers.meta.com/horizon/documentation/unreal/unreal-customize-passthrough-color-mapping/ to learn more about passthrough and color LUTs.
 */
UCLASS(BlueprintType, CollapseCategories, meta = (DisplayName = "Passthrough Color LUT"))
class OCULUSXRPASSTHROUGH_API UOculusXRPassthroughColorLut : public UObject
{
	GENERATED_BODY()
public:
	/** Keeps the information of the source of the current LUT instance. The source can be an array, another LUT texture, or None. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Color LUT")
	EColorLutType ColorLutType = EColorLutType::None;

#if WITH_EDITORONLY_DATA
	/** Color LUT texture. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Color LUT", meta = (EditCondition = "ColorLutType == EColorLutType::TextureLUT", EditConditionHides))
	UTexture2D* LutTexture;
#endif
	/** Informs if the alpha channel of LUT should be ignored. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Color LUT", meta = (EditCondition = "ColorLutType == EColorLutType::TextureLUT", EditConditionHides))
	bool IgnoreAlphaChannel = false;

	/** Generate color LUT from array. Array should have format of exploded cube. Its size should be power of 2. */
	UFUNCTION(BlueprintCallable, Category = "Passthrough Color LUT")
	void SetLutFromArray(const TArray<FColor>& InColorArray, bool InIgnoreAlphaChannel);

	/**
	 * Gets the handle of the lut object. It asks for a layer reference to track the list of objects who currently need the handle.
	 * Call \ref RemoveReference() when you don't need the lut anymore.
	 */
	uint64 GetHandle(UOculusXRPassthroughLayerBase* LayerRef);
	/** Remove a layer reference. When there's no reference to any layer we destroy the lut object and clear the handle. */
	void RemoveReference(UOculusXRPassthroughLayerBase* LayerRef);
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

	void BeginDestroy() override;

private:
	UPROPERTY()
	FLutTextureData StoredTextureData;
	uint64 LutHandle = 0;
	int32 ColorArrayResolution = 0;
	int MaxResolution = -1;
	TArray<uint32> LayerRefs;
	FLutTextureData TextureToColorData(class UTexture2D* InLutTexture) const;
	uint64 CreateLutObject(const TArray<uint8>& InData, uint32 Resolution) const;
	void UpdateLutObject(uint64 Handle, const TArray<uint8>& InData) const;
	void DestroyLutObject(uint64 Handle) const;
	int GetMaxResolution();
};
