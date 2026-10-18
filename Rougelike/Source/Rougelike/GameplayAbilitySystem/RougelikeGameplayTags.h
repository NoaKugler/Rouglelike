// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/** Tags the C++ relies on. Native, so they exist without an entry in DefaultGameplayTags.ini. */
namespace RougelikeGameplayTags
{
	/** Base damage of a hit or tick. Written into damage specs by whoever deals the hit, read by UDamageExecution. */
	ROUGELIKE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
}
