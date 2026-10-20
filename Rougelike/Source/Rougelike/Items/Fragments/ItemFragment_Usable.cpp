// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Fragments/ItemFragment_Usable.h"
#include "Items/Actions/ItemAction.h"

bool UItemFragment_Usable::Use(AActor *ItemOwner, UItemInstance *ItemInstance) const
{
    bool bAnySucceeded = false;
    for (const TObjectPtr<UItemAction> &Action : OnUseActions)
    {
        if (Action && Action->Execute(ItemOwner, ItemInstance))
        {
            bAnySucceeded = true;
        }
    }
    return bAnySucceeded;
}
