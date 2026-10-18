// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilitySystem/Attributes/OffenseAttributeSet.h"
#include "Net/UnrealNetwork.h"

UOffenseAttributeSet::UOffenseAttributeSet()
{
    FlatDamage = 0.f;
    DamageMultiplier = 1.f;
}

void UOffenseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UOffenseAttributeSet, FlatDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UOffenseAttributeSet, DamageMultiplier, COND_None, REPNOTIFY_Always);
}
