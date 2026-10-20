// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/ItemInstance.h"
#include "Items/ItemDefinition.h"
#include "Rougelike.h"
#include "Items/Fragments/ItemFragment.h"

UItemInstance *UItemInstance::CreateItem(UObject *Outer, TSubclassOf<UItemDefinition> InItemDefinition, const TMap<FGameplayTag, float> &InItemStats)
{
    if (!Outer || !InItemDefinition)
    {
        return nullptr;
    }

    UItemInstance *Item = NewObject<UItemInstance>(Outer);
    Item->Initialize(InItemDefinition, InItemStats);
    return Item;
}

FStackInfo UItemInstance::ToStackInfo(int32 Count) const
{
    FStackInfo Data;
    Data.ItemDefinition = ItemDefinition;
    Data.Count = Count;
    Data.ItemStats = ItemStats;
    return Data;
}

void UItemInstance::Initialize(
    TSubclassOf<UItemDefinition> InItemDefinition,
    const TMap<FGameplayTag, float> &InItemStats)
{
    if (!InItemDefinition)
    {
        UE_LOG(LogRougelike, Warning, TEXT("UItemInstance::Initialize called with null ItemDefinition"));
        return;
    }

    ItemDefinition = InItemDefinition;

    UItemDefinition *ItemCDO = ItemDefinition.GetDefaultObject();
    for (const TObjectPtr<UItemFragment> &Fragment : ItemCDO->ItemFragments)
    {
        // An empty entry in a definition's fragment list is easy to leave behind in the editor
        if (Fragment)
        {
            Fragment->OnInstanceCreated(this);
        }
    }

    // Per key: passed-in values (e.g. the uses left on a dropped item) win, and fragment defaults for other keys stay
    ItemStats.Append(InItemStats);
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
