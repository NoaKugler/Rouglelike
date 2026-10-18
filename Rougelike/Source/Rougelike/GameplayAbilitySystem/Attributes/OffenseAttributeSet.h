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

	/** Added to the base damage of every hit and every damage tick, before multipliers */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_FlatDamage)
	FGameplayAttributeData FlatDamage;
	ATTRIBUTE_ACCESSORS_BASIC(UOffenseAttributeSet, FlatDamage)

	/** Scales damage after flat damage is added. 1 means unchanged. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_DamageMultiplier)
	FGameplayAttributeData DamageMultiplier;
	ATTRIBUTE_ACCESSORS_BASIC(UOffenseAttributeSet, DamageMultiplier)

public:
	// Replication Notification Functions
	UFUNCTION()
	virtual void OnRep_FlatDamage(const FGameplayAttributeData &OldFlatDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UOffenseAttributeSet, FlatDamage, OldFlatDamage); };

	UFUNCTION()
	virtual void OnRep_DamageMultiplier(const FGameplayAttributeData &OldDamageMultiplier) { GAMEPLAYATTRIBUTE_REPNOTIFY(UOffenseAttributeSet, DamageMultiplier, OldDamageMultiplier); };

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
};
