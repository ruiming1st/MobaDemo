// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_SlotElement.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MobyGame/MobyGamePlayerState.h"

void UUI_SlotElement::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_SlotElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

UUI_SlotElement::UUI_SlotElement(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TipPtr = NULL;
}

void UUI_SlotElement::ResetSlot()
{
	SlotIcon->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_SlotElement::SetIconSize(const FVector2D& InSize)
{
	if (SlotIcon)
	{
		SlotIcon->SetBrushSize(InSize);
	}
}

void UUI_SlotElement::UpdateIcon(UTexture2D* InIcon)
{
	if (SlotIcon)
	{
		if (InIcon)
		{
			SlotIcon->SetBrushFromTexture(InIcon);
			SlotIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			SlotIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UUI_SlotElement::DisplayNumber(UTextBlock* TextNumberBlock, float TextNumber)
{
	if (TextNumber <= 0.f)
	{
		TextNumberBlock->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		if (TextNumber > 0.f && TextNumber <= 1.0f)
		{
			TextNumberBlock->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), TextNumber)));
		}
		else
		{
			TextNumberBlock->SetText(FText::FromString(FString::Printf(TEXT("%02d"), (int32)TextNumber)));
		}
		
		TextNumberBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UUI_SlotElement::SetSlotID(int32 InID)
{
	BuildSlot.SlotID = InID;
}

void UUI_SlotElement::DisplayNumber(UTextBlock* TextNumberBlock, int32 TextNumber)
{
	if (TextNumber < 1)
	{
		TextNumberBlock->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		TextNumberBlock->SetText(FText::FromString(FString::Printf(TEXT("%02d"), TextNumber)));

		TextNumberBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}