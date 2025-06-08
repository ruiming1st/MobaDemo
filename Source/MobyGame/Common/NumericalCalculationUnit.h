// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Table/SlotAttribute.h"

class AMobyGameCharacter;

namespace NumericalCalculationUnit
{
	float GetArmor(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute = NULL);
	float GetMagicDefense(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute = NULL);
	float GetAttack(AMobyGameCharacter* InOriginal, const FSlotAttribute* InSlotAttribute = NULL);
	float GetPhysicalDamage(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute = NULL);
	float GetMagicDamage(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute = NULL);

	float GetSlotAttributeValue(const FSlotAttributeValue& InValue, float InNewValue);

	float GetDamage(AMobyGameCharacter* InTarget, AMobyGameCharacter* InEnemy, const FSlotAttribute* InSlotAttribute = NULL);
}