#include "Items/Actions/ItemAction_ApplyGameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "Items/ItemInstance.h"

bool UItemAction_ApplyGameplayEffect::Execute_Implementation(AActor *ItemOwner, UItemInstance *ItemInstance)
{
	UAbilitySystemComponent *ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(ItemOwner);
	if (!ASC || !GameplayEffect)
	{
		return false;
	}

	// The item is the source, so whatever this effect does can be traced back to it
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(ItemInstance);

	// WasSuccessfullyApplied, not IsValid: an Instant effect returns the "already executed" handle, which is never valid
	const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GameplayEffect, 1.f, Context);
	return Spec.IsValid() && ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()).WasSuccessfullyApplied();
}
