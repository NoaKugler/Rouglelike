// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CoreAttributeSet.generated.h"

// Generates the property getter, value getter, setter and initter for an attribute.
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)               \
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

	// Old pipeline: GE_Damage reads this as the attacker's damage. Remove once no asset uses it.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UCoreAttributeSet, Damage)

	// Meta attribute: UDamageExecution writes the final damage here, PostGameplayEffectExecute turns it into Health loss.
	// Only holds a value during one execution, so it isn't replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UCoreAttributeSet, IncomingDamage)

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

	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData &OldDamage) { GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Damage, OldDamage); };

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute &Attribute, float &NewValue) const override;

	/** Where all damage lands: converts IncomingDamage into Health loss */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data) override;

private:
	// Shared by both Pre hooks so base and current value always obey the same limits
	void ClampAttribute(const FGameplayAttribute &Attribute, float &NewValue) const;
};
