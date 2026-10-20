#include "Items/Fragments/ItemFragment_StatRolls.h"
#include "Items/ItemInstance.h"
#include "Items/ItemRandom.h"

void UItemFragment_StatRolls::OnInstanceCreated(UItemInstance *ItemInstance)
{
	Super::OnInstanceCreated(ItemInstance);
	if (!ItemInstance)
	{
		return;
	}

	for (const FItemStatRange &Range : Ranges)
	{
		// A rebuilt item (dropped and picked up, later a loaded save) brings its rolls with it
		if (Range.Stat.IsValid() && !ItemInstance->HasStat(Range.Stat))
		{
			ItemInstance->SetStatValue(Range.Stat, ItemRandom::RollRange(Range.Min, Range.Max, Range.bWholeNumber));
		}
	}
}

const FItemStatRange *UItemFragment_StatRolls::FindRange(const FGameplayTag &Stat) const
{
	return Ranges.FindByPredicate([&Stat](const FItemStatRange &Range) { return Range.Stat == Stat; });
}
