// Fill out your copyright notice in the Description page of Project Settings.


#include "MobyGameAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../SimpleThread/Source/SimpleThread/Public/ThreadManage.h"

AMobyGameAIController::AMobyGameAIController()
{
	//Behavior_Tree是一个结构体
	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> Behavior_Tree(TEXT("/Game/TopDownCPP/AI/ABH"));
	//BTAsset = Behavior_Tree.Object;
	HomePoint = FVector::ZeroVector;
	bStopAttackTarget = false;
}

void AMobyGameAIController::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		GThread::Get()->GetCoroutines().BindUObject(0.1f, this, &AMobyGameAIController::InitController);
	} 
}

void AMobyGameAIController::InitController()
{
	if (BTAsset)
	{
		RunBehaviorTree(BTAsset);
	}
}

void AMobyGameAIController::NormalAttack(TWeakObjectPtr<AMobyGameCharacter> InTarget)
{
	if (AMobyGameCharacter* ContorllerPawn = Cast<AMobyGameCharacter>(GetPawn()))
	{
		ContorllerPawn->NormalAttack(InTarget);
	}
}

void AMobyGameAIController::SetTargetForce(AMobyGameCharacter* InTarget)
{
	if (!bStopAttackTarget)
	{
		if (GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsObject("Target", InTarget);
		}
	}
}

AMobyGameCharacter* AMobyGameAIController::FindTarget()
{
	return NULL;
}

void AMobyGameAIController::SimpleMoveToLocation(const FVector& InNewLocation)
{
	if (GetBlackboardComponent())
	{
		SetTargetForce(NULL);
		GetBlackboardComponent()->SetValueAsVector("Location", InNewLocation);
	}

}

FVector AMobyGameAIController::GetGoalLocation()
{
	if (GetBlackboardComponent())
	{
		return GetBlackboardComponent()->GetValueAsVector("Location");
	}

	return FVector::ZeroVector;
}

AMobyGameCharacter* AMobyGameAIController::GetTarget()
{
	if (GetBlackboardComponent())
	{
		return Cast<AMobyGameCharacter>(GetBlackboardComponent()->GetValueAsObject("Target"));
	}

	return NULL;
}

AMobyGameCharacter* AMobyGameAIController::GetMissionTarget()
{
	return  NULL;
}

void AMobyGameAIController::ClearMission()
{
	SetTargetForce(NULL);
	SimpleMoveToLocation(GetPawn()->GetActorLocation());
}

void AMobyGameAIController::SetStopAttack(bool bNewStop)
{
	bStopAttackTarget = bNewStop;
}

void AMobyGameAIController::StopAttackRot(bool bAttackRot)
{
	if (GetBlackboardComponent())
	{
		GetBlackboardComponent()->SetValueAsBool("bStopRot", bAttackRot);
	}
}

