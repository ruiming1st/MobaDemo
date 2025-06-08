// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDecorator_AttackRange.h"
#include "../AIController/Core/MobyGameAIController.h"
#include "../../CharacterInstance/MobyGameCharacter.h"

void UBTDecorator_AttackRange::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	Super::InitializeMemory(OwnerComp, NodeMemory, InitType);

	if (AMobyGameAIController* OwnerController = Cast<AMobyGameAIController>(OwnerComp.GetOwner()))
	{
		if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(OwnerController->GetPawn()))
		{
			float *fv = const_cast<float *>(&FloatValue);
			if (FCharacterAttribute* InCharacterAttribute = InCharacter->GetCharacterAttribute())
			{
				*fv = InCharacterAttribute->RangeAttack;
			}		
		}
	}
}