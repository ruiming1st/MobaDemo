// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_ContinuousAnim.h"
#include "../CharacterInstance/MobyGamePlayerCharacter.h"

UAnimNotifyState_ContinuousAnim::UAnimNotifyState_ContinuousAnim()
{
	bEndContinuousAnim = false;
}

void UAnimNotifyState_ContinuousAnim::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	bEndContinuousAnim = false;

}

void UAnimNotifyState_ContinuousAnim::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (!bEndContinuousAnim)
	{
		if (AActor* Character = Cast<AActor>(MeshComp->GetOuter()))
		{
			if (Character->GetWorld()->IsServer())
			{
				if (AMobyGamePlayerCharacter* NewCharacter = Cast<AMobyGamePlayerCharacter>(MeshComp->GetOuter()))
				{
					if (NewCharacter->bPressSkillKey)
					{
						NewCharacter->MultiCastPlayerAnimMontage(LoopAnim);
					}
					else
					{
						bEndContinuousAnim = true;
						NewCharacter->MultiCastPlayerAnimMontage(EndAnim);

						NewCharacter->CallBulletEndOpenFire();
					}
				}
			}
		}
	}
}

void UAnimNotifyState_ContinuousAnim::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (!bEndContinuousAnim)
	{
		if (AActor* Character = Cast<AActor>(MeshComp->GetOuter()))
		{
			if (Character->GetWorld()->IsServer())
			{
				if (AMobyGamePlayerCharacter* NewCharacter = Cast<AMobyGamePlayerCharacter>(MeshComp->GetOuter()))
				{
					if (!NewCharacter->bPressSkillKey)
					{
						bEndContinuousAnim = true;
						NewCharacter->MultiCastPlayerAnimMontage(EndAnim);

						NewCharacter->CallBulletEndOpenFire();
					}
				}
			}
		}
	}
}