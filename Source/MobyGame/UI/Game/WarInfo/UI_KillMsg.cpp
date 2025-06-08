// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_KillMsg.h"
#include "MobyGame/Data/MisData.h"
#include "MobyGame/MobyGamePlayerState.h"
#include "ThreadManage.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ThreadManage.h"

#define LOCTEXT_NAMESPACE "KillMsg"
void UUI_KillMsg::NativeConstruct()
{
	Super::NativeConstruct();

	BindDelegate();
}

void UUI_KillMsg::UpdateSlot()
{
	if (UWidgetAnimation* InWidgetAnim = GetNameWidgetAnimation(TEXT("KillTargetAnim")))
	{
		PlayAnimation(InWidgetAnim);
	}
}

void UUI_KillMsg::Kill(const FKillNetPackage& NetPackage, const FText& killName)
{
	Show();
	KilledTypeText->SetText(FText::Format(LOCTEXT("KilledType", "{0}"), killName));
	KillerNameText->SetText(FText::FromString(NetPackage.KillerName));
	KilledNameText->SetText(FText::FromString(NetPackage.KilledName));
	KillerIcon->SetBrushFromTexture(NetPackage.KillerIcon);
	KilledIcon->SetBrushFromTexture(NetPackage.KilledIcon);
}

void UUI_KillMsg::SerialKill(const FKillNetPackage& NetPackage, const FText& killName)
{
	Show();
	KilledTypeText->SetText(FText::Format(LOCTEXT("KilledType", "{0}"), killName));
	KillerIcon->SetBrushFromTexture(NetPackage.KillerIcon);
	KilledIcon->SetBrushFromTexture(NetPackage.KilledIcon);

	KilledIcon->SetVisibility(ESlateVisibility::Hidden);
	KillerNameText->SetVisibility(ESlateVisibility::Collapsed);
	KilledNameText->SetVisibility(ESlateVisibility::Collapsed);
}

void UUI_KillMsg::AchievementKill(const FKillNetPackage& NetPackage, const FText& killName)
{
	Show();
	KilledTypeText->SetText(FText::Format(LOCTEXT("KilledType", "{0}"), killName));
	KillerIcon->SetBrushFromTexture(NetPackage.KillerIcon);

	KilledIcon->SetVisibility(ESlateVisibility::Hidden);
	KillerNameText->SetVisibility(ESlateVisibility::Collapsed);
	KilledNameText->SetVisibility(ESlateVisibility::Collapsed);
}

void UUI_KillMsg::Show()
{
	KillerNameText->SetVisibility(ESlateVisibility::HitTestInvisible);
	KilledNameText->SetVisibility(ESlateVisibility::HitTestInvisible);
	KillerIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	KilledIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	KilledTypeText->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UUI_KillMsg::BindDelegate()
{
	if (AMobyGamePlayerState * InPlayerState = GetPlayerState())
	{
		InPlayerState->KillMsgDelegate.BindLambda(
			[&](const FKillNetPackage& NetPackage)
		{
			switch (NetPackage.KillType)
			{
			case EKillType::KILL:
				Kill(NetPackage, LOCTEXT("KILL", "KILL"));
				break;
			case EKillType::DOUBLEKILL:
				SerialKill(NetPackage, LOCTEXT("DOUBLEKILL", "DOUBLEKILL"));
				break;
			case EKillType::TRIPLEKILL:
				SerialKill(NetPackage, LOCTEXT("TRIPLEKILL", "TRIPLEKILL"));
				break;
			case EKillType::QUADRAKILL:
				SerialKill(NetPackage, LOCTEXT("QUADRAKILL", "QUADRAKILL"));
				break;
			case EKillType::PENTAKILL:
				SerialKill(NetPackage, LOCTEXT("PENTAKILL", "PENTAKILL"));
				break;
			case EKillType::BLOOD:
				AchievementKill(NetPackage, LOCTEXT("BLOOD", "BLOOD"));
				break;
			case EKillType::INVINCIBLE:
				AchievementKill(NetPackage, LOCTEXT("INVINCIBLE", "INVINCIBLE"));
				break;
			case EKillType::LEGENDARY:
				AchievementKill(NetPackage, LOCTEXT("LEGENDARY", "LEGENDARY"));
				break;
			case EKillType::ACE:
				AchievementKill(NetPackage, LOCTEXT("ACE", "ACE"));
				break;
			}

			//²¥·Å¶¯»­
			UpdateSlot();
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

#undef LOCTEXT_NAMESPACE