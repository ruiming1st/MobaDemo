// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ContinuousAnim_Start.h"
#include "../CharacterInstance/MobyGameCharacter.h"

UAnimNotify_ContinuousAnim_Start::UAnimNotify_ContinuousAnim_Start()
{

#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(126, 112, 251, 255);
#endif // WITH_EDITORONLY_DATA
}

FString UAnimNotify_ContinuousAnim_Start::GetNotifyName_Implementation() const
{
	if (LoopAnim)
	{
		return LoopAnim->GetName();
	}

	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ContinuousAnim_Start::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!LoopAnim)
	{
		return;
	}

	if (AActor* Character = Cast<AActor>(MeshComp->GetOuter()))
	{
		if (Character->GetWorld()->IsServer())
		{
			if (AMobyGameCharacter* NewCharacter = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
			{
				NewCharacter->MultiCastPlayerAnimMontage(LoopAnim);
			}
		}
	}
}
