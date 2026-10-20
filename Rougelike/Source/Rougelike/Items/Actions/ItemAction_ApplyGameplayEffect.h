#pragma once

#include "CoreMinimal.h"
#include "Items/Actions/ItemAction.h"
#include "ItemAction_ApplyGameplayEffect.generated.h"

class UGameplayEffect;

/** Applies an effect to the user, with the item as source */
UCLASS(DisplayName = "Apply Gameplay Effect")
class ROUGELIKE_API UItemAction_ApplyGameplayEffect : public UItemAction
{
	GENERATED_BODY()

public:
	virtual bool Execute_Implementation(AActor *ItemOwner, UItemInstance *ItemInstance) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UGameplayEffect> GameplayEffect;
};
