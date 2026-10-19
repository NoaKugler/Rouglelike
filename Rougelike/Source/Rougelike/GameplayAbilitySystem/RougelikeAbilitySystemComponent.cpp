// Fill out your copyright notice in the Description page of Project Settings.
#include "GameplayAbilitySystem/RougelikeAbilitySystemComponent.h"
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
