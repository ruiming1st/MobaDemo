// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_StopCharacterRot.h"
#include "../CharacterInstance/MobyGameCharacter.h"
#include "ThreadManage.h"

UAnimNotifyState_StopCharacterRot::UAnimNotifyState_StopCharacterRot()
{
	//锁定旋转时间 
	Time = 1.0f;
}

void UAnimNotifyState_StopCharacterRot::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (AMobyGameCharacter* Character = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
	{
		if (Character->GetWorld()->IsServer())
		{
			Character->ResetCurrentRot();
			Character->LockRot(true);
		}
	}
}

void UAnimNotifyState_StopCharacterRot::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (AMobyGameCharacter* Character = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
	{
		if (Character->GetWorld()->IsServer())
		{
			GThread::Get()->GetCoroutines().BindLambda(Time, [Character]()
			{
				if (Character)
				{
					Character->LockRot(false);
				}
			});
		}
	}
}