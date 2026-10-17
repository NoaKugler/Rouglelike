// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem/Abilities/RougelikeGameplayAbility.h"
#include "ProjectileAbility.generated.h"

class AProjectile;

/**
 * Fires one projectile from the avatar toward its aim point.
 * Each Blueprint child picks the projectile, so one class serves every weapon, item or enemy.
 */
UCLASS(Abstract)
class ROUGELIKE_API UProjectileAbility : public URougelikeGameplayAbility
{
    GENERATED_BODY()

public:
    UProjectileAbility();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData) override;

protected:
    /** What we shoot. Its HitEffects decide what a hit does. */
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TSubclassOf<AProjectile> ProjectileClass;

    /** How far in front of the avatar the projectile appears. Past the capsule radius, so it starts outside. */
    UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = "0", Units = "cm"))
    float SpawnDistance = 60.f;

private:
    void SpawnProjectile();
};
