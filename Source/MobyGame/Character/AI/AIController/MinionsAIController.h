// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MobyGameAIController.h"
#include "MinionsAIController.generated.h"


class UBehaviorTree;
/**
 * 
 */
UCLASS()
class MOBYGAME_API AMinionsAIController : public AMobyGameAIController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	UBehaviorTree* AIBTAsset;

	AMinionsAIController();
	virtual void InitController();
	virtual void BeginPlay() override;

	virtual AMobyGameCharacter* FindTarget();
	virtual AMobyGameCharacter* GetMissionTarget();
	void ResetMissionTargetPtr();
	//¼ì²â
	bool CheckRange();
protected:
	void FindMissonTarget(TArray<AActor*>& OutActors);
private:
	TWeakObjectPtr<AMobyGameCharacter> MissionTargetPtr;
};
