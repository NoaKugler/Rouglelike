// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/BaseCharacter.h"
#include "RougelikeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class AGun;

struct FInputActionValue;

/**
 *  The player character: camera, input, movement and the equipped gun.
 *  Health and death come from ABaseCharacter.
 */
UCLASS(abstract)
class ARougelikeCharacter : public ABaseCharacter

{
	GENERATED_BODY()

	/** Camera boom positioning the follow camera relative to the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

public:
	ARougelikeCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void HandleDeath() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	/** Called when the fire input is pressed */
	void Fire();

	/** Called for movement input */
	void Move(const FInputActionValue &Value);

	void DoMove(float Right, float Forward);

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	/** Gun spawned and equipped on BeginPlay */
	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	TSubclassOf<AGun> DefaultGunClass;

	/** Socket on the character mesh the gun attaches to */
	UPROPERTY(EditDefaultsOnly, Category = "Gun")
	FName GunSocketName = TEXT("hand_r");

	UPROPERTY()
	TObjectPtr<AGun> EquippedGun;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }
};
