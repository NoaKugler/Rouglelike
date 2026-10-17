// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Projectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(10.f);
	Collision->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = Collision;

	// Visual only, the sphere handles collision
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->ProjectileGravityScale = 0.f; // no bullet drop for now
	Movement->bRotationFollowsVelocity = true;
	// Velocity defaults to local forward, so the spawn rotation is the flight direction
	Movement->InitialSpeed = 2000.f;
	Movement->MaxSpeed = 2000.f;

	// Safety net so a bullet that never hits anything cleans itself up
	InitialLifeSpan = 5.f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Don't collide with the whoever fired fired us or the pawn holding it. The muzzle
	// sits inside the pawn's capsule, so without this the bullet dies on spawn.
	if (AActor *OwnerActor = GetOwner())
	{
		Collision->IgnoreActorWhenMoving(OwnerActor, true);
	}
	if (AActor *InstigatorActor = GetInstigator())
	{
		Collision->IgnoreActorWhenMoving(InstigatorActor, true);
	}

	Collision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	// Walls and props have no ASC: we just die on them
	if (UAbilitySystemComponent *TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{
		for (const FGameplayEffectSpecHandle &Spec : HitEffectSpecs)
		{
			if (Spec.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}

	Destroy();
}
