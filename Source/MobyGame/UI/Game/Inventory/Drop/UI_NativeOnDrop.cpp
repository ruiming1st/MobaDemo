// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_NativeOnDrop.h"
#include "../UI_InventorySlot.h"
#include "Blueprint/DragDropOperation.h"

bool UUI_NativeOnDrop::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	bool bDrop = false;

	if (UDragDropOperation* InDragDropOperation = Cast<UDragDropOperation>(InOperation))
	{
		if (UUI_InventorySlot* MyInventorySlot = Cast<UUI_InventorySlot>(InDragDropOperation->Payload))
		{
			MyInventorySlot->UpdateSlot();

			bDrop = true;
		}
	}

	return bDrop;
}
