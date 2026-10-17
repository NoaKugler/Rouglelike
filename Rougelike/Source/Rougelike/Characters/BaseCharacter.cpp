#include "Characters/BaseCharacter.h"
#include "Combat/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySystem/Attributes/CoreAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySystem/RougelikeAbilitySystemComponent.h"

ABaseCharacter::ABaseCharacter()
{
    // Movement and animation tick in their own components; the actor itself has nothing to do per frame
    PrimaryActorTick.bCanEverTick = false;

    // The movement component turns every character. Each subclass picks how (cursor, movement direction).
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Same subobject name as before the move, so Blueprint overrides on it (Max Health) carry over
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

    // Ability System Component
    AbilitySystemComponent = CreateDefaultSubobject<URougelikeAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(AbilityReplicationMode);

    CoreAttributeSet = CreateDefaultSubobject<UCoreAttributeSet>(TEXT("CoreAttributeSet"));
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    HealthComponent->OnDeath.AddDynamic(this, &ABaseCharacter::HandleDeath);
}

void ABaseCharacter::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        ApplyDefaultAttributes();
        GrantAbilities(DefaultAbilities);
    }
}

void ABaseCharacter::ApplyDefaultAttributes()
{
    if (!DefaultAttributesEffect)
    {
        return;
    }

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    Context.AddSourceObject(this);

    const FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesEffect, 1.f, Context);
    if (Spec.IsValid())
    {
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }
}

void ABaseCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

UAbilitySystemComponent *ABaseCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

TArray<FGameplayAbilitySpecHandle> ABaseCharacter::GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant)
{
    if (!AbilitySystemComponent)
    {
        return TArray<FGameplayAbilitySpecHandle>();
    }

    TArray<FGameplayAbilitySpecHandle> GrantedHandles;

    for (TSubclassOf<UGameplayAbility> AbilityClass : AbilitiesToGrant)
    {
        if (AbilityClass)
        {
            GrantedHandles.Add(AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
                AbilityClass, 1, INDEX_NONE, this)));
        }
    }

    SendAbilitiesChangedEvent();
    return GrantedHandles;
}

void ABaseCharacter::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove)
{
    if (!AbilitySystemComponent)
    {
        return;
    }

    for (const FGameplayAbilitySpecHandle &AbilityHandle : AbilityHandlesToRemove)
    {
        AbilitySystemComponent->ClearAbility(AbilityHandle);
    }

    SendAbilitiesChangedEvent();
}

void ABaseCharacter::SendAbilitiesChangedEvent()
{
    FGameplayEventData EventData;
    EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Abilities.Changed"));
    EventData.Instigator = this;
    EventData.Target = this;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

FVector ABaseCharacter::GetAimLocation() const
{
    return FVector();
}

void ABaseCharacter::HandleDeath()
{
    // Every character stops moving when it dies; subclasses add what death means for them
    GetCharacterMovement()->DisableMovement();
}
