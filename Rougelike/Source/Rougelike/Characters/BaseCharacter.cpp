#include "Characters/BaseCharacter.h"
#include "Combat/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySystem/Attributes/CoreAttributeSet.h"
#include "GameplayAbilitySystem/Attributes/OffenseAttributeSet.h"
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

    CoreAttributeSet = CreateDefaultSubobject<UCoreAttributeSet>(TEXT("CoreAttributeSet"));

    // Every character can deal damage, so enemies scale their hits the same way the player does
    OffenseAttributeSet = CreateDefaultSubobject<UOffenseAttributeSet>(TEXT("OffenseAttributeSet"));
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    HealthComponent->OnDeath.AddDynamic(this, &ABaseCharacter::HandleDeath);
}

void ABaseCharacter::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);
    InitializeAbilitySystem();
}

void ABaseCharacter::InitializeAbilitySystem()
{
    // Every possession: the actor info stores the controller, so a new controller has to be picked up
    AbilitySystemComponent->InitAbilityActorInfo(this, this);

    // Only the first time: a second possession must not stack the starting effects or give the abilities twice
    if (bAbilitySystemReady)
    {
        return;
    }

    ApplyStartupEffects();
    GrantAbilities(DefaultAbilities);

    bAbilitySystemReady = true;
    OnAbilitySystemReady.Broadcast();
}

void ABaseCharacter::ApplyStartupEffects()
{
    for (const TSubclassOf<UGameplayEffect> &EffectClass : StartupEffects)
    {
        // An empty slot in the array is easy to leave behind in the editor
        if (!EffectClass)
        {
            continue;
        }

        // One context per effect, so specs never share (and mutate) the same context object
        FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
        Context.AddSourceObject(this);

        const FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, Context);
        if (Spec.IsValid())
        {
            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
        }
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
    // Straight ahead at the character's own height, far enough that a direction computed from it is stable
    constexpr float AimDistance = 1000.f;
    return GetActorLocation() + GetActorForwardVector() * AimDistance;
}

void ABaseCharacter::HandleDeath()
{
    // Every character stops moving when it dies; subclasses add what death means for them
    GetCharacterMovement()->DisableMovement();
}
