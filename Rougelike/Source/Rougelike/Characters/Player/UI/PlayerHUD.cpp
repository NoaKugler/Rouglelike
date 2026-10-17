#include "Characters/Player/UI/PlayerHUD.h"
#include "Components/TextBlock.h"
#include "Combat/HealthComponent.h"
#include "GameFramework/Pawn.h"

UPlayerHUD::UPlayerHUD(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    // While the HUD is the top widget, input belongs to the game. Popping the last menu lands back here.
    InputMode = EUIInputMode::Game;
}

void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    DeathText->SetVisibility(ESlateVisibility::Collapsed);

    // Works for any pawn with a health component
    const APawn *Pawn = GetOwningPlayerPawn();
    UHealthComponent *Health = Pawn ? Pawn->FindComponentByClass<UHealthComponent>() : nullptr;
    if (Health)
    {
        BoundHealthComponent = Health;
        Health->OnDeath.AddDynamic(this, &UPlayerHUD::HandleDeath);
    }
}

void UPlayerHUD::NativeDestruct()
{
    if (BoundHealthComponent.IsValid())
    {
        BoundHealthComponent->OnDeath.RemoveDynamic(this, &UPlayerHUD::HandleDeath);
    }

    Super::NativeDestruct();
}

void UPlayerHUD::HandleDeath()
{
    // HitTestInvisible: visible, but never catches mouse clicks meant for the game
    DeathText->SetVisibility(ESlateVisibility::HitTestInvisible);
}
