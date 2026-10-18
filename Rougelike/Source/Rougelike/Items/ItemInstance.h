// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "ItemInstance.generated.h"

class UItemDefinition;
/**
 *
 */
UCLASS(BlueprintType)
class ROUGELIKE_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	TSubclassOf<UItemDefinition> ItemDefinition;

	UPROPERTY(BlueprintReadOnly, Category = "Item")
	TMap<FGameplayTag, float> ItemStats;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item")
	float GetStatValue(const FGameplayTag &StatTag);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetStatValue(const FGameplayTag &StatTag, float NewValue);
};
