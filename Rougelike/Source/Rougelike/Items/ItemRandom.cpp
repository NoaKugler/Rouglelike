#include "Items/ItemRandom.h"

float ItemRandom::RollRange(float Min, float Max, bool bWholeNumber)
{
	if (Max < Min)
	{
		Swap(Min, Max);
	}

	return bWholeNumber
		? static_cast<float>(FMath::RandRange(FMath::RoundToInt(Min), FMath::RoundToInt(Max)))
		: FMath::FRandRange(Min, Max);
}
