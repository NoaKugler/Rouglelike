#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RougelikeInputConfig.generated.h"

class UInputAction;

/** One ability input: the action and the tag it sends */
USTRUCT()
struct FAbilityInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<const UInputAction> InputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * Which input actions send which input tags. The grant sets decide which ability answers a tag.
 * Ability actions must not have triggers: the ability's activation policy decides press vs hold.
 */
UCLASS()
class ROUGELIKE_API URougelikeInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (TitleProperty = "InputTag"))
	TArray<FAbilityInputAction> AbilityInputActions;
};
