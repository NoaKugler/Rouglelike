#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Items/Fragments/ItemFragment.h"
#include "ItemFragment_StatRolls.generated.h"

/** One stat this item rolls when it's first created */
USTRUCT()
struct FItemStatRange
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Stat", meta = (Categories = "Item.Stat"))
	FGameplayTag Stat;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float Min = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	float Max = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stat")
	bool bWholeNumber = false;
};

/** Rolls stat values once per item. An item rebuilt from data keeps its rolls. */
UCLASS()
class ROUGELIKE_API UItemFragment_StatRolls : public UItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UItemInstance *ItemInstance) override;

	/** For tooltips: the range a stat rolled from, or null */
	const FItemStatRange *FindRange(const FGameplayTag &Stat) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Item", meta = (TitleProperty = "Stat"))
	TArray<FItemStatRange> Ranges;
};
