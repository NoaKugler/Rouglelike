// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/BaseEnemy.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

float ABaseEnemy::TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Applied <= 0.f || CurrentHealth <= 0.f)
	{
		return 0.f;
	}

	CurrentHealth -= Applied;
	if (CurrentHealth <= 0.f)
	{
		Die();
	}

	return Applied;
}

void ABaseEnemy::Die()
{
	Destroy();
}
