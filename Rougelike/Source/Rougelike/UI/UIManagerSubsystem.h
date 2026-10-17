#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "UI/UILayout.h"
#include "UI/UITypes.h"
#include "UIManagerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUILayerChanged, FGameplayTag, LayerTag, UCommonActivatableWidget *, ActiveWidget);

/**
 *  Global entry point for all UI. Lives on the GameInstance, so it survives level changes.
 *  Everything is addressed by layer tag. The root layout is created on demand and rebuilt
 *  whenever the local PlayerController changes (e.g. after OpenLevel).
 */
UCLASS()
class ROUGELIKE_API UUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** C++ accessor. Blueprints use the auto-generated "Get UIManagerSubsystem" node. */
	static UUIManagerSubsystem *Get(const UObject *WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject *Outer) const override;
	virtual void Deinitialize() override;

	/** Pushes a new instance on top of the layer. In Blueprint the return value is typed to WidgetClass. */
	UFUNCTION(BlueprintCallable, Category = "UI", meta = (DeterminesOutputType = "WidgetClass"))
	UCommonActivatableWidget *PushWidget(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> WidgetClass);

	/** C++ only. InitFunc runs on the instance before it is activated. T must be given explicitly. */
	template <typename T>
	T *PushWidget(FGameplayTag LayerTag, TSubclassOf<T> WidgetClass, TFunctionRef<void(T &)> InitFunc);

	/** Removes the top widget of the layer. The one beneath becomes active again. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PopWidget(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag);

	/** Removes this widget from whichever layer holds it. False if no layer holds it. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	bool RemoveWidget(UCommonActivatableWidget *Widget);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ClearLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ClearLayers(UPARAM(meta = (Categories = "UI.Layer")) const FGameplayTagContainer &LayerTags);

	UFUNCTION(BlueprintPure, Category = "UI")
	UCommonActivatableWidget *GetActiveWidget(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag) const;

	UFUNCTION(BlueprintPure, Category = "UI")
	bool IsLayerEmpty(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag) const;

	/** Top widget of any layer changed. ActiveWidget is null when that layer became empty. */
	UPROPERTY(BlueprintAssignable, Category = "UI")
	FOnUILayerChanged OnLayerChanged;

private:
	/** The layout if it belongs to the current local PlayerController, else null. Never creates one. */
	UUILayout *GetCurrentLayout() const;

	/** The current layout, or a fresh one for the first local PlayerController */
	UUILayout *GetOrCreateLayout();

	UCommonActivatableWidgetContainerBase *FindLayer(FGameplayTag LayerTag) const;
	void ReleaseLayout();
	void HandleLayerChanged(FGameplayTag LayerTag, UCommonActivatableWidget *ActiveWidget);

	/** Weak on purpose: the layout belongs to the PlayerController's world. A strong ref would leak that world on OpenLevel. */
	TWeakObjectPtr<UUILayout> Layout;
};

template <typename T>
T *UUIManagerSubsystem::PushWidget(FGameplayTag LayerTag, TSubclassOf<T> WidgetClass, TFunctionRef<void(T &)> InitFunc)
{
	UUILayout *CurrentLayout = GetOrCreateLayout();
	UCommonActivatableWidgetContainerBase *Layer = CurrentLayout ? CurrentLayout->GetLayer(LayerTag) : nullptr;
	return (Layer && WidgetClass) ? Layer->AddWidget<T>(WidgetClass, InitFunc) : nullptr;
}
