#include "Characters/Player/UI/PlayerHUD.h"
#include "Components/TextBlock.h"
#include "Combat/HealthComponent.h"
#include "GameFramework/Pawn.h"

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
        Health->OnHealthChanged.AddDynamic(this, &UPlayerHUD::HandleHealthChanged);
        Health->OnDeath.AddDynamic(this, &UPlayerHUD::HandleDeath);

        // The delegate only fires on change, so pull the starting value once
        HandleHealthChanged(Health->GetCurrentHealth(), Health->GetMaxHealth());
    }
}

void UPlayerHUD::NativeDestruct()
{
    if (BoundHealthComponent.IsValid())
    {
        BoundHealthComponent->OnHealthChanged.RemoveDynamic(this, &UPlayerHUD::HandleHealthChanged);
        BoundHealthComponent->OnDeath.RemoveDynamic(this, &UPlayerHUD::HandleDeath);
    }

    Super::NativeDestruct();
}

void UPlayerHUD::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
    HealthText->SetText(FText::AsNumber(FMath::RoundToInt(CurrentHealth)));
}

void UPlayerHUD::HandleDeath()
{
    // HitTestInvisible: visible, but never catches mouse clicks meant for the game
    DeathText->SetVisibility(ESlateVisibility::HitTestInvisible);
}
