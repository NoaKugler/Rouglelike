// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RougelikeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class AGun;
struct FInputActionValue;

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHealthChanged, float, NewHealth);

UCLASS(abstract)
class ARougelikeCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

public:
	virtual float TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnPlayerHealthChanged OnHealthChanged;

protected:
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.f;

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

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }
};
