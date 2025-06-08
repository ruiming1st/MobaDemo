// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_InformationBar.h"
#include "Components/TextBlock.h"


void UUI_InformationBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_InformationBar::SetTitle(const FString& Msg)
{
	Title->SetText(FText::FromString(Msg));
}



void UUI_InformationBar::SetLv(const int32 &InLv)
{
	Lv->SetText(FText::Format(NSLOCTEXT("CharacterLV", "CharacterDataLv", "Lv. {0}"), InLv));
}

void UUI_InformationBar::SetMana(float InPercentage)
{
	Mana->SetPercent(InPercentage);
}