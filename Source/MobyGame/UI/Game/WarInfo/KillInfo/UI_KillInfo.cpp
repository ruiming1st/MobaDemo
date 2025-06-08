// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_KillInfo.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MobyGame/Data/MisData.h"

void UUI_KillInfo::UpdateSlot(const FPlayerKillInfoNetPackage& InKillInfo)
{
	KillInfoText->SetText(FText::FromString(FString::Printf(TEXT("%2d/%2d/%2d"),
		InKillInfo.Kill,
		InKillInfo.Killed,
		InKillInfo.Assists)));

	NumberOfSoldiersKilled->SetText(FText::FromString(FString::Printf(TEXT("%2d"),
		InKillInfo.KillDogFace)));
}

