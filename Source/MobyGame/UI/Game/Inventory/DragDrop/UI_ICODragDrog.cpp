// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_ICODragDrog.h"
#include "Components/Image.h"

void UUI_ICODragDrog::DrawICON(UTexture2D *InICON)
{
	ICON->SetBrushFromTexture(InICON);
}
