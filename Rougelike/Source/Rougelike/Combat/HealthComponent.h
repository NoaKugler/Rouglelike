#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UAbilitySystemComponent;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamaged, float, DamageAmount);

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class ROUGELIKE_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetCurrentHealth() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsDead() const { return bIsDead; }

    /** Broadcast after every change, with the new values */
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChanged OnHealthChanged;

    /** Broadcast when Health drops, with the amount actually lost (after clamping) */
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDamaged OnDamaged;

    /** Broadcast exactly once, when health first reaches zero */
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDeath OnDeath;

protected:
    virtual void InitializeComponent() override;

private:
    // Bound to both Health and MaxHealth. Native delegate, so no UFUNCTION.
    void HandleHealthAttributeChanged(const FOnAttributeChangeData &Data);

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    bool bIsDead = false;
};
