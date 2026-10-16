#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UTextBlock;
class ARougelikeCharacter;

UCLASS(Abstract)
class ROUGELIKE_API UPlayerHUD : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // UFUNCTION is required to bind to a dynamic delegate
    UFUNCTION()
    void HandleHealthChanged(float NewHealth);

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HealthText;

private:
    TWeakObjectPtr<ARougelikeCharacter> BoundCharacter;
};
