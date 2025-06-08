// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_EndTheGame.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UUI_EndTheGame::NativeConstruct()
{
	Super::NativeConstruct();

	if (AMobyGamePlayerState *InState = GetPlayerState())
	{
		InState->GameOverDelegate.BindLambda([&](bool bVictory)
		{
			SetVisibility(ESlateVisibility::Visible);

			if (bVictory)
			{
				Victory();
			}
			else
			{
				Fail();
			}
		});
	}

	ReturnGameButton->OnClicked.AddDynamic(this, &UUI_EndTheGame::ReturnGame);
}

void UUI_EndTheGame::ReturnGame()
{

}

void UUI_EndTheGame::Victory()
{
	Background->SetBrushFromTexture(VictoryTexture);
}

void UUI_EndTheGame::Fail()
{
	Background->SetBrushFromTexture(FailTexture);
}