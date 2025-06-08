// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MobyGameAIController.h"
#include "TurretsAIController.generated.h"

/**
 * 
 */
class AMobyGameCharacter;
UCLASS()
class MOBYGAME_API ATurretsAIController : public AMobyGameAIController
{
	GENERATED_BODY()
	
public:
	ATurretsAIController();

	virtual void Tick(float DeltaTime) override;

	virtual AMobyGameCharacter* FindTarget();
	virtual void SetTarget(AMobyGameCharacter* InTarget);

	virtual void InitController();


protected:
	bool bIntruder;
};
