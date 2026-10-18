#include "Combat/HealthComponent.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbilitySystem/Attributes/CoreAttributeSet.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = true;
}

void UHealthComponent::InitializeComponent()
{
    Super::InitializeComponent();

    // Runs before the owner is possessed, so we're already listening when the starting attributes get applied
    AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
    if (!AbilitySystemComponent)
    {
        return;
    }

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCoreAttributeSet::GetHealthAttribute()).AddUObject(this, &UHealthComponent::HandleHealthAttributeChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCoreAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UHealthComponent::HandleHealthAttributeChanged);
}

float UHealthComponent::GetCurrentHealth() const
{
    return AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(UCoreAttributeSet::GetHealthAttribute()) : 0.f;
}

float UHealthComponent::GetMaxHealth() const
{
    return AbilitySystemComponent ? AbilitySystemComponent->GetNumericAttribute(UCoreAttributeSet::GetMaxHealthAttribute()) : 0.f;
}

void UHealthComponent::HandleHealthAttributeChanged(const FOnAttributeChangeData &Data)
{
    if (Data.Attribute == UCoreAttributeSet::GetHealthAttribute() && Data.NewValue < Data.OldValue)
    {
        OnDamaged.Broadcast(Data.OldValue - Data.NewValue);
    }

    // Read both getters instead of Data.NewValue: this handler also fires for MaxHealth
    const float CurrentHealth = GetCurrentHealth();
    OnHealthChanged.Broadcast(CurrentHealth, GetMaxHealth());

    if (CurrentHealth <= 0.f && !bIsDead)
    {
        bIsDead = true;
        OnDeath.Broadcast();
    }
}
