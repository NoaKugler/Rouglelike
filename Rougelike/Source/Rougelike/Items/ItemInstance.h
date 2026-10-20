// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Items/ItemTypes.h"
#include "ItemInstance.generated.h"

class UItemDefinition;
/**
 * One item while it sits in an inventory. Its whole state is the definition plus the stats map:
 * anything it needs to remember must be a stat, or dropping the item would lose it.
 */
UCLASS(BlueprintType)
class ROUGELIKE_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * The only way item instances are made. Outer: the inventory that will hold it.
	 * InItemStats are the item's saved state (e.g. uses left); later, rolls happen here only for stats missing from it.
	 */
	static UItemInstance *CreateItem(UObject *Outer, TSubclassOf<UItemDefinition> InItemDefinition, const TMap<FGameplayTag, float> &InItemStats = {});

	/** This item as plain data, e.g. for a dropped pickup. The reverse of CreateItem. */
	FStackInfo ToStackInfo(int32 Count) const;

	UPROPERTY(BlueprintReadOnly, Category = "Item")
	TSubclassOf<UItemDefinition> ItemDefinition;

	UPROPERTY(BlueprintReadOnly, Category = "Item")
	TMap<FGameplayTag, float> ItemStats;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item", meta = (Categories = "Item.Stat"))
	float GetStatValue(const FGameplayTag &StatTag);

	UFUNCTION(BlueprintCallable, Category = "Item", meta = (Categories = "Item.Stat"))
	void SetStatValue(const FGameplayTag &StatTag, float NewValue);

	UFUNCTION(BlueprintPure, Category = "Item", meta = (Categories = "Item.Stat"))
	bool HasStat(const FGameplayTag &StatTag) const { return ItemStats.Contains(StatTag); }

	/** The stat's name from the stat definitions, or its tag if it has none (so missing data shows) */
	UFUNCTION(BlueprintPure, Category = "Item|Stats", meta = (Categories = "Item.Stat"))
	FText GetStatDisplayName(FGameplayTag StatTag) const;

	/** The value, formatted (percent if the definition says so), plus the roll range if the item rolls this stat */
	UFUNCTION(BlueprintPure, Category = "Item|Stats", meta = (Categories = "Item.Stat"))
	FText GetStatValueText(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DeterminesOutputType = "FragmentClass"), Category = "Item")
	const UItemFragment *FindFragmentByClass(const TSubclassOf<UItemFragment> FragmentClass);

private:
	// Only CreateItem calls this: nothing else may hand out a half-built item
	void Initialize(TSubclassOf<UItemDefinition> InItemDefinition, const TMap<FGameplayTag, float> &InItemStats);
};
