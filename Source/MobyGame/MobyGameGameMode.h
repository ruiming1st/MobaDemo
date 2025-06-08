// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MobyGameGameMode.generated.h"

UCLASS(minimalapi)
class AMobyGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMobyGameGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void PostLogin(APlayerController* NewPlayer);
};





