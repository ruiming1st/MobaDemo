// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Inventory.h"
#include "Components/UniformGridSlot.h"
#include "UI_InventorySlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/DragDropOperation.h"
#include "MobyGame/MobyGamePlayerState.h"


UPanelWidget* UUI_Inventory::GetSlotArray()
{
	return SlotArrayInventory;
}

void UUI_Inventory::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_Inventory::LayoutSlot(const TArray<int32>& InKeys)
{
	//��ֹ��������UI
	if (!SlotArrayInventory->GetChildrenCount())
	{
		if (InventorySlotClass)
		{
			if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
			{
				int32 RowNumber = 2;
				int32 ColumnNumber = 3;
				for (int32 MyRow = 0; MyRow < RowNumber; MyRow++)
				{
					for (int32 MyColumn = 0; MyColumn < ColumnNumber; MyColumn++)
					{
						if (UUI_InventorySlot* SlotWidget = CreateWidget<UUI_InventorySlot>(GetWorld(), InventorySlotClass))
						{
							if (UUniformGridSlot* GridSlot = SlotArrayInventory->AddChildToUniformGrid(SlotWidget))
							{
								GridSlot->SetColumn(MyColumn);
								GridSlot->SetRow(MyRow);
								GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
								GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
							}
							//BuildSlot�����SlotID��ӵ���IDs�����е�InventoryID
							//�ݹ鴴��Slots�����ID��Ž������а���˳�����
							SlotWidget->SetSlotID(InKeys[MyRow * ColumnNumber + MyColumn]);
							//Update
							SlotWidget->UpdateSlot();
						}
					}
				}
			}

		}
	}
}

