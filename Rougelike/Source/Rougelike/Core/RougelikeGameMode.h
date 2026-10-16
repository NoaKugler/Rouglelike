// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RougelikeGameMode.generated.h"

/**
 *  Base GameMode. Default pawn and controller classes are set in BP_GameMode.
 */
UCLASS(abstract)
class ARougelikeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	ARougelikeGameMode();
};



