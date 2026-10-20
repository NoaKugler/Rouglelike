// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/Effects/DamageReport.h"
#include "RougelikeAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilitiesChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageReceived, const FDamageReport &);

/**
 * Project ASC: engine hooks that other systems listen to
 */
UCLASS()
class ROUGELIKE_API URougelikeAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/** Fires once on the tick after abilities were given or removed, however many changed in between */
	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnAbilitiesChanged OnAbilitiesChanged;

	/** Fires on the victim once per damaging hit. Native: C++ systems listen here, Blueprint uses UHealthComponent::OnDamaged. */
	FOnDamageReceived OnDamageReceived;

	/** Input with this tag went down: running abilities hear about it, idle ones try to activate */
	void AbilityInputTagPressed(const FGameplayTag &InputTag);

	/** Every frame while the input is down: retries idle WhileInputHeld abilities */
	void AbilityInputTagHeld(const FGameplayTag &InputTag);

	void AbilityInputTagReleased(const FGameplayTag &InputTag);

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec &AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec &AbilitySpec) override;

private:
	void QueueAbilitiesChanged();
	void BroadcastAbilitiesChanged();

	// Wakes WaitInputPress / WaitInputRelease tasks in the running ability
	void InvokeInputEvent(EAbilityGenericReplicatedEvent::Type EventType, const FGameplayAbilitySpec &Spec);

	bool bAbilitiesChangedQueued = false;
};
