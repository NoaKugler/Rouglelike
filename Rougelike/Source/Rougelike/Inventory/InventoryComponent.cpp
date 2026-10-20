#include "Inventory/InventoryComponent.h"
#include "Items/ItemInstance.h"
#include "Items/ItemDefinition.h"
#include "Items/ItemPickup.h"
#include "Items/Fragments/ItemFragment_Stackable.h"
#include "Items/Fragments/ItemFragment_Usable.h"
#include "Items/Fragments/ItemFragment_Durability.h"
#include "Items/Fragments/ItemFragment_WorldRepresentation.h"
#include "Equipment/EquipmentComponent.h"
#include "Engine/World.h"
#include "Rougelike.h"

namespace
{
	// Items without a Stackable fragment stack to 1
	int32 GetMaxStackSize(TSubclassOf<UItemDefinition> ItemDefinition)
	{
		const UItemFragment_Stackable *Stackable = Cast<UItemFragment_Stackable>(UItemDefinition::FindFragmentByClass(ItemDefinition, UItemFragment_Stackable::StaticClass()));
		return Stackable ? Stackable->MaxStackSize : 1;
	}
}

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddItem(TSubclassOf<UItemDefinition> ItemDefinition, int32 Count, const TMap<FGameplayTag, float> &ItemStats)
{
	if (!ItemDefinition || Count <= 0)
	{
		return;
	}

	const int32 MaxStackSize = GetMaxStackSize(ItemDefinition);
	int32 Remaining = FillStacksWithRoom(ItemDefinition, Count);
	while (Remaining > 0)
	{
		// One instance per new stack, rebuilt from the data, like the Blueprint version
		FItemStack &NewStack = Items.AddDefaulted_GetRef();
		NewStack.ItemInstance = UItemInstance::CreateItem(this, ItemDefinition, ItemStats);
		NewStack.Count = FMath::Min(MaxStackSize, Remaining);
		Remaining -= NewStack.Count;
	}
	OnInventoryChanged.Broadcast();
}

int32 UInventoryComponent::FillStacksWithRoom(TSubclassOf<UItemDefinition> ItemDefinition, int32 Count)
{
	// Same order as the recursive Blueprint version: the first stack with room fills up before the next one is touched
	const int32 MaxStackSize = GetMaxStackSize(ItemDefinition);
	for (FItemStack &Stack : Items)
	{
		if (Count <= 0)
		{
			break;
		}

		if (Stack.ItemInstance && Stack.ItemInstance->ItemDefinition == ItemDefinition && Stack.Count < MaxStackSize)
		{
			const int32 Added = FMath::Min(MaxStackSize - Stack.Count, Count);
			Stack.Count += Added;
			Count -= Added;
		}
	}
	return Count;
}

bool UInventoryComponent::TryUseItem(int32 Index)
{
	if (!Items.IsValidIndex(Index))
	{
		return false;
	}

	UItemInstance *Item = Items[Index].ItemInstance;
	const UItemFragment_Usable *Usable = Item ? Cast<UItemFragment_Usable>(Item->FindFragmentByClass(UItemFragment_Usable::StaticClass())) : nullptr;
	if (!Usable)
	{
		return false;
	}

	// Limited uses: the item stays, its counter goes down
	if (const UItemFragment_Durability *Durability = Cast<UItemFragment_Durability>(Item->FindFragmentByClass(UItemFragment_Durability::StaticClass())))
	{
		if (!Durability->HasDurabilityLeft(Item) || !Usable->Use(GetOwner(), Item))
		{
			return false;
		}
		Durability->DecrementDurability(Item);
		OnInventoryChanged.Broadcast();
		return true;
	}

	const bool bUsed = Usable->Use(GetOwner(), Item);
	if (bUsed && Usable->bConsumeOnUse)
	{
		// The actions ran arbitrary code; find the stack again instead of trusting the old index
		const int32 CurrentIndex = Items.IndexOfByPredicate([Item](const FItemStack &Stack) { return Stack.ItemInstance == Item; });
		if (CurrentIndex != INDEX_NONE)
		{
			if (Items[CurrentIndex].Count <= 1)
			{
				RemoveStackAt(CurrentIndex);
			}
			else
			{
				--Items[CurrentIndex].Count;
			}
		}
		OnInventoryChanged.Broadcast();
	}
	return bUsed;
}

bool UInventoryComponent::DropItem(int32 Index)
{
	if (!Items.IsValidIndex(Index) || !PickupClass)
	{
		return false;
	}

	const FItemStack Stack = Items[Index];
	const UItemFragment_WorldRepresentation *WorldFragment = Stack.ItemInstance ? Cast<UItemFragment_WorldRepresentation>(Stack.ItemInstance->FindFragmentByClass(UItemFragment_WorldRepresentation::StaticClass())) : nullptr;
	if (!WorldFragment || !WorldFragment->bCanBeDropped)
	{
		return false;
	}

	UWorld *World = GetWorld();
	const AActor *Owner = GetOwner();
	const FTransform SpawnTransform(Owner->GetActorLocation() + Owner->GetActorForwardVector() * DropDistance);

	// Deferred: the pickup gets its data before its construction script picks the mesh
	AItemPickup *Pickup = World ? World->SpawnActorDeferred<AItemPickup>(PickupClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn) : nullptr;
	if (!Pickup)
	{
		UE_LOG(LogRougelike, Warning, TEXT("%s: failed to spawn a pickup, %s stays in the inventory"), *GetNameSafe(Owner), *GetNameSafe(Stack.ItemInstance));
		return false;
	}

	// The item becomes data; the instance itself is discarded with the stack
	Pickup->SetItemStack(Stack.ItemInstance->ToStackInfo(Stack.Count));
	RemoveStackAt(Index);
	Pickup->FinishSpawning(SpawnTransform);

	OnInventoryChanged.Broadcast();
	return true;
}

void UInventoryComponent::RemoveStackAt(int32 Index)
{
	if (!Items.IsValidIndex(Index))
	{
		return;
	}

	// Nothing an item granted may stay behind once the item is gone
	if (UEquipmentComponent *Equipment = GetOwner()->FindComponentByClass<UEquipmentComponent>())
	{
		Equipment->UnequipItem(Items[Index].ItemInstance);
	}
	Items.RemoveAt(Index);
}
