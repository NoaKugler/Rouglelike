// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CoreAttributeSet.generated.h"

// Generates the property getter, value getter, setter and initter for an attribute.
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class ROUGELIKE_API UCoreAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCoreAttributeSet();

	// Health Attributes
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCoreAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCoreAttributeSet, MaxHealth)

	// Mana Attributes
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UCoreAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UCoreAttributeSet, MaxMana)

public:
	// Replication Notification Functions
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData &OldHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Health, OldHealth); };

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData &OldMaxHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, MaxHealth, OldMaxHealth); };

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData &OldMana) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Mana, OldMana); };

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData &OldMaxMana) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, MaxMana, OldMaxMana); };

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
};
