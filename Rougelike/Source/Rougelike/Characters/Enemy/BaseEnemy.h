#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "BaseEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class ROUGELIKE_API ABaseEnemy : public ABaseCharacter

{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	virtual void BeginPlay() override;

	virtual void HandleDeath() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackDamage = 10.f;

	// Measured centre to centre, so it must exceed AcceptanceRadius + both capsule radii or the enemy stops short of hitting
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackCooldown = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthBarComponent;

	// NEW: Owned here (not just MaxWalkSpeed on the component) so speed modifiers have one value to act on later
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 300.f;

	// NEW: How close the enemy gets before the move request counts as finished
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float AcceptanceRadius = 100.f;

	// NEW: How often the move is re-issued. A finished move never restarts on its own.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float ChaseInterval = 0.25f;

private:
	void ChaseTarget();

	void TryAttack();

	double LastAttackTime = 0.0;

	TWeakObjectPtr<APawn> TargetPawn;
	FTimerHandle ChaseTimerHandle;
};
