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
        FGameplayEffectAttributeCaptureDefinition FlatDamage;
        FGameplayEffectAttributeCaptureDefinition DamageMultiplier;

        FDamageCaptures()
            : FlatDamage(UOffenseAttributeSet::GetFlatDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, false)
            , DamageMultiplier(UOffenseAttributeSet::GetDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, false)
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
    RelevantAttributesToCapture.Add(DamageCaptures().FlatDamage);
    RelevantAttributesToCapture.Add(DamageCaptures().DamageMultiplier);
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

    // 2. Attacker stats. The defaults stay if the attacker has no offense set.
    float FlatDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageCaptures().FlatDamage, EvaluationParameters, FlatDamage);

    float DamageMultiplier = 1.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageCaptures().DamageMultiplier, EvaluationParameters, DamageMultiplier);

    // Negative stats can't turn a hit into a heal
    const float Damage = FMath::Max(0.f, (BaseDamage + FlatDamage) * DamageMultiplier);

    // 3. Output: the victim's CoreAttributeSet turns this into Health loss
    if (Damage > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCoreAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::AddBase, Damage));
    }
}
