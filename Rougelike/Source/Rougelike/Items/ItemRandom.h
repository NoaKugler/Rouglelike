#pragma once

#include "CoreMinimal.h"

/** Every random item value goes through here, so seeding (a run seed, a stream per chest) can be added in one place */
namespace ItemRandom
{
	ROUGELIKE_API float RollRange(float Min, float Max, bool bWholeNumber);
}
