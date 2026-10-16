// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class AProjectile;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class ROUGELIKE_API AGun : public AActor
{
	GENERATED_BODY()

public:
	AGun();

	/** Fires one projectile if off cooldown and ammo remains. */
	UFUNCTION(BlueprintCallable, Category = "Gun")
	void Shoot();

protected:
	virtual void BeginPlay() override;

	/** Damage applied by each projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun")
	float Damage = 10.f;

	/** Shots per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun", meta = (ClampMin = "0.01"))
	float FireRate = 5.f;

	/** Projectile travel speed. Constant for the whole flight. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun", meta = (ClampMin = "1", Units = "cm/s"))
	float ProjectileSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun", meta = (ClampMin = "1"))
	int32 MagazineSize = 30;

	/** What we spawn when firing. Set this in each weapon Blueprint. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gun")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gun")
	int32 CurrentAmmo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Projectiles spawn here. Move it in the Blueprint to line up with the barrel. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	TObjectPtr<USceneComponent> Muzzle;

private:
	/** World time of the last shot, used for the fire rate cooldown. */
	float LastShotTime = -1000.f;
};
