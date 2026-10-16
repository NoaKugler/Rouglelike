// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/BaseEnemy.h"
#include "Components/WidgetComponent.h"
#include "Enemy/UI/EnemyHealthBar.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	// constructor
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(RootComponent);
	HealthBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComponent->SetDrawSize(FVector2D(100.f, 10.f));
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	UpdateHealthBar();
}

float ABaseEnemy::TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Applied <= 0.f || CurrentHealth <= 0.f)
	{
		return 0.f;
	}

	CurrentHealth -= Applied;
	UpdateHealthBar();
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

void ABaseEnemy::UpdateHealthBar()
{
	// UpdateHealthBar: called from BeginPlay (after CurrentHealth is set) and from TakeDamage
	if (UEnemyHealthBar *Bar = Cast<UEnemyHealthBar>(HealthBarComponent->GetUserWidgetObject()))
	{
		Bar->SetHealth(CurrentHealth, MaxHealth);
	}
}
