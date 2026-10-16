// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/BaseEnemy.h"
#include "Components/WidgetComponent.h"
#include "Enemy/UI/EnemyHealthBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "GameFramework/DamageType.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

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

	// AI navigation
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	TargetPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	GetWorldTimerManager().SetTimer(ChaseTimerHandle, this, &ABaseEnemy::ChaseTarget, ChaseInterval, true);

	LastAttackTime = -AttackCooldown;
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

void ABaseEnemy::ChaseTarget()
{
	if (!TargetPawn.IsValid())
	{
		// Player may have respawned since we cached it
		TargetPawn = UGameplayStatics::GetPlayerPawn(this, 0);
		if (!TargetPawn.IsValid())
		{
			return;
		}
	}

	if (AAIController *AI = Cast<AAIController>(GetController()))
	{
		AI->MoveToActor(TargetPawn.Get(), AcceptanceRadius);
		TryAttack();
	}
}

void ABaseEnemy::TryAttack()
{
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown)
	{
		return;
	}

	if (FVector::DistSquared(GetActorLocation(), TargetPawn->GetActorLocation()) > FMath::Square(AttackRange))
	{
		return;
	}

	LastAttackTime = Now;
	UGameplayStatics::ApplyDamage(TargetPawn.Get(), AttackDamage, GetController(), this, UDamageType::StaticClass());
}
