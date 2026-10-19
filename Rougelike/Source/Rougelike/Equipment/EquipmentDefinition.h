// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EquipmentDefinition.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType, Abstract, Const)
class ROUGELIKE_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> EquipmentActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName EquipmentSocketName;
};
