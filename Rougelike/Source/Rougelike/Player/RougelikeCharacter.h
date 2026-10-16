// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RougelikeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UHealthComponent;
class AGun;

struct FInputActionValue;

/**
 *  The player character: movement, jumping, the equipped gun and health.
 *  Mesh, anim and default gun are set in BP_PlayerCharacter.
 */
UCLASS(abstract)
class ARougelikeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Health and death. Damage reaches it through OnTakeAnyDamage, so this class doesn't override TakeDamage. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;

	/** Camera boom positioning the follow camera relative to the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

protected:
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
	/** Constructor */
	ARougelikeCharacter();

protected:
	virtual void BeginPlay() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	/** Called when the fire input is pressed */
	void Fire();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue &Value);

	void DoMove(float Right, float Forward);

	/** Bound to HealthComponent->OnDeath */
	UFUNCTION()
	void HandleDeath();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }
};
