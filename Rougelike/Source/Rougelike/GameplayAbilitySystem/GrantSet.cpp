#include "GameplayAbilitySystem/GrantSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayAbilitySystem/Abilities/RougelikeGameplayAbility.h"
#include "Rougelike.h"

FGrantedHandles FGrantSet::Give(UAbilitySystemComponent *AbilitySystemComponent, UObject *SourceObject) const
{
	FGrantedHandles Handles;
	if (!AbilitySystemComponent)
	{
		return Handles;
	}
	Handles.AbilitySystemComponent = AbilitySystemComponent;

	for (const FGrantSetAbility &Entry : Abilities)
	{
		// An empty slot in the array is easy to leave behind in the editor
		if (!Entry.Ability)
		{
			continue;
		}

		// The spec remembers its source, so the ability can find it later through GetCurrentSourceObject()
		FGameplayAbilitySpec Spec(Entry.Ability, Entry.Level, INDEX_NONE, SourceObject);

		// Dynamic: the input belongs to this grant, not to the ability class, so another source can put the same ability on a different key
		if (Entry.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Entry.InputTag);
		}

		const FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
		if (Handle.IsValid())
		{
			Handles.AbilitySpecHandles.Add(Handle);
		}
	}

	for (const TSubclassOf<UGameplayEffect> &EffectClass : Effects)
	{
		if (!EffectClass)
		{
			continue;
		}

		// One context per effect, so specs never share (and mutate) the same context object
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(SourceObject);

		const FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, Context);
		if (!Spec.IsValid())
		{
			continue;
		}

		Handles.ApplyEffectSpec(*AbilitySystemComponent, *Spec.Data.Get());
	}

	return Handles;
}

bool FGrantedHandles::ApplyEffectSpec(UAbilitySystemComponent &ASC, const FGameplayEffectSpec &Spec)
{
	// One receipt, one ASC: Revoke only knows the one it remembers
	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent.Get() != &ASC)
	{
		ensureMsgf(false, TEXT("A receipt can only hold effects on one ability system component"));
		return false;
	}

	// An Instant effect is over before a receipt could hold it, and a timed one ends on its own
	if (!Spec.Def || Spec.Def->DurationPolicy != EGameplayEffectDurationType::Infinite)
	{
		UE_LOG(LogRougelike, Warning, TEXT("Skipped %s: only Infinite effects can be granted"), *GetNameSafe(Spec.Def));
		return false;
	}

	AbilitySystemComponent = &ASC;

	// Invalid for an effect that failed to apply (e.g. blocked by tag requirements): nothing to revoke then
	const FActiveGameplayEffectHandle Handle = ASC.ApplyGameplayEffectSpecToSelf(Spec);
	if (!Handle.IsValid())
	{
		return false;
	}
	EffectHandles.Add(Handle);
	return true;
}

void FGrantedHandles::Revoke()
{
	if (UAbilitySystemComponent *ASC = AbilitySystemComponent.Get())
	{
		for (const FGameplayAbilitySpecHandle &Handle : AbilitySpecHandles)
		{
			// Ends the ability first if it is running, then removes the spec
			ASC->ClearAbility(Handle);
		}

		for (const FActiveGameplayEffectHandle &Handle : EffectHandles)
		{
			// One stack: this receipt owns one application, never the whole effect
			ASC->RemoveActiveGameplayEffect(Handle, 1);
		}
	}

	// A second Revoke finds nothing left to remove
	AbilitySpecHandles.Reset();
	EffectHandles.Reset();
	AbilitySystemComponent.Reset();
}
