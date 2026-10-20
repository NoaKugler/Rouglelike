// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/ItemInstance.h"
#include "Items/ItemDefinition.h"
#include "Rougelike.h"
#include "Items/Fragments/ItemFragment.h"
#include "Items/Fragments/ItemFragment_StatRolls.h"
#include "Items/Stats/ItemStatDefinitions.h"

#define LOCTEXT_NAMESPACE "ItemInstance"

namespace
{
    FText FormatStatNumber(float Value, bool bPercent)
    {
        FNumberFormattingOptions Options;
        Options.MaximumFractionalDigits = 1;
        return bPercent ? FText::AsPercent(Value, &Options) : FText::AsNumber(Value, &Options);
    }
}

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

    // The data comes first; fragments only fill in what it lacks, so a rebuilt item keeps its state and never re-rolls
    ItemStats = InItemStats;

    UItemDefinition *ItemCDO = ItemDefinition.GetDefaultObject();
    for (const TObjectPtr<UItemFragment> &Fragment : ItemCDO->ItemFragments)
    {
        // An empty entry in a definition's fragment list is easy to leave behind in the editor
        if (Fragment)
        {
            Fragment->OnInstanceCreated(this);
        }
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

FText UItemInstance::GetStatDisplayName(FGameplayTag StatTag) const
{
    const UItemStatDefinitions *Definitions = UItemStatDefinitions::Get();
    const FItemStatDefinition *Definition = Definitions ? Definitions->Find(StatTag) : nullptr;
    return Definition && !Definition->DisplayName.IsEmpty() ? Definition->DisplayName : FText::FromName(StatTag.GetTagName());
}

FText UItemInstance::GetStatValueText(FGameplayTag StatTag) const
{
    const UItemStatDefinitions *Definitions = UItemStatDefinitions::Get();
    const FItemStatDefinition *Definition = Definitions ? Definitions->Find(StatTag) : nullptr;
    const bool bPercent = Definition && Definition->bPercent;
    const FText Value = FormatStatNumber(ItemStats.FindRef(StatTag), bPercent);

    const UItemFragment_StatRolls *Rolls = Cast<UItemFragment_StatRolls>(UItemDefinition::FindFragmentByClass(ItemDefinition, UItemFragment_StatRolls::StaticClass()));
    const FItemStatRange *Range = Rolls ? Rolls->FindRange(StatTag) : nullptr;
    if (!Range)
    {
        return Value;
    }
    return FText::Format(LOCTEXT("StatWithRange", "{0} ({1}-{2})"), Value, FormatStatNumber(Range->Min, bPercent), FormatStatNumber(Range->Max, bPercent));
}

#undef LOCTEXT_NAMESPACE
