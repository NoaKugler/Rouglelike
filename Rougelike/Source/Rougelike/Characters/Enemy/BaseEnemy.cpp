// Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/Enemy/BaseEnemy.h"
#include "Components/WidgetComponent.h"
#include "Characters/Enemy/UI/EnemyHealthBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Combat/HealthComponent.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	// Path following sets velocity directly by default, leaving acceleration at zero, and the anim BP gates walking on acceleration
	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(RootComponent);
	HealthBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarComponent->SetDrawSize(FVector2D(100.f, 10.f));

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetHealthComponent()->OnDamaged.AddDynamic(this, &ABaseEnemy::HandleDamaged);

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

void ABaseEnemy::HandleDamaged_Implementation(float DamageAmount)
{
	UE_LOG(LogTemp, Log, TEXT("%s took %.1f damage"), *GetName(), DamageAmount);
}

void ABaseEnemy::HandleDeath()
{
	Super::HandleDeath();

	// We're inside the ASC's attribute update here, so don't destroy right away
	GetWorldTimerManager().ClearTimer(ChaseTimerHandle);
	SetActorEnableCollision(false);
	SetLifeSpan(0.1f);
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

	UAbilitySystemComponent *SourceASC = GetAbilitySystemComponent();
	UAbilitySystemComponent *TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetPawn.Get());
	if (SourceASC && TargetASC && AttackDamageEffect)
	{
		// Made on the source, so GE_Damage captures this enemy's Damage attribute
		const FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(AttackDamageEffect, 1.f, SourceASC->MakeEffectContext());
		SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
	}
}
