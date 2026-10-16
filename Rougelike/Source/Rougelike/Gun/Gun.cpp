// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun/Gun.h"
#include "Gun/Projectile.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
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
	UWorld* World = GetWorld();
	if (!World || !ProjectileClass || CurrentAmmo <= 0)
	{
		return;
	}

	if (World->GetTimeSeconds() - LastShotTime < 1.f / FireRate)
	{
		return;
	}

	// Aim where the owner's controller is looking, starting from the muzzle
	FRotator AimRotation = Muzzle->GetComponentRotation();
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (const AController* Controller = OwnerPawn->GetController())
		{
			AimRotation = Controller->GetControlRotation();
		}
	}

	// Deferred spawn so Damage is set before the projectile's BeginPlay runs
	FTransform SpawnTransform(AimRotation, Muzzle->GetComponentLocation());
	AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(
		ProjectileClass, SpawnTransform, this, Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile)
	{
		Projectile->Damage = Damage;
		Projectile->FinishSpawning(SpawnTransform);
	}

	--CurrentAmmo;
	LastShotTime = World->GetTimeSeconds();
}
