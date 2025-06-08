// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Slot.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MobyGame/MobyGamePlayerState.h"

void UUI_Slot::NativeConstruct()
{
	Super::NativeConstruct();

	ClickButton->OnClicked.AddDynamic(this, &UUI_Slot::OnClickedWidget);
}

void UUI_Slot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (AMobyGamePlayerState* InState = GetPlayerState())
	{
		if (FSlotData* InSlotData = InState->GetSlotData(GetSlotID()))
		{
			if (InSlotData->CD > 0.f)
			{
				InSlotData->CD -= InDeltaTime;
				DrawSlotCDMat(InSlotData->CD / BuildSlot.MaxCD);
				DrawSlotCDText(InSlotData->CD);
				if (InSlotData->CD <= 0.f)
				{
					DrawSlotCDMat(0.f);
					DrawSlotCDText(0.f);
				}
			}
		}
	}
}

void UUI_Slot::ResetSlot()
{
	Super::ResetSlot();

	SlotCDValue->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_Slot::UpdateSlot()
{
	if (AMobyGamePlayerState* InState = GetPlayerState())
	{
		if (FSlotData* InSlotData = InState->GetSlotData(GetSlotID()))
		{
			if (InSlotData->SlotID != INDEX_NONE)
			{
				UpdateIcon(InSlotData->SlotICO);
				DrawSlotCDMat(InSlotData->CD);
				DrawSlotCDText(InSlotData->CD);
			}
			else
			{
				ResetSlot();
			}
		}
		else
		{
			ResetSlot();
		}
	}
}

void UUI_Slot::StartUpdateCD()
{
	if (AMobyGamePlayerState* InState = GetPlayerState())
	{
		if (FSlotData* InSlotData = InState->GetSlotData(GetSlotID()))
		{
			BuildSlot.MaxCD = InSlotData->CD;
		}
	}
}

void UUI_Slot::EndUpdateCD()
{
	if (AMobyGamePlayerState* InState = GetPlayerState())
	{
		if (FSlotData* InSlotData = InState->GetSlotData(GetSlotID()))
		{
			UpdateIcon(InSlotData->SlotICO);
			DrawSlotCDMat(InSlotData->CD);
			DrawSlotCDText(InSlotData->CD);
		}
	}
}

void UUI_Slot::EnableClickButton(bool bEnable)
{
	ClickButton->SetIsEnabled(bEnable);
}

bool UUI_Slot::IsEnableClickButton()
{
	return ClickButton->bIsEnabled;
}

void UUI_Slot::SetKeyName(const FString& NewKeyName)
{
	SlotKey->SetText(FText::FromString(NewKeyName));
}

void UUI_Slot::DrawSlotCDText(float InSlotCD)
{
	DisplayNumber(SlotCDValue, InSlotCD);
}
