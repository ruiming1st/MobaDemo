// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_StopCharacterRot.generated.h"

/**
 *
 */
UCLASS()
class MOBYGAME_API UAnimNotifyState_StopCharacterRot : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_StopCharacterRot();

	//≥Â¡ø
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
		float Time;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

};
