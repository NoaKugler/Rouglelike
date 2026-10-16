#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.generated.h"

class UProgressBar;

UCLASS(Abstract)
class ROUGELIKE_API UEnemyHealthBar : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION()
    void SetHealth(float Current, float Max);

protected:
    // The widget BP must contain a ProgressBar with exactly this name, or it won't compile
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;
};
