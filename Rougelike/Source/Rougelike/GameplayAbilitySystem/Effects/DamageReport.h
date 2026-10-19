#pragma once

#include "CoreMinimal.h"
#include "DamageReport.generated.h"

class AActor;

/**
 * What one damaging hit did to its victim. Built once by UCoreAttributeSet after Health changed,
 * and sent through the victim's URougelikeAbilitySystemComponent::OnDamageReceived.
 */
USTRUCT(BlueprintType)
struct FDamageReport
{
	GENERATED_BODY()

	/** Who is responsible: the attacking character */
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	TObjectPtr<AActor> Instigator;

	/** What dealt it. For now always the attacker; a projectile would have to add itself to the context. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	TObjectPtr<AActor> DamageCauser;

	/** SourceObject of the ability that dealt it. Becomes the item instance once items grant abilities (F4). */
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	TObjectPtr<UObject> SourceObject;

	/** Final damage from UDamageExecution, before Health clamped it */
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	float Damage = 0.f;

	/** Health actually removed. Lower than Damage on overkill. */
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	float HealthLost = 0.f;

	/** True only for the hit that took Health from above zero to zero */
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	bool bKillingBlow = false;
};
