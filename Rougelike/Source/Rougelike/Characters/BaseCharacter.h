#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySystem/RougelikeAbilitySystemComponent.h"
#include "GameplayAbilitySystem/GrantSet.h"
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
    /** Instant: sets the starting stats once. It can't be revoked, so it isn't part of the grant set. */
    UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
    TSubclassOf<UGameplayEffect> InitAttributesEffect;

    /** What this character has from the start, given once on the first possession. The character is its source. */
    UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
    FGrantSet StartingGrants;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHealthComponent> HealthComponent;

    bool bAbilitySystemReady = false;

    // Never revoked today, but whoever gives keeps the receipt
    FGrantedHandles StartingGrantHandles;

public:
    /** Fires once, after the first possession gave the starting effects and abilities */
    UPROPERTY(BlueprintAssignable, Category = "AbilitySystem")
    FOnAbilitySystemReady OnAbilitySystemReady;

    /** Check this before binding: a listener that arrives late has already missed the broadcast */
    UFUNCTION(BlueprintPure, Category = "AbilitySystem")
    bool IsAbilitySystemReady() const { return bAbilitySystemReady; }

    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

    // TEMPORARY (F1 test): proves the receipt removes exactly what it gave. Delete after testing.
    UFUNCTION(Exec)
    void DebugRevokeStartingGrants();

    UFUNCTION(Exec)
    void DebugGiveStartingGrants();

    // Old weapon path: only WeaponsManagerComponent uses these two. Deleted in F4, when weapons go through FGrantSet.
    UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
    TArray<FGameplayAbilitySpecHandle> GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant);

    UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
    void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove);

protected:
    virtual void BeginPlay() override;

    virtual void PossessedBy(AController *NewController) override;

    void InitializeAbilitySystem();

    void ApplyInitAttributes();

    /** Bound to HealthComponent->OnDeath. Subclasses extend it and call Super. */
    UFUNCTION()
    virtual void HandleDeath();
};
