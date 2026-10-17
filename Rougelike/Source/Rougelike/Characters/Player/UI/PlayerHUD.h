#pragma once

#include "CoreMinimal.h"
#include "UI/UIActivatableWidget.h"
#include "PlayerHUD.generated.h"

class UTextBlock;
class UHealthComponent;

UCLASS(Abstract)
class ROUGELIKE_API UPlayerHUD : public UUIActivatableWidget
{
    GENERATED_BODY()

public:
    UPlayerHUD(const FObjectInitializer &ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // UFUNCTION is required to bind to a dynamic delegate
    UFUNCTION()
    void HandleDeath();

    // Collapsed until the owning pawn dies
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DeathText;

private:
    TWeakObjectPtr<UHealthComponent> BoundHealthComponent;
};
