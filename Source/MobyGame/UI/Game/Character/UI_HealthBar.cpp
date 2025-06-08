// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_HealthBar.h"
#include "Components/ProgressBar.h"
#include "ThreadManage.h"

void UUI_HealthBar::NativeConstruct()
{
	Super::NativeConstruct();
	BindDelegate();
}

void UUI_HealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

}

void UUI_HealthBar::SetHealth(float InPercentage)
{
	if (Health)
	{
		Health->SetPercent(InPercentage);
	}
}

void UUI_HealthBar::SetColor(const FLinearColor& InColor)
{
	if (Health)
	{
		Health->SetFillColorAndOpacity(InColor);
	}
}

void UUI_HealthBar::BindDelegate()
{
	if (AMobyGameState* InState = GetGameState())
	{
		UpdateDelegateHandle = InState->UpdateBuffInfoDelegate.AddUObject(this, &UUI_HealthBar::UpdateCD);
		EndDelegateHandle = InState->EndBuffInfoDelegate.AddUObject(this, &UUI_HealthBar::EndUpdateCD);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			BindDelegate();
		});
	}
}

void UUI_HealthBar::RemoveDelegate()
{
	if (AMobyGameState* InState = GetGameState())
	{
		if (UpdateDelegateHandle.IsValid())
		{
			InState->UpdateBuffInfoDelegate.Remove(UpdateDelegateHandle);
		}

		if (EndDelegateHandle.IsValid())
		{
			InState->EndBuffInfoDelegate.Remove(EndDelegateHandle);
		}
	}
}

void UUI_HealthBar::SetPlayerID(int64 InPlayerID)
{
	PlayerID = InPlayerID;
	Buff->SetPlayerID(PlayerID);

	BindDelegate();
}

int64 UUI_HealthBar::GetPlayerID()
{
	return PlayerID;
}

void UUI_HealthBar::UpdateCD(int64 InPlayerID, int32 InSlotID, float MaxCD)
{
	if (Buff)
	{
		Buff->UpdateCD(InPlayerID, InSlotID, MaxCD);
	}
}

void UUI_HealthBar::EndUpdateCD(int64 InPlayerID, int32 InSlotID)
{
	if (Buff)
	{
		Buff->RemoveCD(InPlayerID, InSlotID);
	}
}

UUI_HealthBar::UUI_HealthBar(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PlayerID = INDEX_NONE;
}
