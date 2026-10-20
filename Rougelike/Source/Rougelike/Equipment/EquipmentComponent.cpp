#include "Equipment/EquipmentComponent.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"
#include "Items/ItemInstance.h"
#include "Items/Fragments/ItemFragment_Equippable.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "AbilitySystemGlobals.h"
#include "Items/Stats/ItemStatDefinitions.h"
#include "Rougelike.h"

UEquipmentComponent::UEquipmentComponent()
{
	// Equipment only changes when told to
	PrimaryComponentTick.bCanEverTick = false;
}

UEquipmentInstance *UEquipmentComponent::EquipItem(UItemInstance *Item)
{
	if (!Item)
	{
		return nullptr;
	}

	if (UEquipmentInstance *Existing = FindEquipmentForItem(Item))
	{
		return Existing;
	}

	const UItemFragment_Equippable *Equippable = Cast<UItemFragment_Equippable>(Item->FindFragmentByClass(UItemFragment_Equippable::StaticClass()));
	const UEquipmentDefinition *Definition = Equippable ? Equippable->EquipmentDefinition.GetDefaultObject() : nullptr;
	if (!Definition)
	{
		UE_LOG(LogRougelike, Warning, TEXT("%s: %s has no equipment definition"), *GetNameSafe(GetOwner()), *GetNameSafe(Item));
		return nullptr;
	}

	const FGameplayTag Slot = FindSlotFor(*Definition);
	if (!Slot.IsValid())
	{
		UE_LOG(LogRougelike, Warning, TEXT("%s has no slot that fits %s (allowed: %s)"), *GetNameSafe(GetOwner()), *GetNameSafe(Definition->GetClass()), *Definition->AllowedSlot.ToString());
		return nullptr;
	}

	// The old occupant leaves first, so its actor (and from F4 its grants) are gone before the new ones arrive
	if (UEquipmentInstance *Occupant = GetEquipmentInSlot(Slot))
	{
		Remove(*Occupant);
	}

	// Outer = this component: the record lives as long as the equipment it describes
	UEquipmentInstance *Equipment = NewObject<UEquipmentInstance>(this);
	Equipment->Item = Item;
	Equipment->Definition = Definition->GetClass();
	Equipment->Slot = Slot;
	SpawnEquipmentActor(*Equipment, *Definition);

	// After the actor exists, so an ability that activates right away can find its weapon.
	// The item is the source: its abilities can find it, and every hit they make carries it.
	if (UAbilitySystemComponent *ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		Equipment->GrantedHandles = Definition->Grants.Give(ASC, Item);

		// The item's own values (its rolls) go on the same receipt, so unequip removes them with the grants
		if (const UItemStatDefinitions *StatDefinitions = UItemStatDefinitions::Get())
		{
			StatDefinitions->ApplyEquippedStats(*Item, *ASC, Equipment->GrantedHandles);
		}
	}

	Equipped.Add(Equipment);
	OnEquipmentChanged.Broadcast(Slot, Equipment);
	return Equipment;
}

bool UEquipmentComponent::UnequipItem(UItemInstance *Item)
{
	UEquipmentInstance *Equipment = FindEquipmentForItem(Item);
	if (!Equipment)
	{
		return false;
	}

	const FGameplayTag Slot = Equipment->Slot;
	Remove(*Equipment);
	OnEquipmentChanged.Broadcast(Slot, nullptr);
	return true;
}

UEquipmentInstance *UEquipmentComponent::FindEquipmentForItem(const UItemInstance *Item) const
{
	if (!Item)
	{
		return nullptr;
	}

	for (UEquipmentInstance *Equipment : Equipped)
	{
		if (Equipment && Equipment->Item.Get() == Item)
		{
			return Equipment;
		}
	}
	return nullptr;
}

UEquipmentInstance *UEquipmentComponent::GetEquipmentInSlot(FGameplayTag Slot) const
{
	for (UEquipmentInstance *Equipment : Equipped)
	{
		// Exact: a slot is one specific place, the parent-tag matching is only for "which slots fit"
		if (Equipment && Equipment->Slot == Slot)
		{
			return Equipment;
		}
	}
	return nullptr;
}

FGameplayTag UEquipmentComponent::FindSlotFor(const UEquipmentDefinition &Definition) const
{
	FGameplayTag FirstFit;
	for (const FGameplayTag &Slot : Slots)
	{
		// "Equipment.Slot.Ring.Left" fits "Equipment.Slot.Ring". An empty AllowedSlot fits nothing.
		if (!Slot.MatchesTag(Definition.AllowedSlot))
		{
			continue;
		}

		if (!GetEquipmentInSlot(Slot))
		{
			return Slot;
		}

		if (!FirstFit.IsValid())
		{
			FirstFit = Slot;
		}
	}

	// Every slot it fits is taken: the first one gets replaced
	return FirstFit;
}

void UEquipmentComponent::SpawnEquipmentActor(UEquipmentInstance &Equipment, const UEquipmentDefinition &Definition)
{
	if (!Definition.EquipmentActorClass)
	{
		return;
	}

	UWorld *World = GetWorld();
	ACharacter *Character = Cast<ACharacter>(GetOwner());
	if (!World || !Character)
	{
		UE_LOG(LogRougelike, Warning, TEXT("%s: equipment actors need a character owner"), *GetNameSafe(GetOwner()));
		return;
	}

	FActorSpawnParameters SpawnParams;
	// Owner: who it belongs to. Instigator: who is responsible for what it does, e.g. traces that ignore the wielder.
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;
	// It spawns at the origin and snaps to the hand right after; overlapping something there must not cancel it
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor *Actor = World->SpawnActor<AActor>(Definition.EquipmentActorClass, FTransform::Identity, SpawnParams);
	if (!Actor)
	{
		UE_LOG(LogRougelike, Warning, TEXT("%s: failed to spawn %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Definition.EquipmentActorClass));
		return;
	}

	Actor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Definition.EquipmentSocketName);
	Equipment.SpawnedActor = Actor;
}

void UEquipmentComponent::Remove(UEquipmentInstance &Equipment)
{
	// Grants first: a running swing ends while its weapon still exists, so its end logic can still reach it
	Equipment.GrantedHandles.Revoke();

	if (Equipment.SpawnedActor)
	{
		Equipment.SpawnedActor->Destroy();
		Equipment.SpawnedActor = nullptr;
	}

	Equipped.Remove(&Equipment);
}

void UEquipmentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Destroying the owner only detaches attached actors, it doesn't destroy them
	for (int32 Index = Equipped.Num() - 1; Index >= 0; --Index)
	{
		if (UEquipmentInstance *Equipment = Equipped[Index])
		{
			Remove(*Equipment);
		}
	}
	Equipped.Reset();

	Super::EndPlay(EndPlayReason);
}
