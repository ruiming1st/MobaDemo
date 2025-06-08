// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MobyGame/Data/MisData.h"


class AMobyGamePlayerState;
class UUI_Tip;
class UWidget;

class IUI_TipInterface
{
public:
	IUI_TipInterface();

protected:
	UWidget* VerifyTip(AMobyGamePlayerState* InPlayerState, UUI_Tip *InTip, int32 InSlotID, const FSlotData* ClientSlotData = NULL);
};