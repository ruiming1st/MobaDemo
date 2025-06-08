// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MobyGameCharacter.h"
#include "NonMobyGamePlayer.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API ANonMobyGamePlayer : public AMobyGameCharacter
{
	GENERATED_BODY()
	
public:
	virtual void Die();
};
