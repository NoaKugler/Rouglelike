#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UIManagerSettings.generated.h"

class UUILayout;

/**
 *  Project Settings > Game > UI Manager.
 *  Configures the UI manager through config instead of a Blueprint subclass of the subsystem,
 *  which would get instantiated alongside the C++ one.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "UI Manager"))
class ROUGELIKE_API UUIManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Root layout widget created for the local player (a Blueprint child of UUILayout) */
	UPROPERTY(Config, EditAnywhere, Category = "Layout")
	TSoftClassPtr<UUILayout> LayoutClass;

	UPROPERTY(Config, EditAnywhere, Category = "Layout")
	int32 LayoutZOrder = 1000;
};
