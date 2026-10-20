// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySystem/Effects/DamageEffect.h"
#include "RougelikeGameplayAbility.generated.h"

class UTexture2D;

/** How a granted ability reacts to its input tag */
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	/** Activates once per press */
	OnInputPressed,

	/** Activates on press, then again whenever it can while the input stays down. Its cooldown sets the rate. */
	WhileInputHeld
};

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

	/** Only matters for abilities granted with an input tag */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	EAbilityActivationPolicy ActivationPolicy = EAbilityActivationPolicy::OnInputPressed;

	/** Spec of DamageEffect.Effect with its base damage filled in. The attacker's stats scale it when it hits. */
	UFUNCTION(BlueprintCallable, Category = "Damage")
	FGameplayEffectSpecHandle MakeDamageEffectSpec(const FDamageEffect &DamageEffect) const;
};
