#include "Enemy/UI/EnemyHealthBar.h"
#include "Components/ProgressBar.h"

void UEnemyHealthBar::SetHealth(float Current, float Max)
{
    HealthBar->SetPercent(Max > 0.f ? Current / Max : 0.f);
}
