#include "UI/UILayout.h"
#include "UI/UITypes.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UUILayout::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase *LayerWidget)
{
	if (!LayerTag.IsValid() || !LayerWidget)
	{
		UE_LOG(LogUI, Error, TEXT("%s: RegisterLayer needs a valid tag and container."), *GetName());
		return;
	}

	if (const TObjectPtr<UCommonActivatableWidgetContainerBase> *Existing = Layers.Find(LayerTag))
	{
		UE_LOG(LogUI, Warning, TEXT("%s: layer %s registered twice, replacing it."), *GetName(), *LayerTag.ToString());
		(*Existing)->OnDisplayedWidgetChanged().RemoveAll(this);
	}

	// Re-broadcast with the tag, so listeners never need to know about containers
	LayerWidget->OnDisplayedWidgetChanged().AddWeakLambda(this, [this, LayerTag](UCommonActivatableWidget *ActiveWidget)
	{
		OnLayerChanged.Broadcast(LayerTag, ActiveWidget);
	});

	Layers.Add(LayerTag, LayerWidget);
}

UCommonActivatableWidgetContainerBase *UUILayout::GetLayer(FGameplayTag LayerTag) const
{
	if (const TObjectPtr<UCommonActivatableWidgetContainerBase> *Found = Layers.Find(LayerTag))
	{
		return *Found;
	}

	UE_LOG(LogUI, Warning, TEXT("%s: no layer registered for %s."), *GetName(), *LayerTag.ToString());
	return nullptr;
}
