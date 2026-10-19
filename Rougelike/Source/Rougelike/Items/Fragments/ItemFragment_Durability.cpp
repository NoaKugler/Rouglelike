// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Fragments/ItemFragment_Durability.h"
#include "Items/ItemInstance.h"
#include "GameplayTagContainer.h"

void UItemFragment_Durability::OnInstanceCreated(UItemInstance *ItemInstance)
{
    Super::OnInstanceCreated(ItemInstance);

    static const FGameplayTag DurabilityTag = FGameplayTag::RequestGameplayTag(FName("Item.Stat.Durability"), false);
    if (!ItemInstance || !DurabilityTag.IsValid())
    {
        return;
    }

    ItemInstance->SetStatValue(DurabilityTag, MaxUses);
}

void UItemFragment_Durability::DecrementDurability(UItemInstance *ItemInstance) const
{
    static const FGameplayTag DurabilityTag = FGameplayTag::RequestGameplayTag(FName("Item.Stat.Durability"), false);
    if (!ItemInstance || !DurabilityTag.IsValid())
    {
        return;
    }

    ItemInstance->SetStatValue(DurabilityTag, FMath::Max(0.f, ItemInstance->GetStatValue(DurabilityTag) - 1.f));
}

bool UItemFragment_Durability::HasDurabilityLeft(UItemInstance *ItemInstance) const
{
    static const FGameplayTag DurabilityTag = FGameplayTag::RequestGameplayTag(FName("Item.Stat.Durability"), false);
    if (!ItemInstance || !DurabilityTag.IsValid())
    {
        return false;
    }

    return ItemInstance->GetStatValue(DurabilityTag) > 0.f;
}
