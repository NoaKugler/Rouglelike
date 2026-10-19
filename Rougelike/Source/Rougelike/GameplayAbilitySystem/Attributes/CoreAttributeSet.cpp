// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilitySystem/Attributes/CoreAttributeSet.h"
#include "GameplayAbilitySystem/RougelikeAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UCoreAttributeSet::UCoreAttributeSet()
{
    Health = 100.f;
    MaxHealth = 100.f;
    Mana = 100.f;
    MaxMana = 100.f;
    IncomingDamage = 0.f;
}

void UCoreAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UCoreAttributeSet::PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UCoreAttributeSet::PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    // Infinite GEs (an item's +MaxHealth) never execute, so this is the only hook that sees every max change
    if (Attribute == GetMaxHealthAttribute())
    {
        // A dead character stays dead when its max rises
        if (GetHealth() > 0.f)
        {
            FollowMaxChange(GetHealthAttribute(), OldValue, NewValue);
        }
    }
    else if (Attribute == GetMaxManaAttribute())
    {
        FollowMaxChange(GetManaAttribute(), OldValue, NewValue);
    }
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
            const float HealthBefore = GetHealth();
            SetHealth(HealthBefore - DamageDone);
            SendDamageReport(Data, DamageDone, HealthBefore - GetHealth());
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

void UCoreAttributeSet::FollowMaxChange(const FGameplayAttribute &CurrentAttribute, float OldMax, float NewMax)
{
    UAbilitySystemComponent *ASC = GetOwningAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    // Raise: 30/100 -> 80/150, no full refill. Drop: only clamp, 80/150 -> 80/100.
    const float Current = CurrentAttribute.GetNumericValue(this);
    const float NewCurrent = NewMax > OldMax ? Current + (NewMax - OldMax) : FMath::Min(Current, NewMax);

    // Setting an unchanged value would still fire every change listener
    if (!FMath::IsNearlyEqual(Current, NewCurrent))
    {
        // What the generated SetHealth/SetMana do: change the base value, so PreAttributeBaseChange clamps it like any other change
        ASC->SetNumericAttributeBase(CurrentAttribute, NewCurrent);
    }
}

void UCoreAttributeSet::SendDamageReport(const FGameplayEffectModCallbackData &Data, float Damage, float HealthLost) const
{
    URougelikeAbilitySystemComponent *VictimASC = Cast<URougelikeAbilitySystemComponent>(&Data.Target);
    if (!VictimASC)
    {
        return;
    }

    // Filled when the spec was made: instigator = the attacker's ASC owner, causer = its avatar,
    // source object = the SourceObject of the ability spec that made it
    const FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();

    FDamageReport Report;
    Report.Instigator = Context.GetInstigator();
    Report.DamageCauser = Context.GetEffectCauser();
    Report.SourceObject = Context.GetSourceObject();
    Report.Damage = Damage;
    Report.HealthLost = HealthLost;
    Report.bKillingBlow = HealthLost > 0.f && GetHealth() <= 0.f;

    VictimASC->OnDamageReceived.Broadcast(Report);
}
