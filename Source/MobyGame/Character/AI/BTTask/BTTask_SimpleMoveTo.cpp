// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SimpleMoveTo.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

EBTNodeResult::Type UBTTask_SimpleMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AcceptableRadius = 950.f;

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
