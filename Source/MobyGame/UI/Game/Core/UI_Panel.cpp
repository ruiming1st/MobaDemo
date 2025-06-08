// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Panel.h"
#include "MobyGame/MobyGamePlayerState.h"
#include "UI_Slot.h"
#include "ThreadManage.h"
#include "MobyGame/MobyGameState.h"
#include "../Character/InfoPanel/Info/UI_CharacterInfo.h"

UUI_Panel::UUI_Panel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerID = INDEX_NONE;
}

void UUI_Panel::NativeConstruct()
{
	Super::NativeConstruct();

	BindDelegate();
}

void UUI_Panel::RequestUpdateSlot(const ECharacterAttributeType Type)
{
	if (PlayerID != INDEX_NONE)
	{
		if (AMobyGameState* InGameState = GetGameState())
		{
			InGameState->RequestCharacterAttribute(PlayerID, PlayerID, Type);
		}
	}
}

void UUI_Panel::SetPlayerID(const int64& InPlayerID)
{
	PlayerID = InPlayerID;
}


void UUI_Panel::BindDelegate()
{
	if (AMobyGameState* InGameState = GetGameState())
	{
		InGameState->SimpleUpdateAttributeDelegate.AddUObject(this, &UUI_Panel::ResponseUpdateSlot);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			BindDelegate();
		});
	}
}
