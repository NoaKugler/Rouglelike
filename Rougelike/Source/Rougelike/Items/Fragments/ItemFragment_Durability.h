// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/ItemFragment.h"
#include "ItemFragment_Durability.generated.h"

/**
 *
 */
UCLASS()
class ROUGELIKE_API UItemFragment_Durability : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxUses = 1;

	virtual void OnInstanceCreated(UItemInstance *ItemInstance) override;

	// Takes the instance because the fragment is shared definition data; the durability itself lives on the instance.
	UFUNCTION(BlueprintCallable, Category = "Item")
	void DecrementDurability(UItemInstance *ItemInstance) const;
};
