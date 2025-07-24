// @lint-ignore-every LICENSELINT
// Copyright Epic Games, Inc. All Rights Reserved.
// OculusEventComponent.h: Component to handle receiving events from Oculus HMDs

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/ObjectMacros.h"
#include "Components/StereoLayerComponent.h"
#include "OculusXRPassthroughLayerShapes.h"
#include "OculusXRPassthroughColorLut.h"
#include "OculusXRHMDRuntimeSettings.h"
#include "OculusXRPassthroughLayerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOculusPassthrough, Log, All);

/**
 * @brief Represents a layer used for passthrough.
 *
 * The Passthrough API enables you to show the user's real environment in your mixed reality experiences.
 * It offers several options to customize the appearance of passthrough, such as adjusting opacity,
 * highlight salient edges in the image, or control the color reproduction.
 * For passthrough to be visible, it must be enabled in the Meta XR Plugin
 * via the \ref UOculusXRHMDRuntimeSettings::bInsightPassthroughEnabled property.
 *
 * @see https://developers.meta.com/horizon/documentation/unreal/unreal-passthrough-overview/ to learn more about passthrough and its features
 * @see https://developers.meta.com/horizon/documentation/unreal/unreal-passthrough-tutorial/ to create a simple app which uses passthrough
 */
UCLASS(Abstract, meta = (DisplayName = "Passthrough Layer Base"))
class OCULUSXRPASSTHROUGH_API UOculusXRPassthroughLayerBase : public UStereoLayerShape
{
	GENERATED_BODY()
public:
	/**
	 * Specifies whether passthrough should appear on top of (when \ref LayerOrder is `PassthroughLayerOrder_Overlay`)
	 * or beneath (when \ref LayerOrder is `PassthroughLayerOrder_Underlay`) the virtual content. The default is `Overlay`.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", DisplayName = "Layer Placement")
	TEnumAsByte<enum EOculusXRPassthroughLayerOrder> LayerOrder;

	/**
	 * Defines the passthrough opacity. It can be used to blend between passthrough and VR when \ref LayerOrder is set to `Overlay`,
	 * or to dim passthrough when \ref LayerOrder is set to `Underlay`.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (UIMin = 0.0, UIMax = 1.0, ClampMin = 0.0, ClampMax = 1.0))
	float TextureOpacityFactor = 1.0f;

	/**
	 * Enables or disables edge rendering.
	 * Use this flag to toggle the edge rendering but retain the previously selected color (including alpha) in the UI when it is disabled.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (DisplayName = "Enable Edge Rendering"))
	bool bEnableEdgeColor = false;

	/** Color for the edge rendering. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (EditCondition = "bEnableEdgeColor", EditConditionHides))
	FLinearColor EdgeColor;

	/** Enables or disables the color mapping for this layer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties")
	bool bEnableColorMap = false;

	/** Represents the color mapping technique applied to this layer. Prefer setting it with \ref SetColorMapType() method */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (EditCondition = "bEnableColorMap", EditConditionHides))
	TEnumAsByte<enum EOculusXRColorMapType> ColorMapType;

	/** Controls whether to use a color map curve or a gradient. Use it together with \ref ColorMapCurve property, or set it with \ref EnableColorMapCurve(). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (EditCondition = "bEnableColorMap && ColorMapType == 1", EditConditionHides))
	bool bUseColorMapCurve = false;

	/** Contains color mapping gradient which is used to convert grayscale passthrough to color. This property only has an effect if \ref ColorMapType is set to \ref ColorMapType_GrayscaleToColor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (EditCondition = "bEnableColorMap && bUseColorMapCurve && ColorMapType == 1", EditConditionHides))
	UCurveLinearColor* ColorMapCurve;

	/** Contains contrast setting for color mapping. Ranges from -1 (minimum) to 1 (maximum). This property only has an effect if \ref ColorMapType is set to \ref ColorMapType_ColorAdjustment, \ref ColorMapType_Grayscale or \ref ColorMapType_GrayscaleToColor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (ClampMin = "-1", ClampMax = "1", EditCondition = "bEnableColorMap && ColorMapType > 0 && ColorMapType < 4", EditConditionHides))
	float Contrast = 0.0f;

	/** Contains brightness setting for color mapping. Ranges from -1 (minimum) to 1 (maximum). This property only has an effect if \ref ColorMapType is set to \ref ColorMapType_ColorAdjustment, \ref ColorMapType_Grayscale or \ref ColorMapType_GrayscaleToColor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (ClampMin = "-1", ClampMax = "1", EditCondition = "bEnableColorMap && ColorMapType > 0 && ColorMapType < 4", EditConditionHides))
	float Brightness = 0.0f;

	/** Contains posterize setting for grayscale and grayscale to color mappings. Ranges from 0 to 1, where 0 = no posterization (no effect), 1 = reduce to two colors. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (ClampMin = "0", ClampMax = "1", EditCondition = "bEnableColorMap && ColorMapType > 0 && ColorMapType < 3", EditConditionHides))
	float Posterize = 0.0f;

	/** Contains saturation for color adjustment mapping. Ranges from -1 (minimum) to 1 (maximum). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (ClampMin = "-1", ClampMax = "1", EditCondition = "bEnableColorMap && ColorMapType == 3", EditConditionHides))
	float Saturation = 0.0f;

	/**
	 * Controls how \ref ColorMapType_ColorLut color mapping technique is applied to passthrough.
	 * If the value is 0, then the appearance of Passthrough is unchanged. If it is 1, the colors are fully taken from the LUT. Values between 0 and 1 lead to a linear interpolation
	 * between the original color and the LUT color. If two LUTs are provided LutWeight is used to blend them.
	 * This value can be animated to create smooth transitions.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (ClampMin = "0", ClampMax = "1", EditCondition = "bEnableColorMap && ColorMapType > 3", EditConditionHides))
	float LutWeight = 1.0f;

	/**
	 *  Color LUT properties. If only \ref ColorLUTSource is provided it will be blended with passthrough layer using following formula:
	 *  Result = ColorLUTSource * LutWeight + Passthrough * ( 1 - LutWeight )
	 */
	UPROPERTY(EditAnywhere, Category = "Passthrough Properties", meta = (EditCondition = "bEnableColorMap && ColorMapType > 3", EditConditionHides))
	UOculusXRPassthroughColorLut* ColorLUTSource;

	/**
	 *  Color LUT properties. If both \ref ColorLUTSource and \ref ColorLUTTarget are provided they will be blended using following formula:
	 *  Result = ColorLUTsSource * ( 1 - LutWeight ) + ColorLUTsTarget * LutWeight
	 */
	UPROPERTY(EditAnywhere, Category = "Passthrough Properties", meta = (EditCondition = "bEnableColorMap && ColorMapType > 4", EditConditionHides))
	UOculusXRPassthroughColorLut* ColorLUTTarget;

	/**
	 * Contains the color value that will be multiplied to the pixel color values during compositing. Default is white = `(1,1,1,1)`.
	 * This property only has an effect if \ref ColorMapType is set to \ref ColorMapType_GrayscaleToColor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (EditCondition = "bEnableColorMap && ColorMapType == 1", EditConditionHides))
	FLinearColor ColorScale = FLinearColor::White;

	/**
	 * Contains the color value that will be added to the pixel color values during compositing. Default is black = `(0,0,0,0)`.
	 * This property only has an effect if \ref ColorMapType is set to \ref ColorMapType_GrayscaleToColor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Passthrough Properties", meta = (EditCondition = "bEnableColorMap && ColorMapType == 1", EditConditionHides))
	FLinearColor ColorOffset = FLinearColor::Black;

	/**
	 * This method changes the passthrough texture opacity. See \ref TextureOpacityFactor for more details.
	 * @param InOpacity New value of the passthrough texture opacity.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetTextureOpacity(float InOpacity);

	/**
	 * This method allows to enable or disable the edge rendering in this passthrough layer. See \ref bEnableEdgeColor for more details.
	 * @param bInEnableEdgeColor Specify `true` to enable edge rendering.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void EnableEdgeColor(bool bInEnableEdgeColor);

	/**
	 * This method allows to enable or disable the color mapping technique which is configured for this passthrough layer.
	 * @param bInEnableColorMap Specify `true` to enable the color mapping, and `false` to disable it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void EnableColorMap(bool bInEnableColorMap);

	/**
	 * Enable or disables the color map curve used to convert grayscale passthrough to color. This mehtod only has an effect when the color mapping type is set to \ref ColorMapType_GrayscaleToColor.
	 * See \ref bUseColorMapCurve for more details.
	 * @param bInEnableColorMapCurve Specify `true` to apply the color map curve.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void EnableColorMapCurve(bool bInEnableColorMapCurve);

	/**
	 * This method controls the color of the edges when edge rendering is enabled. See \ref EdgeColor for more details.
	 * @param InEdgeColor Edge rendering color.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetEdgeRenderingColor(FLinearColor InEdgeColor);

	/**
	 * Sets the color map controls for grayscale and grayscale to rgb color mappings. The method fails if ColorMapType is not set to ColorMapType_Grayscale or ColorMapType_GrayscaleToColor.
	 * @param InContrast Contast of passthrough. Valid range: [-1, 1]. A value of 0 means that contrast is left unchanged.
	 * @param InBrightness Brightness of passthrough. Valid range: [-1, 1]. A value of 0 means that brightness is left unchanged.
	 * @param InPosterize Posterize of passthrough. Ranges from 0 to 1, where 0 = no posterization (no effect), 1 = reduce to two colors.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetColorMapControls(float InContrast = 0, float InBrightness = 0, float InPosterize = 0);

	/**
	 * This method allows to configure brightness and contrast adjustment for Passthrough images. It fails if ColorMapType is not set to ColorMapType_ColorAdjustment.
	 * @param InContrast Contast of passthrough. Valid range: [-1, 1]. A value of 0 means that contrast is left unchanged.
	 * @param InBrightness Brightness of passthrough. Valid range: [-1, 1]. A value of 0 means that brightness is left unchanged.
	 * @param InSaturation Saturation of passthrough. Valid range: [-1, 1]. A value of 0 means that saturation is left unchanged.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetBrightnessContrastSaturation(float InContrast = 0, float InBrightness = 0, float InSaturation = 0);

	/**
	 * This method allows to configure the color scale and offset values applied to passthrough pixels.
	 * The method only has an effect if \ref ColorMapType is set to \ref ColorMapType_GrayscaleToColor.
	 * @param InColorScale Color value that will be multiplied to the pixel color values during compositing. Default is `{1,1,1,1}`
	 * @param InColorOffset Color value that will be added to the pixel color values during compositing. Default is `{0,0,0,0}`.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetColorScaleAndOffset(FLinearColor InColorScale = FLinearColor::White, FLinearColor InColorOffset = FLinearColor::Black);

	/** Sets the color curve that will be added to the color map in grayscale modes --> will be converted into a gradient */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetColorMapCurve(UCurveLinearColor* InColorMapCurve);

	/** Sets the color mapping technique applied to the passthrough texture if \ref bEnableColorMap is set to `true` */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetColorMapType(EOculusXRColorMapType InColorMapType);

	/** Set color map array directly instead through a color curve. This method only has an affect when \ref ColorMapType is set to \ref ColorMapType_GrayscaleToColor */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetColorArray(const TArray<FLinearColor>& InColorArray);

	/** Clears any color maps previously applied to this layer and sets ColorMapType to \ref ColorMapType_None value */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void ClearColorMap();

	/**
	 * Specifies whether passthrough should appear on top of (\ref PassthroughLayerOrder_Overlay)
	 * or beneath (\ref PassthroughLayerOrder_Underlay) the virtual content. The default is `Overlay`.
	 * See \ref LayerOrder property for more details */
	UFUNCTION(BlueprintCallable, Category = "Passthrough Properties")
	void SetLayerPlacement(EOculusXRPassthroughLayerOrder InLayerOrder);

	/**
	 * Sets Color LUT source.
	 * If ColorMapType is `Color LUT`, then source will be blended with passthrough
	 * using folowing formula:
	 * Result = ColorLUTSource * LutWeight + Passthrough * (1 - LutWeight )
	 * If ColorMapType is `Interpolated Color LUT`, then source will be blended with color LUT target
	 * using folowing formula:
	 * Result = ColorLUTSource * (  1 - LutWeight ) + ColorLUTTarget * LutWeight
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetColorLUTSource(class UOculusXRPassthroughColorLut* InColorLUTSource);

	/**
	 * Sets Color LUT target.
	 * If ColorMapType is `Interpolated Color LUT`, then target will be blended with passthrough
	 * using folowing formula:
	 * Result = ColorLUTSource * (  1 - LutWeight ) + ColorLUTTarget * LutWeight
	 * Note: If ColorLUTSource is not specified, Color LUT will be not be applied to the Passthrough layer.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetColorLUTTarget(class UOculusXRPassthroughColorLut* InColorLUTTarget);

	/** Sets the color LUT weight. See \ref LutWeight for more details */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void SetColorLUTWeight(float InWeight = 1.0f);

	/** Removes color grading if any is active */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void RemoveColorLut();

	void ClearLUTsReferences();

	virtual void BeginDestroy();

