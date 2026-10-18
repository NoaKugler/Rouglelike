// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.generated.h"

class UItemDefinition;

/**
 * A quantity of one item type. Used wherever "N of this item" is needed (inventory slots, loot entries, costs).
 */
USTRUCT(BlueprintType)
struct ROUGELIKE_API FItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<UItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "1", UIMin = "1"))
	int32 Count = 1;
};
