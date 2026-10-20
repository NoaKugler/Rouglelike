// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilitySystem/Effects/DamageExecution.h"
#include "GameplayAbilitySystem/Attributes/CoreAttributeSet.h"
#include "GameplayAbilitySystem/Attributes/OffenseAttributeSet.h"
#include "GameplayAbilitySystem/RougelikeGameplayTags.h"

namespace
{
    // Attacker stats the formula reads. Not snapshotted: read when the hit lands, not when the spec was made.
    struct FDamageCaptures
    {
        FGameplayEffectAttributeCaptureDefinition BonusDamage;
        FGameplayEffectAttributeCaptureDefinition IncreasedDamage;

        FDamageCaptures()
            : BonusDamage(UOffenseAttributeSet::GetBonusDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, false)
            , IncreasedDamage(UOffenseAttributeSet::GetIncreasedDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, false)
        {
        }
    };

    const FDamageCaptures &DamageCaptures()
    {
        static const FDamageCaptures Captures;
        return Captures;
    }
}

UDamageExecution::UDamageExecution()
{
    // Links these to every spec of a GE using this execution, so Execute can read them
    RelevantAttributesToCapture.Add(DamageCaptures().BonusDamage);
    RelevantAttributesToCapture.Add(DamageCaptures().IncreasedDamage);
}

void UDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams, FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
    const FGameplayEffectSpec &Spec = ExecutionParams.GetOwningSpec();

    // Tagged modifiers ("projectile only", "vs burning") are checked against these two lists
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // 1. Base: set by whoever made the spec. GAS logs a warning if it's missing.
    const float BaseDamage = Spec.GetSetByCallerMagnitude(RougelikeGameplayTags::SetByCaller_Damage);

    // 2. Attacker stats. Both stay 0 if the attacker has no offense set.
    float BonusDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageCaptures().BonusDamage, EvaluationParameters, BonusDamage);

    float IncreasedDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageCaptures().IncreasedDamage, EvaluationParameters, IncreasedDamage);

    // Negative stats can't turn a hit into a heal
    const float Damage = FMath::Max(0.f, (BaseDamage + BonusDamage) * (1.f + IncreasedDamage));

    // 3. Output: the victim's CoreAttributeSet turns this into Health loss
    if (Damage > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCoreAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::AddBase, Damage));
    }
}
