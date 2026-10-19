#pragma once

#include "NativeGameplayTags.h"

/** Item tags the C++ reads. Native, so they always exist and a typo is a compile error. */
namespace ItemTags
{
    /** Uses left on an item with a durability fragment. Stored in the item instance's stats. */
    ROUGELIKE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Durability);
}
