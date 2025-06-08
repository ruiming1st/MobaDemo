// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ContinuousAnim_Start.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API UAnimNotify_ContinuousAnim_Start : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_ContinuousAnim_Start();
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	UAnimMontage* LoopAnim;
};
