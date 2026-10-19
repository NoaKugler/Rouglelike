// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Fragments/ItemFragment_Durability.h"
#include "Items/ItemInstance.h"
#include "Items/ItemTags.h"

void UItemFragment_Durability::OnInstanceCreated(UItemInstance *ItemInstance)
{
    Super::OnInstanceCreated(ItemInstance);

    if (!ItemInstance)
        return;

    ItemInstance->SetStatValue(ItemTags::Stat_Durability, MaxUses);
}

void UItemFragment_Durability::DecrementDurability(UItemInstance *ItemInstance) const
{
    if (!ItemInstance)
        return;

    ItemInstance->SetStatValue(ItemTags::Stat_Durability, FMath::Max(0.f, ItemInstance->GetStatValue(ItemTags::Stat_Durability) - 1.f));
}

bool UItemFragment_Durability::HasDurabilityLeft(UItemInstance *ItemInstance) const
{
    if (!ItemInstance)
        return false;

    return ItemInstance->GetStatValue(ItemTags::Stat_Durability) > 0.f;
}
