// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../../../CharacterInstance/MobyGameCharacter.h"
#include "MobyGameAIController.generated.h"


class MobyGameCharacter;
/**
 * 
 */
UCLASS()
class MOBYGAME_API AMobyGameAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMobyGameAIController();

	void BeginPlay();

	UFUNCTION()
	virtual void InitController();
	
	//服务器NormalAttack进行数据的计算
	void NormalAttack(TWeakObjectPtr<AMobyGameCharacter> InTarget);

	void SetTargetForce(AMobyGameCharacter* InTarget);

	virtual AMobyGameCharacter* FindTarget();

	virtual void SimpleMoveToLocation(const FVector& InNewLocation);
	FVector GetGoalLocation();

	AMobyGameCharacter* GetTarget();

	virtual AMobyGameCharacter* GetMissionTarget();

	void ClearMission();
	virtual void SetStopAttack(bool bNewStop);

	void StopAttackRot(bool bAttackRot);

public:
	FVector HomePoint;

protected:
	UPROPERTY()
	class UBehaviorTree* BTAsset;
	bool bStopAttackTarget;
};
