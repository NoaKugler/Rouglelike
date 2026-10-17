#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "BaseCharacter.generated.h"

class UHealthComponent;

/**
 *  Shared base for every character that fights: the player and all enemies.
 *  Holds only what every combatant has. Player-only and enemy-only parts stay in the subclasses.
 */
UCLASS(Abstract)
class ROUGELIKE_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ABaseCharacter();

    FORCEINLINE UHealthComponent *GetHealthComponent() const { return HealthComponent; }

    // Ability System Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    UAbilitySystemComponent *AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    class UCoreAttributeSet *CoreAttributeSet;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
    EGameplayEffectReplicationMode AbilityReplicationMode = EGameplayEffectReplicationMode::Mixed;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHealthComponent> HealthComponent;

public:
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

protected:
    virtual void BeginPlay() override;

    virtual void PossessedBy(AController *NewController) override;

    virtual void OnRep_PlayerState() override;

    /** Bound to HealthComponent->OnDeath. Subclasses extend it and call Super. */
    UFUNCTION()
    virtual void HandleDeath();
};
