// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "OffenseAttributeSet.generated.h"

/**
 * Attacker stats. UDamageExecution reads them from whoever deals a hit, at the moment it lands.
 * Items, talents and buffs change them through GameplayEffect modifiers, optionally limited by tags.
 */
UCLASS()
class ROUGELIKE_API UOffenseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UOffenseAttributeSet();

	/** Added to the base damage of every hit and every damage tick */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BonusDamage)
	FGameplayAttributeData BonusDamage;
	ATTRIBUTE_ACCESSORS_BASIC(UOffenseAttributeSet, BonusDamage)

	/** All "increased damage" bonuses, summed: 0.2 means +20%. Damage is scaled by (1 + this) once. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_IncreasedDamage)
	FGameplayAttributeData IncreasedDamage;
	ATTRIBUTE_ACCESSORS_BASIC(UOffenseAttributeSet, IncreasedDamage)

public:
	// Replication Notification Functions
	UFUNCTION()
	virtual void OnRep_BonusDamage(const FGameplayAttributeData &OldBonusDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UOffenseAttributeSet, BonusDamage, OldBonusDamage); };

	UFUNCTION()
	virtual void OnRep_IncreasedDamage(const FGameplayAttributeData &OldIncreasedDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UOffenseAttributeSet, IncreasedDamage, OldIncreasedDamage); };

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
};
