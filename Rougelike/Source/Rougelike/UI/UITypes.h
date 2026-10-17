#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);

/** Layer tags. Each one names a stack registered by the layout Blueprint. */
namespace UITags
{
	ROUGELIKE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Layer_Game);
	ROUGELIKE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Layer_Menu);
	ROUGELIKE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Layer_Modal);
	ROUGELIKE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Layer_Loading);
}
