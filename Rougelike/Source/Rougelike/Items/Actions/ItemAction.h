// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemAction.generated.h"

class UItemInstance;

/**
 * One thing using an item does. Lives inline in a Usable fragment; C++ and Blueprint subclasses both work.
 */
UCLASS(Blueprintable, BlueprintType, Abstract, DefaultToInstanced, EditInlineNew)
class ROUGELIKE_API UItemAction : public UObject
{
	GENERATED_BODY()

public:
	/** What using the item does. True if it worked: a consumable is only used up when its use succeeds. */
	UFUNCTION(BlueprintNativeEvent, Category = "Item")
	bool Execute(AActor *ItemOwner, UItemInstance *ItemInstance);
	virtual bool Execute_Implementation(AActor *ItemOwner, UItemInstance *ItemInstance);
};
