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
	Movement->Velocity = FVector::ZeroVector;

	// Safety net so a bullet that never hits anything cleans itself up
	InitialLifeSpan = 5.f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Rotation is the direction, Speed is the magnitude. Set here because the gun
	// assigns Speed after the constructor, in the deferred spawn window.
	Movement->MaxSpeed = Speed;
	Movement->Velocity = GetActorForwardVector() * Speed;

	// Don't collide with the gun that fired us or the pawn holding it. The muzzle
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
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		UAbilitySystemComponent *SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());
		UAbilitySystemComponent *TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
		if (SourceASC && TargetASC && DamageEffect)
		{
			// Made on the source, so GE_Damage captures the shooter's Damage attribute
			const FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffect, 1.f, SourceASC->MakeEffectContext());
			SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
		}
	}

	Destroy();
}
