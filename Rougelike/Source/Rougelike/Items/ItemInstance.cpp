// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/ItemInstance.h"

float UItemInstance::GetStatValue(const FGameplayTag &StatTag)
{
    return ItemStats.FindRef(StatTag);
}

void UItemInstance::SetStatValue(const FGameplayTag &StatTag, float NewValue)
{
    ItemStats.Add(StatTag, NewValue);
}
