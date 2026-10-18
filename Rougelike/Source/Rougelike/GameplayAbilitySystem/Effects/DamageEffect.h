// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"
#include "DamageEffect.generated.h"

class UGameplayEffect;

/**
 * A gameplay effect plus the base damage it deals.
 * The base damage goes into the spec as SetByCaller.Damage; UDamageExecution scales it with the attacker's stats.
 */
USTRUCT(BlueprintType)
struct FDamageEffect
{
	GENERATED_BODY()

	/** Usually GE_Damage, or a periodic one like GE_Burn. Effects without the damage execution ignore BaseDamage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> Effect;

	/** Per hit, or per tick for periodic effects. A curve table can scale it with ability level later. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	FScalableFloat BaseDamage;
};