protected:
	TArray<FLinearColor> ColorArray;
	TArray<FLinearColor> NeutralColorArray;
	TArray<FLinearColor> GenerateColorArrayFromColorCurve(const UCurveLinearColor* InColorMapCurve) const;
	TArray<FLinearColor> GetOrGenerateNeutralColorArray();
	TArray<FLinearColor> GenerateColorArray(bool bInUseColorMapCurve, const UCurveLinearColor* InColorMapCurve);
	TArray<FLinearColor> GetColorArray(bool bInUseColorMapCurve, const UCurveLinearColor* InColorMapCurve);
	FColorLutDesc GenerateColorLutDescription(float InLutWeight, UOculusXRPassthroughColorLut* InLutSource, UOculusXRPassthroughColorLut* InLutTarget);

	void MarkStereoLayerDirty();
};

/**
 * @brief Represents a passthrough layer which uses automatic environment depth reconstruction to render itself.
 *
 * The Passthrough API enables you to show the user's real environment in your mixed reality experiences.
 * It offers several options to customize the appearance of passthrough, such as adjusting opacity,
 * highlight salient edges in the image, or control the color reproduction.
 * For passthrough to be visible, it must be enabled in the Meta XR Plugin
 * via the \ref UOculusXRHMDRuntimeSettings::bInsightPassthroughEnabled property.
 *
 * Reconstructed passthrough used via this class vanishes when the current component
 * is destroyed. Consider using Persistent Passthrough Layer instead, which remains active throughout the application's lifetime,
 * including across level transitions. See https://developers.meta.com/horizon/documentation/unreal/unreal-persistent-passthrough/ for more details.
 */
