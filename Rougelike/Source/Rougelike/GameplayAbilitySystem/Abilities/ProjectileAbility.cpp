#include "GameplayAbilitySystem/Abilities/ProjectileAbility.h"
#include "Characters/BaseCharacter.h"
#include "Weapons/Projectile.h"
#include "Engine/World.h"

UProjectileAbility::UProjectileAbility()
{
    // 5.8 defaults to InstancedPerExecution, which creates a new ability object for every shot
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UProjectileAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
    // Pays the cost and starts the cooldown. Fails if either can't be paid.
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Gameplay actors are spawned by the server only. Always true in single player, free to get right now.
    if (HasAuthority(&ActivationInfo))
    {
        SpawnProjectile();
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UProjectileAbility::SpawnProjectile()
{
    ABaseCharacter *Avatar = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
    UWorld *World = GetWorld();
    if (!Avatar || !World || !ProjectileClass)
    {
        return;
    }

    // Flat direction so projectiles fly level and converge on the aim point, like the old gun
    const FVector AvatarLocation = Avatar->GetActorLocation();
    const FVector Direction = (Avatar->GetAimLocation() - AvatarLocation).GetSafeNormal2D(UE_SMALL_NUMBER, Avatar->GetActorForwardVector());
    const FTransform SpawnTransform(Direction.Rotation(), AvatarLocation + Direction * SpawnDistance);

    // Deferred so the specs are in place before the projectile's BeginPlay
    AProjectile *Projectile = World->SpawnActorDeferred<AProjectile>(
        ProjectileClass, SpawnTransform, Avatar, Avatar,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!Projectile)
    {
        return;
    }

    // Made by the ability: each spec captures the caster's attributes, the ability level and the ability's tags
    TArray<FGameplayEffectSpecHandle> Specs;
    for (const TSubclassOf<UGameplayEffect> &EffectClass : Projectile->GetHitEffects())
    {
        if (EffectClass)
        {
            Specs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, GetAbilityLevel()));
        }
    }

    Projectile->SetHitEffectSpecs(Specs);
    Projectile->FinishSpawning(SpawnTransform);
}
