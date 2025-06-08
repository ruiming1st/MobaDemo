// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_StopMovement.h"
#include "../CharacterInstance/MobyGameCharacter.h"

void UAnimNotifyState_StopMovement::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (AMobyGameCharacter* Character = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
	{
		if (Character->GetWorld()->IsServer())
		{
			Character->ResetSpeed(0.f);
		}

	}
}

void UAnimNotifyState_StopMovement::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (AMobyGameCharacter* Character = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
	{
		if (Character->GetWorld()->IsServer())
		{
			Character->ResetSpeed(INDEX_NONE);
		}

	}
}
