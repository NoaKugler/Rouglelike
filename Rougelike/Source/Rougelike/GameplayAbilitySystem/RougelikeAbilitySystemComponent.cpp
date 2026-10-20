// Fill out your copyright notice in the Description page of Project Settings.
#include "GameplayAbilitySystem/RougelikeAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Abilities/RougelikeGameplayAbility.h"
#include "TimerManager.h"
#include "Engine/World.h"

void URougelikeAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec &AbilitySpec)
{
    Super::OnGiveAbility(AbilitySpec);
    QueueAbilitiesChanged();
}

void URougelikeAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec &AbilitySpec)
{
    Super::OnRemoveAbility(AbilitySpec);
    QueueAbilitiesChanged();
}

void URougelikeAbilitySystemComponent::QueueAbilitiesChanged()
{
    // OnRemoveAbility runs while the spec is still in the list, and a weapon swap removes and gives in a row.
    // Waiting a tick means listeners read the finished list, once.
    UWorld *World = GetWorld();
    if (bAbilitiesChangedQueued || !World)
    {
        return;
    }

    bAbilitiesChangedQueued = true;
    World->GetTimerManager().SetTimerForNextTick(this, &URougelikeAbilitySystemComponent::BroadcastAbilitiesChanged);
}

void URougelikeAbilitySystemComponent::BroadcastAbilitiesChanged()
{
    bAbilitiesChangedQueued = false;
    OnAbilitiesChanged.Broadcast();
}

void URougelikeAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag &InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    // Activating can give or remove abilities; the lock delays those list changes until the loop is done
    ABILITYLIST_SCOPE_LOCK();
    for (FGameplayAbilitySpec &Spec : ActivatableAbilities.Items)
    {
        if (!Spec.Ability || !Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
        {
            continue;
        }

        Spec.InputPressed = true;
        if (Spec.IsActive())
        {
            AbilitySpecInputPressed(Spec);
            InvokeInputEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec);
        }
        else
        {
            TryActivateAbility(Spec.Handle);
        }
    }
}

void URougelikeAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag &InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    ABILITYLIST_SCOPE_LOCK();
    for (FGameplayAbilitySpec &Spec : ActivatableAbilities.Items)
    {
        if (!Spec.Ability || Spec.IsActive() || !Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
        {
            continue;
        }

        // Spec.Ability is the class default object, so this reads the Blueprint's setting
        const URougelikeGameplayAbility *Ability = Cast<URougelikeGameplayAbility>(Spec.Ability);
        if (Ability && Ability->ActivationPolicy == EAbilityActivationPolicy::WhileInputHeld)
        {
            // Fails quietly while on cooldown or short of cost, so the cooldown sets the repeat rate
            TryActivateAbility(Spec.Handle);
        }
    }
}

void URougelikeAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag &InputTag)
{
    if (!InputTag.IsValid())
    {
        return;
    }

    ABILITYLIST_SCOPE_LOCK();
    for (FGameplayAbilitySpec &Spec : ActivatableAbilities.Items)
    {
        if (!Spec.Ability || !Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
        {
            continue;
        }

        Spec.InputPressed = false;
        if (Spec.IsActive())
        {
            AbilitySpecInputReleased(Spec);
            InvokeInputEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec);
        }
    }
}

void URougelikeAbilitySystemComponent::InvokeInputEvent(EAbilityGenericReplicatedEvent::Type EventType, const FGameplayAbilitySpec &Spec)
{
    // The tasks listen per activation, identified by its prediction key. Same lookup as the engine's own input handling.
    const TArray<UGameplayAbility *> Instances = Spec.GetAbilityInstances();
    const FPredictionKey ActivationKey = Instances.IsEmpty() ? FPredictionKey() : Instances.Last()->GetCurrentActivationInfo().GetActivationPredictionKey();
    InvokeReplicatedEvent(EventType, Spec.Handle, ActivationKey);
}
