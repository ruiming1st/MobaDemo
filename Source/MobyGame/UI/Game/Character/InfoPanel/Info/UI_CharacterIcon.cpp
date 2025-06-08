// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_CharacterIcon.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

UUI_CharacterIcon::UUI_CharacterIcon(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	DeathTime = 0.f;
}

void UUI_CharacterIcon::NativeConstruct()
{
	Super::NativeConstruct();

	DeathTimeBlock->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_CharacterIcon::SetDisplayNumber(int32 InNumebr)
{
	DeathTimeBlock->SetText(FText::FromString(FString::Printf(TEXT("%2d"), InNumebr)));
}

void UUI_CharacterIcon::Die(float InDeathTime)
{
	DeathTime = InDeathTime;
	CharacterIcon->SetIsEnabled(false);
	DeathTimeBlock->SetVisibility(ESlateVisibility::Visible);
}

void UUI_CharacterIcon::Reborn()
{
	CharacterIcon->SetIsEnabled(true);
	DeathTimeBlock->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_CharacterIcon::SetBrushFromTexture(UTexture2D* NewTexture)
{
	CharacterIcon->SetBrushFromTexture(NewTexture);
}

void UUI_CharacterIcon::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (DeathTime > 0.f)
	{
		DeathTime -= InDeltaTime;
		SetDisplayNumber((int32)DeathTime);
	}
	else
	{
		Reborn();
	}
}