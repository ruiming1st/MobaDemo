// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Table/Config/GlobalClientConfiguration.h"
#include "MobyGameHUD.generated.h"

class UUI_MainScreen;
/**
 *
 */
UCLASS()
class MOBYGAME_API AMobyGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMobyGameHUD();

	virtual void BeginPlay() override;

	const FGlobalClientConfiguration& GetGlobalClientConfig();
private:
	UPROPERTY()
	UDataTable* GlobalClientConfigurationPtr;

private:
	TSubclassOf<UUI_MainScreen> MainScreenClass;
	UUI_MainScreen* MainScreen;

	UPROPERTY()
	FGlobalClientConfiguration GlobalClientConfiguration;
};
