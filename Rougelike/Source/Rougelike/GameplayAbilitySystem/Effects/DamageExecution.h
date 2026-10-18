// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageExecution.generated.h"

/**
 * The damage formula, selected as the execution of every damage GE (GE_Damage, GE_Burn).
 * Runs on the victim for each hit and each periodic tick:
 * (SetByCaller.Damage + attacker's FlatDamage) * attacker's DamageMultiplier, written to the victim's IncomingDamage.
 */
UCLASS()
class ROUGELIKE_API UDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams, FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;
};
