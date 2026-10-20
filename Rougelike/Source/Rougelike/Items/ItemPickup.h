#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ItemTypes.h"
#include "ItemPickup.generated.h"

/**
 * An item lying in the world, as plain data. Placed and dropped pickups work the same way:
 * whoever picks it up rebuilds the item from this data. Blueprint children add the visuals.
 */
UCLASS(Abstract)
class ROUGELIKE_API AItemPickup : public AActor
{
	GENERATED_BODY()

public:
	AItemPickup();

	/** Call between SpawnActorDeferred and FinishSpawning, so the construction script already sees it */
	void SetItemStack(const FStackInfo &InItemStack) { ItemStack = InItemStack; }

protected:
	virtual void NotifyActorBeginOverlap(AActor *OtherActor) override;

	/** Placed pickups set this in the level; dropped ones get it from the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FStackInfo ItemStack;
};
