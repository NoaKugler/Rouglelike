#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ItemSettings.generated.h"

class UItemStatDefinitions;

/** Project Settings > Game > Items */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Items"))
class ROUGELIKE_API UItemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** What each Item.Stat.* means: its name in tooltips and the effect it applies while equipped */
	UPROPERTY(Config, EditAnywhere, Category = "Stats")
	TSoftObjectPtr<UItemStatDefinitions> StatDefinitions;
};
