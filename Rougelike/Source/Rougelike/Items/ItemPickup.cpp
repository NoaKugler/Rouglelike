#include "Items/ItemPickup.h"
#include "Items/ItemDefinition.h"
#include "Inventory/InventoryComponent.h"

AItemPickup::AItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AItemPickup::NotifyActorBeginOverlap(AActor *OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Two of our components can start overlapping in the same frame
	if (IsActorBeingDestroyed())
	{
		return;
	}

	UInventoryComponent *Inventory = OtherActor ? OtherActor->FindComponentByClass<UInventoryComponent>() : nullptr;
	if (!Inventory || !ItemStack.ItemDefinition)
	{
		return;
	}

	Inventory->AddItem(ItemStack.ItemDefinition, ItemStack.Count, ItemStack.ItemStats);
	Destroy();
}
