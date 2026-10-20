#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemTypes.h"
#include "InventoryComponent.generated.h"

class AItemPickup;
class UItemDefinition;
class UItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

/**
 * Owns the items a character carries: stacking, use and drop.
 * Items arrive and leave as data (FStackInfo); anything leaving is unequipped first.
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class ROUGELIKE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	/**
	 * Adds Count items of this type: fills stacks with room first, then starts new ones.
	 * ItemStats is the items' saved state (empty for fresh items); new stacks are built from it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (AutoCreateRefTerm = "ItemStats"))
	void AddItem(TSubclassOf<UItemDefinition> ItemDefinition, int32 Count, const TMap<FGameplayTag, float> &ItemStats);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryUseItem(int32 Index);

	/** Unequips it, then drops the whole stack in front of the owner as a pickup. False if it can't be dropped. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropItem(int32 Index);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FItemStack> &GetItems() const { return Items; }

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<AItemPickup> PickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (ClampMin = "0", Units = "cm"))
	float DropDistance = 200.f;

private:
	// Puts Count into stacks of this type that have room. Returns what didn't fit.
	int32 FillStacksWithRoom(TSubclassOf<UItemDefinition> ItemDefinition, int32 Count);

	// The one way out: unequips the item, then removes the stack
	void RemoveStackAt(int32 Index);

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Inventory")
	TArray<FItemStack> Items;
};
