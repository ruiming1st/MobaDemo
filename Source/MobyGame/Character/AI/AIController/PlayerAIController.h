// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MobyGameAIController.h"
#include "PlayerAIController.generated.h"


/**
 * 
 */
UCLASS()
class MOBYGAME_API APlayerAIController : public AMobyGameAIController
{
	GENERATED_BODY()
	
public:
	APlayerAIController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void InitContorller();

	virtual AMobyGameCharacter* FindTarget();

	virtual void SimpleMoveToLocation(const FVector& InNewLocation);

	virtual void SetStopAttack(bool bNewStop);
private:

	bool bAutoFindTarget;
};