UCLASS(meta = (DisplayName = "Reconstructed Passthrough Layer"))
class OCULUSXRPASSTHROUGH_API UOculusXRStereoLayerShapeReconstructed : public UOculusXRPassthroughLayerBase
{
	GENERATED_BODY()
public:
	virtual void ApplyShape(IStereoLayers::FLayerDesc& LayerDesc) override;
};

/**
 * @brief Represents a passthrough layer which relies on the geometry and the depth provided by the client to render itself.
 *
 * The Passthrough API enables you to show the user's real environment in your mixed reality experiences.
 * It offers several options to customize the appearance of passthrough, such as adjusting opacity,
 * highlight salient edges in the image, or control the color reproduction.
 * For passthrough to be visible, it must be enabled in the Meta XR Plugin
 * via the \ref UOculusXRHMDRuntimeSettings::bInsightPassthroughEnabled property.
 *
 * @see https://developers.meta.com/horizon/documentation/unreal/unreal-customize-passthrough-surface-projected-passthrough/ to learn more about surface projected passthrough.
 */
UCLASS(meta = (DisplayName = "User Defined Passthrough Layer"))
class OCULUSXRPASSTHROUGH_API UOculusXRStereoLayerShapeUserDefined : public UOculusXRPassthroughLayerBase
{
	GENERATED_BODY()
public:
	/**
	 * Adds a geometry onto which the passthrough images will be projected.
	 * @param MeshName Name for the geometry to be added. It is used if later you decide to remove the geometry from the layer.
	 * @param PassthroughMesh Reference to the mesh to be added.
	 * @param Transform Transform of the mesh to be added.
	 * @param bUpdateTransform When the value is `true`, current layer will update the transform of the surface mesh every frame.
	 * Otherwise only the initial transform is recorded.
	 */
	void AddGeometry(const FString& MeshName, OculusXRHMD::FOculusPassthroughMeshRef PassthroughMesh, FTransform Transform, bool bUpdateTransform);
	/**
	 * Removes the geometry that has previously been added using \ref AddGeometry() from the projection surface
	 * @param MeshName Name for the geometry to be removed.
	 */
	void RemoveGeometry(const FString& MeshName);

