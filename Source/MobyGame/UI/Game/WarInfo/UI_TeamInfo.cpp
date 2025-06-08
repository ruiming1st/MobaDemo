// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_TeamInfo.h"
#include "Components/VerticalBox.h"
#include "Team/UI_PlayersInfo.h"
#include "Components/VerticalBoxSlot.h"
#include "MobyGame/Data/MisData.h"
#include "ThreadManage.h"

void UUI_TeamInfo::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction(TEXT("TeamInfo"), IE_Pressed, this, &UUI_TeamInfo::OnClickedWidget);
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction(TEXT("TeamInfo"), IE_Released, this, &UUI_TeamInfo::OnReleasedClickedWidget);

	SetVisibility(ESlateVisibility::Hidden);

	BindDelegate();	
}


void UUI_TeamInfo::OnClickedWidget()
{
	SetVisibility(ESlateVisibility::Visible);

	EnemyTeam->ClearChildren();
	FriendlyTeam->ClearChildren();

	//发送请求
	if (AMobyGamePlayerState* InGamePlayerState = GetPlayerState())
	{
		InGamePlayerState->RequestAllCharacterTeamsAttribute();
	}
}

void UUI_TeamInfo::OnReleasedClickedWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
}


void UUI_TeamInfo::BindDelegate()
{
	if (AMobyGamePlayerState *InGamePlayerState = GetPlayerState())
	{
		//InGamePlayerState->GetPlayerData()->Team;
		InGamePlayerState->SimplePlayerTeamDelegate.BindLambda(
			[&](const TArray<FPlayerTeamNetPackage> & InPlayerTeamNetPackage)
			{
				int32 i = 0;
				for (const auto& Tmp : InPlayerTeamNetPackage)
				{
					if (UUI_PlayersInfo* SlotWidget = CreateWidget<UUI_PlayersInfo>(GetWorld(), PlayersInfoClass))
					{
						UVerticalBoxSlot* GridSlot = NULL;
						if (i & 0x1)
						{
							GridSlot = EnemyTeam->AddChildToVerticalBox(SlotWidget);
						}
						else
						{
							GridSlot = FriendlyTeam->AddChildToVerticalBox(SlotWidget);
						}
						GridSlot->SetPadding(1.0f);
						GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
						GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

						SlotWidget->UpdateSlot(Tmp);

						//开始倒计时记录
						if (Tmp.PlayerInfo.DeathTime != 0.f)
						{
							SlotWidget->Die(Tmp.PlayerInfo.DeathTime);
						}

						i++;
					}
				}
			});
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			BindDelegate();
		});
	}
}
