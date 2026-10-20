// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilitySystem/Abilities/RougelikeGameplayAbility.h"
#include "GameplayAbilitySystem/RougelikeGameplayTags.h"
#include "Items/ItemInstance.h"
#include "Equipment/EquipmentComponent.h"
#include "Equipment/EquipmentInstance.h"
#include "GameplayEffect.h"

URougelikeGameplayAbility::URougelikeGameplayAbility()
{
	// 5.8 defaults to InstancedPerExecution: a new ability object for every activation.
	// One instance per actor is cheaper, but its variables survive between activations, so reset them on activate.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FGameplayEffectSpecHandle URougelikeGameplayAbility::MakeDamageEffectSpec(const FDamageEffect &DamageEffect) const
{
	if (!DamageEffect.Effect)
	{
		return FGameplayEffectSpecHandle();
	}

	// Made by the ability, so the spec carries the ability's tags and level
	const float Level = GetAbilityLevel();
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffect.Effect, Level);
	if (Spec.IsValid())
	{
		// Has to come after: MakeOutgoingGameplayEffectSpec replaces the spec's SetByCaller values while creating it
		Spec.Data->SetSetByCallerMagnitude(RougelikeGameplayTags::SetByCaller_Damage, DamageEffect.BaseDamage.GetValueAtLevel(Level));
	}
	return Spec;
}

UItemInstance *URougelikeGameplayAbility::GetSourceItem() const
{
	return Cast<UItemInstance>(GetCurrentSourceObject());
}

AActor *URougelikeGameplayAbility::GetSourceEquipmentActor() const
{
	// Asks the equipment rather than caching the actor: the record is the one place that knows what is equipped
	const AActor *Avatar = GetAvatarActorFromActorInfo();
	const UEquipmentComponent *Equipment = Avatar ? Avatar->FindComponentByClass<UEquipmentComponent>() : nullptr;
	const UEquipmentInstance *Record = Equipment ? Equipment->FindEquipmentForItem(GetSourceItem()) : nullptr;
	return Record ? Record->GetSpawnedActor() : nullptr;
}
