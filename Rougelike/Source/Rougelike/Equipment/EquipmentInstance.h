// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySystem/GrantSet.h"
#include "EquipmentInstance.generated.h"

class AActor;
class UItemInstance;
class UEquipmentDefinition;

/**
 * Runtime record of one equipped item. Created and owned by UEquipmentComponent, which does all the work;
 * the record only remembers what equipping did, so unequipping can undo exactly that.
 */
UCLASS(BlueprintType)
class ROUGELIKE_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UItemInstance *GetItem() const { return Item; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TSubclassOf<UEquipmentDefinition> GetDefinition() const { return Definition; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	FGameplayTag GetSlot() const { return Slot; }

	/** Null for equipment without an actor */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	AActor *GetSpawnedActor() const { return SpawnedActor; }

private:
	friend class UEquipmentComponent;

	// The inventory owns the item; the record only points at it
	UPROPERTY()
	TObjectPtr<UItemInstance> Item;

	UPROPERTY()
	TSubclassOf<UEquipmentDefinition> Definition;

	UPROPERTY()
	FGameplayTag Slot;

	UPROPERTY()
	TObjectPtr<AActor> SpawnedActor;

	// What this equip granted. Plain C++ (no UPROPERTY): it holds only handles and a weak pointer.
	FGrantedHandles GrantedHandles;
};
