// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RougelikePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Player controller: adds the input mapping contexts and spawns the HUD
 */
UCLASS(abstract)
class ARougelikePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	/** HUD widget to spawn for the local player */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
};
