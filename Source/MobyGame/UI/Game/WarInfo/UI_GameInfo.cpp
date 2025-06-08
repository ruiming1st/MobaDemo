// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_GameInfo.h"
#include "Components/TextBlock.h"
#include "ThreadManage.h"
#include "KillInfo/UI_KillInfo.h"
#include "MobyGame/Data/MisData.h"

void UUI_GameInfo::NativeConstruct()
{
	Super::NativeConstruct();

	//Bind
	BindDelegate();
}

void UUI_GameInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TimeText && GetWorld())
	{
		TimeText->SetText(FText::FromString(GetCurrentCount(GetWorld()->GetRealTimeSeconds())));
	}
}

void UUI_GameInfo::BindDelegate()
{
	if (AMobyGamePlayerState* InGamePlayerState = GetPlayerState())
	{
		InGamePlayerState->SimplePlayerKillInfoDelegate.BindLambda(
			[&](const FPlayerKillInfoNetPackage& InNetPackage)
		{
			KillInfo->UpdateSlot(InNetPackage);
		});

		InGamePlayerState->TeamKillInfoDelegate.BindLambda(
			[&](int32 InFriendly, int32 InEnemy)
		{
			NumberOfFriendlyKilledText->SetText(FText::FromString(FString::FromInt(InEnemy)));
			NumberOfEnemyKilledText->SetText(FText::FromString(FString::FromInt(InFriendly)));
		});
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]() { BindDelegate(); });
	}
}

FString UUI_GameInfo::GetCurrentCount(float NewTimeCount)
{
	const int32 NewOurTime = FMath::Max(0, FMath::TruncToInt(NewTimeCount));
	const int32 NewMinrutes = NewOurTime / 60;
	const int32 NewSeconds = NewOurTime % 60;
	return FString::Printf(TEXT("%02d:%02d"), NewMinrutes, NewSeconds); //00 : 00
}
