#include "Items/Stats/ItemStatDefinitions.h"
#include "Items/ItemSettings.h"
#include "Items/ItemInstance.h"
#include "GameplayAbilitySystem/GrantSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

const UItemStatDefinitions *UItemStatDefinitions::Get()
{
	// Cheap once it's in memory; a small asset every equip and tooltip needs
	return GetDefault<UItemSettings>()->StatDefinitions.LoadSynchronous();
}

void UItemStatDefinitions::ApplyEquippedStats(const UItemInstance &Item, UAbilitySystemComponent &ASC, FGrantedHandles &Receipt) const
{
	for (const TPair<FGameplayTag, float> &Stat : Item.ItemStats)
	{
		const FItemStatDefinition *Definition = Stats.Find(Stat.Key);
		if (!Definition || !Definition->EquippedEffect)
		{
			continue;
		}

		FGameplayEffectContextHandle Context = ASC.MakeEffectContext();
		Context.AddSourceObject(&Item);

		const FGameplayEffectSpecHandle Spec = ASC.MakeOutgoingSpec(Definition->EquippedEffect, 1.f, Context);
		if (!Spec.IsValid())
		{
			continue;
		}

		// The archetype effect reads its value under the stat's own tag
		Spec.Data->SetSetByCallerMagnitude(Stat.Key, Stat.Value);
		Receipt.ApplyEffectSpec(ASC, *Spec.Data.Get());
	}
}
