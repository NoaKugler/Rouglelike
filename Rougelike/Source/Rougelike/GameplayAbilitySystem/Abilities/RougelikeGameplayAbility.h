// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySystem/Effects/DamageEffect.h"
#include "RougelikeGameplayAbility.generated.h"

class UTexture2D;

/**
 *
 */
UCLASS()
class ROUGELIKE_API URougelikeGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URougelikeGameplayAbility();

	/** Shown on the ability bar. The bar falls back to the class name when this is empty. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTexture2D> AbilityIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	bool bShouldShowInAbilityBar = true;

	/** Spec of DamageEffect.Effect with its base damage filled in. The attacker's stats scale it when it hits. */
	UFUNCTION(BlueprintCallable, Category = "Damage")
	FGameplayEffectSpecHandle MakeDamageEffectSpec(const FDamageEffect &DamageEffect) const;
};
