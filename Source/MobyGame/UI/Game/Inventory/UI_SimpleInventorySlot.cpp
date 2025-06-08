// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_SimpleInventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUI_SimpleInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

}

void UUI_SimpleInventorySlot::UpdateSlot(const FSlotData* InSlotData)
{
	if (InSlotData)
	{	
		UpdateIcon(InSlotData->SlotICO);
		UpdateNumber(InSlotData->Number);

		ClientSlotData = *InSlotData;
	}	
}

UWidget* UUI_SimpleInventorySlot::GetTip()
{
	return VerifyTip(GetPlayerState(), Cast<UUI_Tip>(Super::GetTip()), GetSlotID(), &ClientSlotData);
}

void UUI_SimpleInventorySlot::ResetSlot()
{
	Super::ResetSlot();

	UpdateNumber(INDEX_NONE);

	SetSlotID(INDEX_NONE);
}

void UUI_SimpleInventorySlot::UpdateNumber(int32 InSlotNumber)
{
	DisplayNumber(SlotNumber, InSlotNumber);
}
