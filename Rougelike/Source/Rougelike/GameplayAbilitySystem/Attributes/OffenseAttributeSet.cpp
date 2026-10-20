// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilitySystem/Attributes/OffenseAttributeSet.h"
#include "Net/UnrealNetwork.h"

UOffenseAttributeSet::UOffenseAttributeSet()
{
    BonusDamage = 0.f;
    // 0, not 1: increased bonuses are summed and applied once as (1 + sum)
    IncreasedDamage = 0.f;
}

void UOffenseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UOffenseAttributeSet, BonusDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UOffenseAttributeSet, IncreasedDamage, COND_None, REPNOTIFY_Always);
}
