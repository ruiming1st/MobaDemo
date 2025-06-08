// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NonMobyGamePlayer.h"
#include "MobyGameMonsterCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API AMobyGameMonsterCharacter : public ANonMobyGamePlayer
{
	GENERATED_BODY()
	
public:
	virtual void InitCharacter();

protected:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
