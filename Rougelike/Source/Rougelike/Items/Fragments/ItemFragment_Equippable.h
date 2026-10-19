// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemFragment.h"
#include "ItemFragment_Equippable.generated.h"

class UEquipmentDefinition;

/**
 *
 */
UCLASS()
class ROUGELIKE_API UItemFragment_Equippable : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;
};
