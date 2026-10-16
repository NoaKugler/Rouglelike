#include "Player/UI/PlayerHUD.h"
#include "Components/TextBlock.h"
#include "Player/RougelikeCharacter.h"

void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (ARougelikeCharacter *Character = Cast<ARougelikeCharacter>(GetOwningPlayerPawn()))
    {
        BoundCharacter = Character;
        Character->OnHealthChanged.AddDynamic(this, &UPlayerHUD::HandleHealthChanged);

        // The delegate only fires on change, so pull the starting value once
        HandleHealthChanged(Character->GetCurrentHealth());
    }
}

void UPlayerHUD::NativeDestruct()
{
    if (BoundCharacter.IsValid())
    {
        BoundCharacter->OnHealthChanged.RemoveDynamic(this, &UPlayerHUD::HandleHealthChanged);
    }

    Super::NativeDestruct();
}

void UPlayerHUD::HandleHealthChanged(float NewHealth)
{
    HealthText->SetText(FText::AsNumber(FMath::RoundToInt(NewHealth)));
}
