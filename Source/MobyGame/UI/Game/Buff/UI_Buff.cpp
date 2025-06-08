// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_Buff.h"
#include "UI_BuffSlot.h"
#include "Components/WrapBoxSlot.h"
#include "Components/WrapBox.h"

UUI_Buff::UUI_Buff()
{
	BuffSlotIconSize = FVector2D(32.f);
}

UPanelWidget* UUI_Buff::GetSlotArray()
{
	return Buf;
}

void UUI_Buff::Clear()
{
	if (Buf)
	{
		Buf->ClearChildren();
	}
}

void UUI_Buff::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_Buff::UpdateCD(int32 InSlotID, float InMaxCD)
{
	if (BuffSlotClass && InSlotID != INDEX_NONE && InMaxCD != 0.f)
	{
		if (UUI_BuffSlot* BuffSlot = CreateWidget<UUI_BuffSlot>(GetWorld(), BuffSlotClass))
		{
			if (UWrapBoxSlot* InWrapBox = Buf->AddChildToWrapBox(BuffSlot))
			{
				BuffSlot->SetSlotID(InSlotID);
				InWrapBox->SetPadding(1.f);
				BuffSlot->UpdateCD(InSlotID, InMaxCD);
				BuffSlot->SetIconSize(BuffSlotIconSize);
			}
		}
	}
}

void UUI_Buff::UpdateCD(int64 InPlayerID,int32 InSlotID, float InMaxCD)
{
	if (InPlayerID == PlayerID)
	{
		//相同的buff只更新CD
		bool bFind = false;
		CallSlotBreak<UUI_BuffSlot>([&](UUI_BuffSlot* InSlot)->bool
		{
			if (InSlot->GetSlotID() == InSlotID)
			{
				InSlot->SetCurrentBuffCD(InMaxCD);
				bFind = true;

				return false;
			}

			return true;
		});

		if (!bFind)
		{
			UpdateCD(InSlotID, InMaxCD);
		}
	}
}

void UUI_Buff::RemoveCD(int64 InPlayerID, int32 InSlotID)
{
	if (InPlayerID == PlayerID)
	{
		CallSlotBreak<UUI_BuffSlot>([&](UUI_BuffSlot* InSlot)->bool
		{
			if (InSlot->GetSlotID() == InSlotID)
			{
				InSlot->SetCurrentBuffCD(0.f);
				return false;
			}

			return true;
		});
	}
}
