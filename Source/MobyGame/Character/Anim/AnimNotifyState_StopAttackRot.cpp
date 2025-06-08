// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_StopAttackRot.h"
#include "../CharacterInstance/MobyGameCharacter.h"
#include "../AI/AIController/Core/MobyGameAIController.h"

void UAnimNotifyState_StopAttackRot::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (AMobyGameCharacter* Character = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
	{
		
		if (Character->GetWorld()->IsServer())
		{
			if (AMobyGameAIController* AIController = Cast<AMobyGameAIController>(Character->GetController()))
			{
				AIController->StopAttackRot(true);
			}
		}
	}
}

void UAnimNotifyState_StopAttackRot::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	if (AMobyGameCharacter* Character = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
	{

		if (Character->GetWorld()->IsServer())
		{
			if (AMobyGameAIController* AIController = Cast<AMobyGameAIController>(Character->GetController()))
			{
				AIController->StopAttackRot(false);
			}

		}

	}
}
