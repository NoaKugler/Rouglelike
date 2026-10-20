// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySystem/GrantSet.h"
#include "EquipmentDefinition.generated.h"

/**
 * What equipping an item does. Several items can share one definition.
 */
UCLASS(Blueprintable, BlueprintType, Abstract, Const)
class ROUGELIKE_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	/** Spawned and attached while equipped. Leave empty for equipment with no actor (rings, amulets). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> EquipmentActorClass;

	/** Socket on the owner's character mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName EquipmentSocketName;

	/** Fits any slot whose tag is this tag or a child of it: Equipment.Slot.Ring fits Ring.Left and Ring.Right */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	FGameplayTag AllowedSlot;

	/** Given while equipped, with the item instance as source: a weapon's attack ability, an item's passive effects */
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FGrantSet Grants;
};
