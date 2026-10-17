#include "UI/UIManagerSubsystem.h"
#include "UI/UIManagerSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UUIManagerSubsystem *UUIManagerSubsystem::Get(const UObject *WorldContextObject)
{
	const UWorld *World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	const UGameInstance *GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UUIManagerSubsystem>() : nullptr;
}

bool UUIManagerSubsystem::ShouldCreateSubsystem(UObject *Outer) const
{
	return !CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance();
}

void UUIManagerSubsystem::Deinitialize()
{
	ReleaseLayout();
	Super::Deinitialize();
}

UUILayout *UUIManagerSubsystem::GetCurrentLayout() const
{
	UUILayout *Current = Layout.Get();
	const APlayerController *PC = GetGameInstance()->GetFirstLocalPlayerController();

	// After OpenLevel the old layout can still be in memory (off screen, not yet GC'd).
	// It belongs to the old PlayerController, so treat it as gone.
	return (Current && PC && Current->GetOwningPlayer() == PC) ? Current : nullptr;
}

UUILayout *UUIManagerSubsystem::GetOrCreateLayout()
{
	if (UUILayout *Current = GetCurrentLayout())
	{
		return Current;
	}

	APlayerController *PC = GetGameInstance()->GetFirstLocalPlayerController();
	UWorld *World = PC ? PC->GetWorld() : nullptr;
	if (!World || World->bIsTearingDown)
	{
		UE_LOG(LogUI, Warning, TEXT("No local PlayerController available, UI layout not created."));
		return nullptr;
	}

	const UUIManagerSettings *Settings = GetDefault<UUIManagerSettings>();
	const TSubclassOf<UUILayout> LayoutClass = Settings->LayoutClass.LoadSynchronous();
	if (!LayoutClass)
	{
		UE_LOG(LogUI, Error, TEXT("Project Settings > Game > UI Manager > Layout Class is not set."));
		return nullptr;
	}

	// Stale layout from a previous PlayerController, if any
	ReleaseLayout();

	// Layers register themselves during CreateWidget (Event On Initialized), so pushing right after this is safe
	UUILayout *NewLayout = CreateWidget<UUILayout>(PC, LayoutClass);
	if (!NewLayout)
	{
		return nullptr;
	}

	NewLayout->OnLayerChanged.AddUObject(this, &ThisClass::HandleLayerChanged);
	NewLayout->AddToPlayerScreen(Settings->LayoutZOrder);
	Layout = NewLayout;
	return NewLayout;
}

void UUIManagerSubsystem::ReleaseLayout()
{
	if (UUILayout *OldLayout = Layout.Get())
	{
		OldLayout->OnLayerChanged.RemoveAll(this);
		OldLayout->RemoveFromParent();
	}
	Layout.Reset();
}

UCommonActivatableWidgetContainerBase *UUIManagerSubsystem::FindLayer(FGameplayTag LayerTag) const
{
	const UUILayout *Current = GetCurrentLayout();
	return Current ? Current->GetLayer(LayerTag) : nullptr;
}

UCommonActivatableWidget *UUIManagerSubsystem::PushWidget(FGameplayTag LayerTag, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!WidgetClass)
	{
		UE_LOG(LogUI, Warning, TEXT("PushWidget on %s with no widget class."), *LayerTag.ToString());
		return nullptr;
	}

	UUILayout *CurrentLayout = GetOrCreateLayout();
	UCommonActivatableWidgetContainerBase *Layer = CurrentLayout ? CurrentLayout->GetLayer(LayerTag) : nullptr;
	return Layer ? Layer->AddWidget(WidgetClass) : nullptr;
}

void UUIManagerSubsystem::PopWidget(FGameplayTag LayerTag)
{
	UCommonActivatableWidgetContainerBase *Layer = FindLayer(LayerTag);
	if (UCommonActivatableWidget *Top = Layer ? Layer->GetActiveWidget() : nullptr)
	{
		Layer->RemoveWidget(*Top);
	}
}

bool UUIManagerSubsystem::RemoveWidget(UCommonActivatableWidget *Widget)
{
	const UUILayout *Current = GetCurrentLayout();
	if (!Widget || !Current)
	{
		return false;
	}

	for (const TPair<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> &Pair : Current->GetLayers())
	{
		if (Pair.Value->GetWidgetList().Contains(Widget))
		{
			// Handles both the displayed widget and ones further down the stack
			Pair.Value->RemoveWidget(*Widget);
			return true;
		}
	}
	return false;
}

void UUIManagerSubsystem::ClearLayer(FGameplayTag LayerTag)
{
	if (UCommonActivatableWidgetContainerBase *Layer = FindLayer(LayerTag))
	{
		Layer->ClearWidgets();
	}
}

void UUIManagerSubsystem::ClearLayers(const FGameplayTagContainer &LayerTags)
{
	for (const FGameplayTag &LayerTag : LayerTags)
	{
		ClearLayer(LayerTag);
	}
}

UCommonActivatableWidget *UUIManagerSubsystem::GetActiveWidget(FGameplayTag LayerTag) const
{
	const UCommonActivatableWidgetContainerBase *Layer = FindLayer(LayerTag);
	return Layer ? Layer->GetActiveWidget() : nullptr;
}

bool UUIManagerSubsystem::IsLayerEmpty(FGameplayTag LayerTag) const
{
	const UCommonActivatableWidgetContainerBase *Layer = FindLayer(LayerTag);
	return !Layer || Layer->GetNumWidgets() == 0;
}

void UUIManagerSubsystem::HandleLayerChanged(FGameplayTag LayerTag, UCommonActivatableWidget *ActiveWidget)
{
	OnLayerChanged.Broadcast(LayerTag, ActiveWidget);
}
