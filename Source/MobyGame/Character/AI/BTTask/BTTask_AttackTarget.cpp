// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AttackTarget.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AIController/Core/MobyGameAIController.h"

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (Blackboard_Actor.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		if (UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent())
		{
			if (AMobyGameAIController* AIController = Cast<AMobyGameAIController>(OwnerComp.GetOwner()))
			{
				if (AMobyGameCharacter* InTarget = Cast<AMobyGameCharacter>(MyBlackBoard->GetValueAsObject(Blackboard_Actor.SelectedKeyName)))
				{
					AIController->NormalAttack(InTarget);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_AttackTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData *BBAsset = GetBlackboardAsset())
	{
		Blackboard_Actor.ResolveSelectedKey(*BBAsset);
	}
}
