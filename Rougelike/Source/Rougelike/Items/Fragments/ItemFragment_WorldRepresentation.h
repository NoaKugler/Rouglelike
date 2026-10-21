// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/ItemFragment.h"
#include "ItemFragment_WorldRepresentation.generated.h"

/**
 *
 */
UCLASS()
class ROUGELIKE_API UItemFragment_WorldRepresentation : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> ItemMesh;

	/** Applied wherever the item shows up in the world (pickup mesh, equipped actor), so one mesh can be sized per item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.01"))
	float ItemScale = 1.f;

	/** ItemScale as the uniform vector that Set Scale nodes expect */
	UFUNCTION(BlueprintPure, Category = "Item")
	FVector GetItemScale3D() const { return FVector(ItemScale); }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanBeDropped = true;
};
