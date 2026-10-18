// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySystem/Effects/DamageEffect.h"
#include "RougelikeGameplayAbility.generated.h"

/**
 *
 */
UCLASS()
class ROUGELIKE_API URougelikeGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bShouldShowInAbilityBar = true;

	/** Spec of DamageEffect.Effect with its base damage filled in. The attacker's stats scale it when it hits. */
	UFUNCTION(BlueprintCallable, Category = "Damage")
	FGameplayEffectSpecHandle MakeDamageEffectSpec(const FDamageEffect &DamageEffect) const;
};
