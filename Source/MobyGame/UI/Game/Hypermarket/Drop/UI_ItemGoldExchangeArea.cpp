// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_ItemGoldExchangeArea.h"
#include "../../Inventory/UI_InventorySlot.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/TextBlock.h"
#include "MobyGame/Data/MisData.h"

void UUI_ItemGoldExchangeArea::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_ItemGoldExchangeArea::SetSellText(const FText& InContent)
{
	SellText->SetText(InContent);
}

bool UUI_ItemGoldExchangeArea::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	bool bDrop = false;

	if (UDragDropOperation* InDragDropOperation = Cast<UDragDropOperation>(InOperation))
	{
		if (UUI_InventorySlot* MyInventorySlot = Cast<UUI_InventorySlot>(InDragDropOperation->Payload))
		{
			if (const FSlotData* InSlotData = GetPlayerState()->GetInventorySlotData(MyInventorySlot->GetSlotID()))
			{
				if (InSlotData->bCancelBuy)
				{
					//订单退货
					GetPlayerState()->CancellationOfOrder(MyInventorySlot->GetSlotID(),InSlotData->SlotID); 
				}
				else
				{
					GetPlayerState()->Sell(MyInventorySlot->GetSlotID(),InSlotData->SlotID);//二手卖出
				}			
			}
			else
			{
				MyInventorySlot->UpdateSlot();
			}

			bDrop = true;
		}
	}

	return bDrop;
}
