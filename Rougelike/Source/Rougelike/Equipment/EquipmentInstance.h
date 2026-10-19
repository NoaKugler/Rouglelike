// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EquipmentInstance.generated.h"

class ACharacter;
class UItemInstance;
class UEquipmentDefinition;
/**
 *
 */
UCLASS(BlueprintType)
class ROUGELIKE_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UItemInstance> SourceItemInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<AActor> EquipmentActor;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void Initialize(UItemInstance *InSourceItemInstance, ACharacter *Character);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void HandleEquipItem(ACharacter *Character);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void HandleUnequipItem(ACharacter *Character);
};
