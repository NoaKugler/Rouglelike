#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UHealthComponent;

/**
 *  Shared base for every character that fights: the player and all enemies.
 *  Holds only what every combatant has. Player-only and enemy-only parts stay in the subclasses.
 */
UCLASS(Abstract)
class ROUGELIKE_API ABaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseCharacter();

    FORCEINLINE UHealthComponent *GetHealthComponent() const { return HealthComponent; }

protected:
    virtual void BeginPlay() override;

    /** Bound to HealthComponent->OnDeath. Subclasses extend it and call Super. */
    UFUNCTION()
    virtual void HandleDeath();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHealthComponent> HealthComponent;
};
