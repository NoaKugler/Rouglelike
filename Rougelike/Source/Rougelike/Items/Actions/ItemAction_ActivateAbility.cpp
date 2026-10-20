#include "Items/Actions/ItemAction_ActivateAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbilitySystem/Abilities/RougelikeGameplayAbility.h"
#include "Items/ItemInstance.h"

bool UItemAction_ActivateAbility::Execute_Implementation(AActor *ItemOwner, UItemInstance *ItemInstance)
{
	UAbilitySystemComponent *ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(ItemOwner);
	if (!ASC || !Ability)
	{
		return false;
	}

	// One-shot: the spec removes itself when the ability ends, so it needs no receipt (outside the grant door, like Instant effects)
	FGameplayAbilitySpec Spec(Ability, 1, INDEX_NONE, ItemInstance);
	return ASC->GiveAbilityAndActivateOnce(Spec).IsValid();
}
