// Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/Enemy/BaseEnemy.h"
#include "Components/WidgetComponent.h"
#include "Characters/Enemy/UI/EnemyHealthBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "GameFramework/DamageType.h"
#include "Combat/HealthComponent.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

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

	if (UEnemyHealthBar *Bar = Cast<UEnemyHealthBar>(HealthBarComponent->GetUserWidgetObject()))
	{
		UHealthComponent *Health = GetHealthComponent();
		Health->OnHealthChanged.AddDynamic(Bar, &UEnemyHealthBar::SetHealth);
		Bar->SetHealth(Health->GetCurrentHealth(), Health->GetMaxHealth());
	}

	// AI navigation
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	TargetPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	GetWorldTimerManager().SetTimer(ChaseTimerHandle, this, &ABaseEnemy::ChaseTarget, ChaseInterval, true);

	LastAttackTime = -AttackCooldown;
}

void ABaseEnemy::HandleDeath()
{
	Super::HandleDeath();
	Destroy();
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
