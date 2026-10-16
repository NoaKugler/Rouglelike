#include "Combat/HealthComponent.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = true;
}

void UHealthComponent::InitializeComponent()
{
    Super::InitializeComponent();

    // Runs after Blueprint defaults are applied and before any BeginPlay
    CurrentHealth = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
}

void UHealthComponent::HandleTakeAnyDamage(AActor *DamagedActor, float Damage, const UDamageType *DamageType, AController *InstigatedBy, AActor *DamageCauser)
{
    // Negative damage also arrives here. Healing gets its own function when it's needed.
    if (Damage <= 0.f || bIsDead)
    {
        return;
    }

    CurrentHealth = FMath::Max(CurrentHealth - Damage, 0.f);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
        bIsDead = true;
        OnDeath.Broadcast();
    }
}
