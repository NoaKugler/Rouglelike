// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilitySystem/RougelikeGameplayTags.h"

namespace RougelikeGameplayTags
{
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "Base damage of a hit or tick, before the attacker's stats scale it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Unkillable, "Status.Unkillable", "Health can't drop below 1, however large the hit");
}
