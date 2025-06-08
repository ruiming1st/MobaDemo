// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "BulletDamageType.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API UBulletDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	UBulletDamageType();

	const struct FSlotAttribute* SlotData;
};
