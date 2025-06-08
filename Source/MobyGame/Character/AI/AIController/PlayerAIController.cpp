// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAIController.h"
#include "../../../Common/MethodUnit.h"

APlayerAIController::APlayerAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> PlayerBehavior_Tree(TEXT("/Game/TopDownCPP/AI/Player/PlayerBH"));
	BTAsset = PlayerBehavior_Tree.Object;

	bAutoFindTarget = true;
}

void APlayerAIController::SimpleMoveToLocation(const FVector& InNewLocation)
{
	Super::SimpleMoveToLocation(InNewLocation);

	bAutoFindTarget = false;
}

void APlayerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bAutoFindTarget)
	{
		if (AMobyGameCharacter* InOwner = GetPawn<AMobyGameCharacter>())
		{
			FVector GoalPos = GetGoalLocation();
			if (GoalPos != FVector::ZeroVector)
			{
				float Distance = FVector::Dist(GoalPos, InOwner->GetActorLocation());
				if (Distance <= 80.f)
				{
					bAutoFindTarget = true;
				}
			}
		}
	}
}

void APlayerAIController::InitContorller()
{
	Super::InitController();
}

void APlayerAIController::SetStopAttack(bool bNewStop)
{
	Super::SetStopAttack(bNewStop);

	bAutoFindTarget = false;
}

AMobyGameCharacter* APlayerAIController::FindTarget()
{
	if (bAutoFindTarget)
	{
		if (AMobyGameCharacter* InOwner = GetPawn<AMobyGameCharacter>())
		{
			return MethodUnit::FindTargetInRange(GetWorld(), 700.f, InOwner, false);
		}
	}

	return NULL;
}
