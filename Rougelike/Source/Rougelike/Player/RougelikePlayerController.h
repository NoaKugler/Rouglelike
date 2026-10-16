// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RougelikePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Player controller: adds the input mapping contexts, spawns the HUD
 *  and turns the mouse cursor into a world-space aim point.
 */
UCLASS(abstract)
class ARougelikePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARougelikePlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	/** The cursor projected onto a horizontal plane at the pawn's center height */
	FVector GetAimLocation() const { return AimLocation; }

	void UpdateFacing();

protected:
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	/** HUD widget to spawn for the local player */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidget;

	/** The aim point never gets closer to the pawn than this, so the aim direction can't flip when the cursor is on the character */
	UPROPERTY(EditDefaultsOnly, Category = "Aim", meta = (ClampMin = "0", Units = "cm"))
	float MinAimDistance = 150.f;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

private:
	void UpdateAimLocation();

	/** Last valid aim point. Kept while the cursor is outside the viewport. */
	UPROPERTY(VisibleInstanceOnly, Category = "Aim")
	FVector AimLocation = FVector::ZeroVector;
};
