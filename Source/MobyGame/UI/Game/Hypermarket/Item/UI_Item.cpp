// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_Item.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MobyGame/Table/SlotTable.h"

UUI_Item::UUI_Item(const FObjectInitializer& ObjectInitializer)
{
	Table = NULL;
}

void UUI_Item::NativeConstruct()
{
	Super::NativeConstruct();

}

void UUI_Item::SetItemIntroduction(const FText& InContent)
{
	ItemIntroduction->SetText(InContent);
}

void UUI_Item::SetItemName(const FString& InName)
{
	ItemName->SetText(FText::FromString(InName));
}

void UUI_Item::SetItemGold(const int32 InGold)
{
	Gold->SetText(FText::Format(NSLOCTEXT("UUI_Item","SetItemGold","{0}"), InGold));
}

void UUI_Item::UpdateSlot(const FSlotTable *InTable)
{
	Super::UpdateSlot(InTable);

	if (InTable)
	{
		SetItemGold(InTable->SlotGold);
		SetItemIntroduction(InTable->SlotIntroduction);

		Table = InTable;
	}
}

//¹ºÂò
void UUI_Item::OnClickedWidget()
{
	Super::OnClickedWidget();
}

FReply UUI_Item::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton || InMouseEvent.IsTouchEvent())
	{
		CallEquipmentSynthesisDelegate.ExecuteIfBound(GetItemID());
	}


	return FReply::Handled();
}

int32 UUI_Item::GetItemCostGold()
{
	if (Gold)
	{
		//FString GoldString = Gold->GetText().ToString();

		////È¥µô¶ººÅ
		//if (GoldString.Contains(TEXT(",")))
		//{
		//	GoldString.ReplaceInline(TEXT(","),TEXT(""));
		//}

		//return FCString::Atoi(*GoldString);

		if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
		{
			int32 NewGold = InPlayerState->GetItemAndCheckGold(Table);
			SetItemGold(NewGold);

			return NewGold;
		}
	}

	return 0;
}
