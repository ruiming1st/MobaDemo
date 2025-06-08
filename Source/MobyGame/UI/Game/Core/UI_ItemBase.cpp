// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_ItemBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "MobyGame/MobyGamePlayerState.h"
#include "../Tip/UI_Tip.h"
#include "MobyGame/Common/MethodUnit.h"

void UUI_ItemBase::NativeConstruct()
{
	Super::NativeConstruct();

	ItemButton->OnClicked.AddDynamic(this, &UUI_ItemBase::OnClickedWidget);
}

void UUI_ItemBase::SetIcon(UTexture2D* InIcon2D)
{
	ItemIcon->SetBrushFromTexture(InIcon2D);
}

void UUI_ItemBase::OnClickedWidget()
{
	if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
	{
		InPlayerState->Buy(GetItemID());
	}
}

void UUI_ItemBase::SetItemID(int32 InID)
{
	ItemID = InID;
}

void UUI_ItemBase::UpdateSlot(const FSlotTable *InTable)
{
	if (InTable)
	{
		SetItemID(InTable->ID);
		SetIcon(InTable->SlotIcon);

		if (UUI_Tip* Tip = Cast<UUI_Tip>(GetTip()))
		{
			if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
			{
				if (const FSlotAttribute* InSlotAttribute = InPlayerState->GetSlotAttributeTemplate(InTable->ID))
				{
					//½éÉÜ
					Tip->SetDescribeText(InTable->SlotIntroduction);

					MethodUnit::TipRegistration(Tip, InSlotAttribute);
				}
				
			}

		
		}
	 
	}
}

