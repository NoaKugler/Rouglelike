// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/ItemFragment.h"
#include "ItemFragment_Usable.generated.h"

class UItemAction;
class UItemInstance;

/**
 *
 */
UCLASS()
class ROUGELIKE_API UItemFragment_Usable : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	bool bConsumeOnUse = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Item")
	TArray<TObjectPtr<UItemAction>> OnUseActions;

	/** Runs every action. True if at least one succeeded. Const: a fragment is shared definition data. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Item")
	bool Use(AActor *ItemOwner, UItemInstance *ItemInstance) const;
};
