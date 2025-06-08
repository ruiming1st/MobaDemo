// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MobyGameAIController.h"
#include "MonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API AMonsterAIController : public AMobyGameAIController
{
	GENERATED_BODY()
	
public:
	AMonsterAIController();

	virtual void InitContorller();

	virtual AMobyGameCharacter* FindTarget();

	virtual void Tick(float DeltaSeconds) override;

	void CheckRange();
};
