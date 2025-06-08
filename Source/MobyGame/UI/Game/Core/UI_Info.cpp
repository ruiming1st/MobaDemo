// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Info.h"

template<class T>
void UUI_Info::CallSlotBreak(TFunction<bool(T*)> InventorySlotLamada)
{
	//返回为假，找到了，break，否则继续
	UPanelWidget* SlotArray = GetSlotArray();
	if (SlotArray)
	{
		for (auto& TmpSlot : SlotArray->GetAllChildren())
		{
			if (T* InventorySlot = Cast<T>(TmpSlot))
			{
				if (!InventorySlotLamada(InventorySlot))
				{
					break;
				}
			}
		}
	}
}
