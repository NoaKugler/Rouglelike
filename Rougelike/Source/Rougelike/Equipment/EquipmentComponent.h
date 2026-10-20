#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "EquipmentComponent.generated.h"

class UEquipmentDefinition;
class UEquipmentInstance;
class UItemInstance;

/** Slot: the slot that changed. Equipment: what is in it now, or null if it was emptied. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, FGameplayTag, Slot, UEquipmentInstance *, Equipment);

/**
 * Equips items into slots. Each owner lists its slots as tags; each equipment definition says which slots it fits.
 * Knows no item types and no slot names.
 */
UCLASS(ClassGroup = (Equipment), meta = (BlueprintSpawnableComponent))
class ROUGELIKE_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentComponent();

	/**
	 * Puts the item into the first free slot it fits, or replaces the first one it fits if all are taken.
	 * Returns the record, or null if the item can't be equipped here. Equipping an equipped item changes nothing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	UEquipmentInstance *EquipItem(UItemInstance *Item);

	/** Returns false if the item wasn't equipped */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipItem(UItemInstance *Item);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UEquipmentInstance *FindEquipmentForItem(const UItemInstance *Item) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UEquipmentInstance *GetEquipmentInSlot(FGameplayTag Slot) const;

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentChanged OnEquipmentChanged;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** The slots this owner has, in the order items fill them */
	UPROPERTY(EditDefaultsOnly, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	TArray<FGameplayTag> Slots;

private:
	// Invalid if the definition fits none of our slots
	FGameplayTag FindSlotFor(const UEquipmentDefinition &Definition) const;

	void SpawnEquipmentActor(UEquipmentInstance &Equipment, const UEquipmentDefinition &Definition);

	// Undoes what equipping did, without broadcasting. Shared by unequip, replace and EndPlay.
	void Remove(UEquipmentInstance &Equipment);

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Equipment")
	TArray<TObjectPtr<UEquipmentInstance>> Equipped;
};
