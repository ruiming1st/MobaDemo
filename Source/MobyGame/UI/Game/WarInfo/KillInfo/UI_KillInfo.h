// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../Core/UI_Base.h"
#include "UI_KillInfo.generated.h"

class UTextBlock;
struct FPlayerKillInfoNetPackage;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_KillInfo : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillInfoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NumberOfSoldiersKilled;

public:
	void UpdateSlot(const FPlayerKillInfoNetPackage &InKillInfo);
};
