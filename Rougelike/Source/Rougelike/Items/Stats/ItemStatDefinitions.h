#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ItemStatDefinitions.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UItemInstance;
struct FGrantedHandles;

USTRUCT()
struct FItemStatDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	FText DisplayName;

	/** Shown as a percentage: 0.2 → 20% */
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	bool bPercent = false;

	/**
	 * Applied while the item is equipped. Must be Infinite with Stacking Type None, and read its value as SetByCaller
	 * under this stat's own tag. Empty for stats that only describe the item (durability).
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	TSubclassOf<UGameplayEffect> EquippedEffect;
};

/** The meaning of every item stat, in one place */
UCLASS()
class ROUGELIKE_API UItemStatDefinitions : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The asset set in Project Settings > Items, or null */
	static const UItemStatDefinitions *Get();

	const FItemStatDefinition *Find(const FGameplayTag &Stat) const { return Stats.Find(Stat); }

	/** One effect per stat that has one, with the item as source, kept on the receipt */
	void ApplyEquippedStats(const UItemInstance &Item, UAbilitySystemComponent &ASC, FGrantedHandles &Receipt) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Stats", meta = (ForceInlineRow))
	TMap<FGameplayTag, FItemStatDefinition> Stats;
};
