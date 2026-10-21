#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ItemTypes.h"
#include "ItemPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * An item lying in the world, as plain data. Placed and dropped pickups work the same way:
 * whoever picks it up rebuilds the item from this data. The look comes from the item's World Representation fragment.
 */
UCLASS()
class ROUGELIKE_API AItemPickup : public AActor
{
	GENERATED_BODY()

public:
	AItemPickup();

	/** Call between SpawnActorDeferred and FinishSpawning, so construction already sees it */
	void SetItemStack(const FStackInfo &InItemStack) { ItemStack = InItemStack; }

protected:
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor *OtherActor) override;

#if WITH_EDITOR
	virtual void BeginDestroy() override;
#endif

	/** Placed pickups set this in the level; dropped ones get it from the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FStackInfo ItemStack;

	/** The pickup range, independent of how big the mesh is */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

private:
	/** Mesh and scale from the item's World Representation fragment */
	void ApplyWorldRepresentation();

#if WITH_EDITOR
	/** Placed pickups follow edits to their item definition (or its fragments) instead of going stale */
	void HandleObjectPropertyChanged(UObject *Object, FPropertyChangedEvent &PropertyChangedEvent);

	FDelegateHandle PropertyChangedHandle;
#endif
};
