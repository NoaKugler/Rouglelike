#pragma once

#include "Engine/EngineTypes.h"

// Must match Project Settings > Collision. Numbered in creation order, check DefaultEngine.ini.
inline constexpr ECollisionChannel ECC_Projectile = ECC_GameTraceChannel1;
inline constexpr ECollisionChannel ECC_Weapon = ECC_GameTraceChannel2;
