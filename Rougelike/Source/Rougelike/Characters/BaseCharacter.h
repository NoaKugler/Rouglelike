#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySystem/RougelikeAbilitySystemComponent.h"
#include "BaseCharacter.generated.h"

class UHealthComponent;
class UOffenseAttributeSet;
class UCoreAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilitySystemReady);

/**
 *  Shared base for every character that fights: the player and all enemies.
 *  Holds only what every combatant has. Player-only and enemy-only parts stay in the subclasses.
 */
UCLASS(Abstract)
class ROUGELIKE_API ABaseCharacter : public ACharacter,
                                     public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ABaseCharacter();

    FORCEINLINE UHealthComponent *GetHealthComponent() const { return HealthComponent; }

    /** World point this character's attacks go toward. Base version: straight ahead. */
    UFUNCTION(BlueprintPure, Category = "Combat")
    virtual FVector GetAimLocation() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    TObjectPtr<URougelikeAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    TObjectPtr<UCoreAttributeSet> CoreAttributeSet;

    /** Attacker stats (FlatDamage, DamageMultiplier) that scale every hit this character deals */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
    TObjectPtr<UOffenseAttributeSet> OffenseAttributeSet;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
    TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

    UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHealthComponent> HealthComponent;

    bool bAbilitySystemReady = false;

public:
    /** Fires once, after the first possession gave the starting effects and abilities */
    UPROPERTY(BlueprintAssignable, Category = "AbilitySystem")
    FOnAbilitySystemReady OnAbilitySystemReady;

    /** Check this before binding: a listener that arrives late has already missed the broadcast */
    UFUNCTION(BlueprintPure, Category = "AbilitySystem")
    bool IsAbilitySystemReady() const { return bAbilitySystemReady; }

    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

    UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
    TArray<FGameplayAbilitySpecHandle> GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant);

    UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
    void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove);

protected:
    virtual void BeginPlay() override;

    virtual void PossessedBy(AController *NewController) override;

    void InitializeAbilitySystem();

    void ApplyStartupEffects();

    /** Bound to HealthComponent->OnDeath. Subclasses extend it and call Super. */
    UFUNCTION()
    virtual void HandleDeath();
};
