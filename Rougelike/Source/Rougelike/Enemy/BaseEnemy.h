#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class ROUGELIKE_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual float TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	void Die();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.f;

	// Runtime state, deliberately not editable
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthBarComponent;

	void UpdateHealthBar();

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
	// NEW
	void ChaseTarget();

	TWeakObjectPtr<APawn> TargetPawn;
	FTimerHandle ChaseTimerHandle;
};
