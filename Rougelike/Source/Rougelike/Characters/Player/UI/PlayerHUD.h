#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UTextBlock;
class UHealthComponent;

UCLASS(Abstract)
class ROUGELIKE_API UPlayerHUD : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // UFUNCTION is required to bind to a dynamic delegate
    UFUNCTION()
    void HandleHealthChanged(float CurrentHealth, float MaxHealth);

    UFUNCTION()
    void HandleDeath();

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HealthText;

    // Collapsed until the owning pawn dies
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DeathText;

private:
    TWeakObjectPtr<UHealthComponent> BoundHealthComponent;
};
