// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RougelikeAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilitiesChanged);

/**
 *
 */
UCLASS()
class ROUGELIKE_API URougelikeAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/** Fires once on the tick after abilities were given or removed, however many changed in between */
	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnAbilitiesChanged OnAbilitiesChanged;

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec &AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec &AbilitySpec) override;

private:
	void QueueAbilitiesChanged();
	void BroadcastAbilitiesChanged();

	bool bAbilitiesChangedQueued = false;
};
