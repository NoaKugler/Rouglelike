// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun/Gun.h"
#include "Gun/Projectile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(Mesh);
}

void AGun::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MagazineSize;
}

void AGun::Shoot(const FVector &TargetLocation)
{
	UWorld *World = GetWorld();
	if (!World || !ProjectileClass || CurrentAmmo <= 0)
	{
		return;
	}

	if (World->GetTimeSeconds() - LastShotTime < 1.f / FireRate)
	{
		return;
	}

	// Flat direction so bullets fly level at muzzle height and converge on the target.
	// The fallback only matters if the target is directly above or below the muzzle.
	const FVector MuzzleLocation = Muzzle->GetComponentLocation();
	const FVector Fallback = GetOwner() ? GetOwner()->GetActorForwardVector() : GetActorForwardVector();
	const FVector Direction = (TargetLocation - MuzzleLocation).GetSafeNormal2D(UE_SMALL_NUMBER, Fallback);
	const FTransform SpawnTransform(Direction.Rotation(), MuzzleLocation);

	// Deferred spawn so Damage and Speed are set before the projectile's BeginPlay runs
	AProjectile *Projectile = World->SpawnActorDeferred<AProjectile>(
		ProjectileClass, SpawnTransform, this, Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile)
	{
		Projectile->Damage = Damage;
		Projectile->Speed = ProjectileSpeed;
		Projectile->FinishSpawning(SpawnTransform);
	}

	--CurrentAmmo;
	LastShotTime = World->GetTimeSeconds();
}
