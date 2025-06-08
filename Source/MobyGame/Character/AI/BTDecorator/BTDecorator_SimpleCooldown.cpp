// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_SimpleCooldown.h"
#include "../../CharacterInstance/MobyGameCharacter.h"
#include "../AIController/Core/MobyGameAIController.h"

bool UBTDecorator_SimpleCooldown::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (AMobyGameAIController* OwnerController = Cast<AMobyGameAIController>(OwnerComp.GetOwner()))
	{
		if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(OwnerController->GetPawn()))
		{
			float* CD = const_cast<float*>(&CoolDownTime);
			*CD = 1 / InCharacter->GetCharacterAttribute()->AttackSpeed;
		}
	}

	return Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
}
