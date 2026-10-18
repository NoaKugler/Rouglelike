// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilitySystem/Attributes/CoreAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UCoreAttributeSet::UCoreAttributeSet()
{
    Health = 100.f;
    MaxHealth = 100.f;
    Mana = 100.f;
    MaxMana = 100.f;
    Damage = 0.f;
    IncomingDamage = 0.f;
}

void UCoreAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Damage, COND_None, REPNOTIFY_Always);
}

void UCoreAttributeSet::PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UCoreAttributeSet::PreAttributeBaseChange(const FGameplayAttribute &Attribute, float &NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UCoreAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        // Consume the meta value right away so the next hit starts from zero
        const float DamageDone = GetIncomingDamage();
        SetIncomingDamage(0.f);

        if (DamageDone > 0.f)
        {
            // Goes through PreAttributeBaseChange, so Health stays clamped at 0
            SetHealth(GetHealth() - DamageDone);
        }
    }
}

void UCoreAttributeSet::ClampAttribute(const FGameplayAttribute &Attribute, float &NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    else if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
    }
    else if (Attribute == GetMaxHealthAttribute() || Attribute == GetMaxManaAttribute())
    {
        // A max of 0 would divide by zero in the health bar
        NewValue = FMath::Max(NewValue, 1.f);
    }
}
