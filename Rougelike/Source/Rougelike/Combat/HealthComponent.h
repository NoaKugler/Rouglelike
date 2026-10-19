#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySystem/Effects/DamageReport.h"
#include "HealthComponent.generated.h"

class UAbilitySystemComponent;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamaged, const FDamageReport &, Report);

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

    /** Broadcast once per damaging hit. Health lost any other way (a lowered max, a cost) doesn't count. */
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

    // Bound to the ASC's native OnDamageReceived
    void HandleDamageReceived(const FDamageReport &Report);

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    bool bIsDead = false;
};
