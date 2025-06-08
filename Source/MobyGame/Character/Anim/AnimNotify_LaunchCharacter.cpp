// Fill out your copyright notice in the Description page of Project Settings.
#include "AnimNotify_LaunchCharacter.h"
#include "../CharacterInstance/MobyGameCharacter.h"
#include "../AI/AIController/Core/MobyGameAIController.h"

UAnimNotify_LaunchCharacter::UAnimNotify_LaunchCharacter()
{
	LaunchValue = 0.f;

#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(153, 12, 25, 255);
#endif // WITH_EDITORONLY_DATA
}

void UAnimNotify_LaunchCharacter::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
	{
		if (InCharacter->GetWorld()->IsServer())
		{	
			if (AMobyGameAIController *InContorller = Cast<AMobyGameAIController>(InCharacter->GetController()))
			{
				FVector NewPos = InCharacter->GetActorForwardVector() * 20.f + InCharacter->GetActorLocation();
				InContorller->SimpleMoveToLocation(NewPos);

				InCharacter->LaunchCharacter(InCharacter->GetActorForwardVector() * LaunchValue, true, true);
			}
		}
	}
}
