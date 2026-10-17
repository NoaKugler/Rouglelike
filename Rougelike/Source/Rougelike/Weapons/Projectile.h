// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class UGameplayEffect;

UCLASS()
class ROUGELIKE_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

	const TArray<TSubclassOf<UGameplayEffect>> &GetHitEffects() const { return HitEffects; }

	/** Called by the firing ability between SpawnActorDeferred and FinishSpawning */
	void SetHitEffectSpecs(const TArray<FGameplayEffectSpecHandle> &InSpecs) { HitEffectSpecs = InSpecs; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** What a hit does, applied in order. The firing ability turns these into specs. */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TArray<TSubclassOf<UGameplayEffect>> HitEffects;

private:
	/** Shared pointers inside, nothing for the GC to track, so no UPROPERTY */
	TArray<FGameplayEffectSpecHandle> HitEffectSpecs;
};
