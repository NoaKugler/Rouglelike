#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "GrantSet.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class URougelikeGameplayAbility;

/** One ability a grant set gives, and at which level */
USTRUCT()
struct FGrantSetAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<URougelikeGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (ClampMin = "1"))
	int32 Level = 1;

	/** The input that activates it. Empty for abilities started another way (gameplay events, other abilities). */
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * The receipt for one Give: exactly what it added, so Revoke removes that and nothing else.
 * Whoever gave the grant set keeps it (the character for its starting kit, the equipment instance for an item).
 * Plain C++: it holds no strong UObject references, so the garbage collector doesn't need to see it.
 */
struct ROUGELIKE_API FGrantedHandles
{
	/** Removes everything this receipt added. Running abilities among them end immediately. Safe to call twice. */
	void Revoke();

	bool IsEmpty() const { return AbilitySpecHandles.IsEmpty() && EffectHandles.IsEmpty(); }

private:
	friend struct FGrantSet;

	// Weak: the receipt must not keep the ASC alive. If the ASC is gone, everything it held went with it.
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	TArray<FActiveGameplayEffectHandle> EffectHandles;
};

/**
 * Everything one source of power gives while it is active: abilities, plus Infinite effects.
 * The only way persistent abilities and effects reach an ASC.
 */
USTRUCT()
struct ROUGELIKE_API FGrantSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Grants")
	TArray<FGrantSetAbility> Abilities;

	/** Must be Infinite, with Stacking Type None: each one lasts exactly as long as the receipt */
	UPROPERTY(EditDefaultsOnly, Category = "Grants")
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	/**
	 * SourceObject: where this power comes from (an item instance, or the character for its starting kit).
	 * Every granted ability and effect context carries it.
	 */
	[[nodiscard]] FGrantedHandles Give(UAbilitySystemComponent *AbilitySystemComponent, UObject *SourceObject) const;
};
