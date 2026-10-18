#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "BaseCharacter.generated.h"

class UHealthComponent;
class UOffenseAttributeSet;

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

    /** World point this character's attacks go toward. Base version: straight ahead. */
    UFUNCTION(BlueprintPure, Category = "Combat")
    virtual FVector GetAimLocation() const;

    // Ability System Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    UAbilitySystemComponent *AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    class UCoreAttributeSet *CoreAttributeSet;

    /** Attacker stats (FlatDamage, DamageMultiplier) that scale every hit this character deals */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    TObjectPtr<UOffenseAttributeSet> OffenseAttributeSet;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
    TSubclassOf<UGameplayEffect> DefaultAttributesEffect;

    UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
    EGameplayEffectReplicationMode AbilityReplicationMode = EGameplayEffectReplicationMode::Mixed;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHealthComponent> HealthComponent;

public:
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

    UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
    TArray<FGameplayAbilitySpecHandle> GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant);

    UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
    void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove);

    UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
    void SendAbilitiesChangedEvent();

protected:
    virtual void BeginPlay() override;

    virtual void PossessedBy(AController *NewController) override;

    virtual void OnRep_PlayerState() override;

    void ApplyDefaultAttributes();

    /** Bound to HealthComponent->OnDeath. Subclasses extend it and call Super. */
    UFUNCTION()
    virtual void HandleDeath();
};
