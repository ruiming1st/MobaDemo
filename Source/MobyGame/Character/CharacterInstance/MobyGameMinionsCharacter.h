// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NonMobyGamePlayer.h"
#include "../../Data/System/AssistSystem.h"
#include "MobyGameMinionsCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API AMobyGameMinionsCharacter : public ANonMobyGamePlayer
{
	GENERATED_BODY()
	
public:
	AMobyGameMinionsCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void Hurt(bool bDie);
private:
	float WidgetTime;
};
