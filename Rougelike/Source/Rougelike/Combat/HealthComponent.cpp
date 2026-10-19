#include "Combat/HealthComponent.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbilitySystem/Attributes/CoreAttributeSet.h"
#include "GameplayAbilitySystem/RougelikeAbilitySystemComponent.h"

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

    // Damage comes from the attribute set's report, not from Health going down: a lowered MaxHealth also lowers Health
    if (URougelikeAbilitySystemComponent *RougelikeASC = Cast<URougelikeAbilitySystemComponent>(AbilitySystemComponent))
    {
        RougelikeASC->OnDamageReceived.AddUObject(this, &UHealthComponent::HandleDamageReceived);
    }
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
    // Read both getters instead of Data.NewValue: this handler also fires for MaxHealth
    const float CurrentHealth = GetCurrentHealth();
    OnHealthChanged.Broadcast(CurrentHealth, GetMaxHealth());

    if (CurrentHealth <= 0.f && !bIsDead)
    {
        bIsDead = true;
        OnDeath.Broadcast();
    }
}

void UHealthComponent::HandleDamageReceived(const FDamageReport &Report)
{
    OnDamaged.Broadcast(Report);
}
