// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/ItemInstance.h"
#include "Items/ItemDefinition.h"
#include "Items/Fragments/ItemFragment.h"

void UItemInstance::Initialize(TSubclassOf<UItemDefinition> InItemDefinition)
{
    if (!InItemDefinition)
    {
        UE_LOG(LogTemp, Warning, TEXT("UItemInstance::Initialize called with null ItemDefinition"));
        return;
    }

    ItemDefinition = InItemDefinition;

    UItemDefinition *ItemCDO = ItemDefinition.GetDefaultObject();
    for (const TObjectPtr<UItemFragment> &Fragment : ItemCDO->ItemFragments)
    {
        Fragment->OnInstanceCreated(this);
    }
}

const UItemFragment *UItemInstance::FindFragmentByClass(const TSubclassOf<UItemFragment> FragmentClass)
{
    if (!FragmentClass || !ItemDefinition)
    {
        return nullptr;
    }

    UItemDefinition *ItemCDO = ItemDefinition.GetDefaultObject();
    return ItemCDO->FindFragmentByClass(ItemDefinition, FragmentClass);
}

float UItemInstance::GetStatValue(const FGameplayTag &StatTag)
{
    return ItemStats.FindRef(StatTag);
}

void UItemInstance::SetStatValue(const FGameplayTag &StatTag, float NewValue)
{
    ItemStats.Add(StatTag, NewValue);
}
