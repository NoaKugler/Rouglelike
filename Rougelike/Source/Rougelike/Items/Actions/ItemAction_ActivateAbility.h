#pragma once

#include "CoreMinimal.h"
#include "Items/Actions/ItemAction.h"
#include "ItemAction_ActivateAbility.generated.h"

class URougelikeGameplayAbility;

/** Grants an ability for one activation, with the item as source. Fails (and isn't consumed) if it can't activate. */
UCLASS(DisplayName = "Activate Ability")
class ROUGELIKE_API UItemAction_ActivateAbility : public UItemAction
{
	GENERATED_BODY()

public:
	virtual bool Execute_Implementation(AActor *ItemOwner, UItemInstance *ItemInstance) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<URougelikeGameplayAbility> Ability;
};