	virtual void ApplyShape(IStereoLayers::FLayerDesc& LayerDesc) override;
	/**
	 * Returns a list of geometries previously added to this surface projected passthrough layer.
	 */
	TArray<FUserDefinedGeometryDesc>& GetUserGeometryList() { return UserGeometryList; };

private:
	TArray<FUserDefinedGeometryDesc> UserGeometryList;
};

class UProceduralMeshComponent;

/**
 * @brief A component which defines reusable passthrough behavior that can be added to different types of Actors.
 *
 * The Passthrough API enables you to show the user's real environment in your mixed reality experiences.
 * It offers several options to customize the appearance of passthrough, such as adjusting opacity,
 * highlight salient edges in the image, or control the color reproduction.
 *
 * @see https://developers.meta.com/horizon/documentation/unreal/unreal-passthrough-overview/ to learn more about passthrough and its features
 * @see https://developers.meta.com/horizon/documentation/unreal/unreal-passthrough-tutorial/ to create a simple app which uses passthrough
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent), ClassGroup = OculusXRHMD)
class OCULUSXRPASSTHROUGH_API UOculusXRPassthroughLayerComponent : public UStereoLayerComponent
{
	GENERATED_UCLASS_BODY()

public:
	void DestroyComponent(bool bPromoteChildren) override;
	void OnRegister() override;

	void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdatePassthroughObjects();

	/**
	 * \deprecated Adds a static geometry onto which the passthrough images will be projected. This method is deprecated in favour of \ref AddStaticSurfaceGeometry().
	 *
	 * @param StaticMeshActor The actor containing the static geometry.
	 * @param bUpdateTransform When the value is `true`, current layer will update the transform of the mesh every frame.
	 * Otherwise only the initial transform is recorded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Passthrough", meta = (DeprecatedFunction, DeprecationMessage = "Please use AddStaticSurfaceGeometry instead"))
	void AddSurfaceGeometry(AStaticMeshActor* StaticMeshActor, bool updateTransform);
	/**
	 * Adds a static geometry onto which the passthrough images will be projected. This only has an effect with the surface projected passthrough layers.
	 * @see https://developers.meta.com/horizon/documentation/unreal/unreal-customize-passthrough-surface-projected-passthrough/ to learn more about it.
	 *
	 * @param StaticMeshComponent Reference to the component that contains the static mesh to be added.
	 * @param updateTransform When the value is `true`, corresponding passthrough layer will update the transform of the surface mesh every frame.
	 * Otherwise only the initial transform is recorded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Passthrough")
	void AddStaticSurfaceGeometry(UStaticMeshComponent* StaticMeshComponent, bool updateTransform);
	/**
	 * Adds a procedural geometry onto which the passthrough images will be projected. This only has an effect with the surface projected passthrough layers.
	 * @see https://developers.meta.com/horizon/documentation/unreal/unreal-customize-passthrough-surface-projected-passthrough/ to learn more about it.
	 *
	 * @param ProceduralMeshComponent Reference to the component that contains the procedural mesh to be added.
	 * @param updateTransform When the value is `true`, corresponding passthrough will update the transform of the surface mesh every frame.
	 * Otherwise only the initial transform is recorded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Passthrough")
	void AddProceduralSurfaceGeometry(UProceduralMeshComponent* ProceduralMeshComponent, bool updateTransform);

	/**
	 * \deprecated Removes previously added static geometry from the projection surface. This method is deprecated in favour of \ref RemoveStaticSurfaceGeometry().
	 * @param StaticMeshActor The actor with the static geometry to be removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Passthrough", meta = (DeprecatedFunction, DeprecationMessage = "Please use RemoveStaticSurfaceGeometry instead"))
	void RemoveSurfaceGeometry(AStaticMeshActor* StaticMeshActor);

	/**
	 * Removes previously added static geometry from the projection surface.
	 * @param StaticMeshComponent Reference to the component that contains the static mesh to be removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Passthrough")
	void RemoveStaticSurfaceGeometry(UStaticMeshComponent* StaticMeshComponent);
	/**
	 * Removes previously added procedural geometry from the projection surface.
	 * @param ProceduralMeshComponent Reference to the component that contains the procedural mesh to be removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Passthrough")
	void RemoveProceduralSurfaceGeometry(UProceduralMeshComponent* ProceduralMeshComponent);

	/**
	 * \deprecated Checks if the actor contains a static geomenty that has been added to the current component. This method is deprecated in favour of \ref IsSurfaceGeometryComponent().
	 * @param StaticMeshActor The actor for which the check is to be made.
	 */
	UFUNCTION(BlueprintCallable, Category = "Passthrough", meta = (DeprecatedFunction, DeprecationMessage = "Please use IsSurfaceGeometryComponent instead"))
	bool IsSurfaceGeometry(AStaticMeshActor* StaticMeshActor) const;
	/**
	 * Checks if the current component contains the mesh passed as an argument.
	 * @param MeshComponent The component with the mesh for which the check is to be made.
	 */
	UFUNCTION(BlueprintPure, Category = "Passthrough")
	bool IsSurfaceGeometryComponent(const UMeshComponent* MeshComponent) const;

	/**
	 * Manually mark the stereo layer passthrough effect for updating.
	 */
	UFUNCTION(BlueprintCallable, Category = "Components|Stereo Layer")
	void MarkPassthroughStyleForUpdate();

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif // WITH_EDITOR

	/**
	 * Calling this method results in broadcasting \ref OnLayerResumed event if `InLayerId` matches the Id of the current passthrough layer.
	 */
	UFUNCTION()
	void OnAnyLayerResumedEvent(int InLayerId);

	/**
	 * Occurs when current passthrough layer has been rendered and presented on the HMD screen for the first time after being restarted.
	 */
	UPROPERTY(BlueprintAssignable)
	FOculusXRPassthrough_LayerResumed OnLayerResumed;

protected:
	virtual bool LayerRequiresTexture();
	virtual void RemoveSurfaceGeometryComponent(UMeshComponent* MeshComponent);

	UPROPERTY(Transient)
	TMap<FString, const UMeshComponent*> PassthroughComponentMap;

private:
	OculusXRHMD::FOculusPassthroughMeshRef CreatePassthroughMesh(UProceduralMeshComponent* ProceduralMeshComponent);
	OculusXRHMD::FOculusPassthroughMeshRef CreatePassthroughMesh(UStaticMeshComponent* StaticMeshComponent);

	/** Passthrough style needs to be marked for update **/
	bool bPassthroughStyleNeedsUpdate;
};
