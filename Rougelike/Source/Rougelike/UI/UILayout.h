#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "UILayout.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLayoutLayerChanged, FGameplayTag /*LayerTag*/, UCommonActivatableWidget * /*ActiveWidget*/);

/**
 *  Root of the UI. Knows nothing about game logic or the UI manager.
 *  The Blueprint child builds an Overlay with one container per layer
 *  and calls RegisterLayer for each in Event On Initialized.
 */
UCLASS(Abstract, Blueprintable)
class ROUGELIKE_API UUILayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	/** Takes the container base class, so a layer can also be a queue (e.g. toasts) instead of a stack */
	UFUNCTION(BlueprintCallable, Category = "UI Layout")
	void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase *LayerWidget);

	UCommonActivatableWidgetContainerBase *GetLayer(FGameplayTag LayerTag) const;

	const TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> &GetLayers() const { return Layers; }

	/** Top widget of a layer changed (nullptr = the layer became empty) */
	FOnLayoutLayerChanged OnLayerChanged;

private:
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};
