// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MobyGameArchitectureCharacter.h"
#include "MobyGameMasterCharacter.generated.h"

class ABullet;

/**
 * 
 */
UCLASS()
class MOBYGAME_API AMobyGameMasterCharacter : public AMobyGameArchitectureCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Bullet", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABullet> Bullet;

public:
	AMobyGameMasterCharacter();

protected:
	virtual void Tick(float DeltaSeconds) override;

	virtual void InitCharacter();
};
