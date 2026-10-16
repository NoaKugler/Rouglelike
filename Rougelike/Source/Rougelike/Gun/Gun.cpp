// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun/Gun.h"
#include "Gun/Projectile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "MyCollisionChannels.h"

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

void AGun::Shoot()
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

	// Spawn at the muzzle, fly toward whatever is under the crosshair
	const FVector MuzzleLocation = Muzzle->GetComponentLocation();
	const FTransform SpawnTransform(GetAimDirection(MuzzleLocation).Rotation(), MuzzleLocation);

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

FVector AGun::GetAimDirection(const FVector &MuzzleLocation) const
{
	const APawn *OwnerPawn = Cast<APawn>(GetOwner());
	const AController *Controller = OwnerPawn ? OwnerPawn->GetController() : nullptr;
	if (!Controller)
	{
		return Muzzle->GetForwardVector();
	}

	// Player: the camera. AI: the pawn's eyes. So enemy guns get this for free.
	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	const FVector ViewDirection = ViewRotation.Vector();

	// Start the trace level with the muzzle so nothing between camera and character can be aimed at
	const FVector TraceStart = ViewLocation + ViewDirection * FVector::DotProduct(MuzzleLocation - ViewLocation, ViewDirection);
	const FVector TraceEnd = TraceStart + ViewDirection * AimTraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(GunAim), false, OwnerPawn);
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Weapon, Params);
	const FVector AimPoint = bHit ? Hit.ImpactPoint : TraceEnd;

	// Fallback for when the target is pressed against the muzzle
	return (AimPoint - MuzzleLocation).GetSafeNormal(UE_SMALL_NUMBER, ViewDirection);
}
