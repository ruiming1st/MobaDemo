// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_Character_Top_Panel.h"
#include "Components/UniformGridPanel.h"
#include "Info/UI_CharacterInfo.h"
#include "ThreadManage.h"
#include "MobyGame/Data/MisData.h"
#include "../../Core/UI_Slot.h"
#include "../../Inventory/UI_SimpleInventorySlot.h"
#include "../../Buff/UI_Buff.h"

void UUI_Character_Top_Panel::NativeConstruct()
{
	Super::NativeConstruct();

	BindPlayerStateDelegate();
	BindGameStateDelegate();
}


UPanelWidget* UUI_Character_Top_Panel::GetSlotArray()
{
	return SlotArrayInventory;
}

void UUI_Character_Top_Panel::BindPlayerStateDelegate()
{
	if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
	{
		InPlayerState->LookPlayerInfoDelegate.BindLambda([&](const FLookPlayerInfoNetPackage& InInfoNetPackage)
		{
			CharacterInfo->SetPlayerID(InInfoNetPackage.PlayerID);

			//清除上一个记录
			CallSlotBreak<UUI_SimpleInventorySlot>([&](UUI_SimpleInventorySlot* InSlot)->bool
			{
				InSlot->ResetSlot();
				return true;
			});

			//更新我们的Slot ID
			if (InInfoNetPackage.SlotDataNetPackage.SlotIDs.Num() > 0 &&
				InInfoNetPackage.SlotDataNetPackage.Slots.Num() > 0)
			{
				int32 i = 0;
				CallSlotBreak<UUI_SimpleInventorySlot>([&](UUI_SimpleInventorySlot* InSlot)->bool
				{
					InSlot->SetSlotID(InInfoNetPackage.SlotDataNetPackage.SlotIDs[i]);
					InSlot->UpdateSlot(&InInfoNetPackage.SlotDataNetPackage.Slots[i]);
					i++;
					return true;
				});
			}

			SetVisibility(ESlateVisibility::Visible);
		});


		InPlayerState->HideOrShowTopPanelDelegate.BindLambda([&](bool bHideOrNot)
		{
			if (bHideOrNot)
			{
				SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				SetVisibility(ESlateVisibility::Hidden);
				Buff->Clear();
			}			
		});

		InPlayerState->UpdateBuffInfosDelegate.BindLambda([&](const TArray<FBuffNetPackage>& InBuffNetPackage)
		{
			for (auto &Tmp : InBuffNetPackage)
			{
				Buff->UpdateCD(Tmp.SlotID, Tmp.MaxCD);
			}
		});
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			BindPlayerStateDelegate();
		});
	}
}

void UUI_Character_Top_Panel::BindGameStateDelegate()
{
	if (AMobyGameState* InGameState = GetGameState())
	{
		InGameState->SimpleUpdateAttributesDelegate.AddUObject(CharacterInfo, &UUI_CharacterInfo::ResponseUpdateSlots);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			BindGameStateDelegate();
		});
	}
}
