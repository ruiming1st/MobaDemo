// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_BuffSlot.h"
#include "Components/Image.h"
#include "MobyGame/Table/SlotTable.h"
#include "../../../Common/MethodUnit.h"

UUI_BuffSlot::UUI_BuffSlot()
{
	CurrentBuffCD = 999999.f;
}

void UUI_BuffSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_BuffSlot::UpdateCD(int32 InSlotID, float InMaxCD)
{
	BuildSlot.MaxCD = InMaxCD;
	BuildSlot.SlotID = InSlotID;
	CurrentBuffCD = InMaxCD;

	if (AMobyGamePlayerState *InPlayerState = MethodUnit::GetPlayerState(GetWorld()))
	{
		if (const FSlotTable* InTable = InPlayerState->GetSlotTableTemplate(InSlotID))
		{
			UpdateIcon(InTable->SlotIcon);
		}
	}
}

void UUI_BuffSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CurrentBuffCD -= InDeltaTime;
	DrawSlotCDMat(CurrentBuffCD / BuildSlot.MaxCD);
	if (CurrentBuffCD <= 0.f)
	{
		CurrentBuffCD = 0.f;

		RemoveFromParent();
	}
}

void UUI_BuffSlot::SetCurrentBuffCD(float InCD)
{
	CurrentBuffCD = InCD;
}
