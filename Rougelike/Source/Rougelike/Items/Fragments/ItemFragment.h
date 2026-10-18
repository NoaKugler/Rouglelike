// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemFragment.generated.h"

class UItemInstance;

/**
 *
 */
UCLASS(BlueprintType, Abstract, DefaultToInstanced, EditInlineNew)
class ROUGELIKE_API UItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UItemInstance *ItemInstance) {}
};
