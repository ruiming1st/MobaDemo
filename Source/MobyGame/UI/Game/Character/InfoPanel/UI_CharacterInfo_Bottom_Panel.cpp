// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_CharacterInfo_Bottom_Panel.h"
#include "MobyGame/MobyGameState.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "ThreadManage.h"
#include "Info/UI_CharacterInfo.h"
#include "MobyGame/Common/RenderingUtils.h"
#include "../../Skill/UI_Skill.h"

void UUI_CharacterInfo_Bottom_Panel::NativeConstruct()
{
	Super::NativeConstruct();

	BindGameStateDelegate();
	BindPlayerStateDelegate();
}

void UUI_CharacterInfo_Bottom_Panel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
	{
		if (InPlayerState->GetPlayerData())
		{
			GoldText->SetText(FText::FromString(FString::FromInt(InPlayerState->GetPlayerData()->Gold)));
		}
	}
}

void UUI_CharacterInfo_Bottom_Panel::ResponseUpdateSlots(int64 InPlayerID)
{
	if (GetPlayerID() == INDEX_NONE)
	{
		SetPlayerID(InPlayerID);
		CharacterInfo->SetPlayerID(InPlayerID);
	}

	if (InPlayerID == GetPlayerID())
	{
		if (AMobyGameState* InGameState = GetGameState())
		{
			//更新属性
			if (const FCharacterAttribute* InAttribute = GetGameState()->GetCharacterAttribute(InPlayerID))
			{
				Lv->SetText(FText::FromString(FString::FromInt(InAttribute->Lv)));

				ManaText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), (int32)InAttribute->Mana, (int32)InAttribute->MaxMana)));
				ManaBar->SetPercent(InAttribute->GetManaPercentage());

				HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), (int32)InAttribute->Health, (int32)InAttribute->MaxHealth)));
				HealthBar->SetPercent(InAttribute->GetHealthPercentage());

				ExpBar->SetPercent(InAttribute->GetEmpircalPercentage());

				CharacterInfo->ResponseUpdateSlots(InPlayerID);
			}
		}
	}
}

void UUI_CharacterInfo_Bottom_Panel::Die(float InTime)
{
	CharacterInfo->Die(InTime);
	SkillInventory->SetIsEnabled(false);

	//死亡 屏幕变灰
	RenderingUtils::ClientDeath(GetWorld(), true);
}

void UUI_CharacterInfo_Bottom_Panel::Reborn()
{
	CharacterInfo->Reborn();
	SkillInventory->SetIsEnabled(true);

	RenderingUtils::ClientDeath(GetWorld(), false);
}

void UUI_CharacterInfo_Bottom_Panel::ResponseUpdateSlot(int64 InPlayerID, const ECharacterAttributeType Type)
{
	if (InPlayerID == GetPlayerID())
	{
		if (AMobyGameState* InGameState = GetGameState())
		{
			if (const FCharacterAttribute* InAttribute = GetGameState()->GetCharacterAttribute(InPlayerID))
			{
				switch (Type)
				{
				case ECharacterAttributeType::ATTR_LV:
					Lv->SetText(FText::FromString(FString::FromInt(InAttribute->Lv)));
					break;
				case ECharacterAttributeType::ATTR_MANA:
				case ECharacterAttributeType::ATTR_MAX_MANA:
					ManaText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), (int32)InAttribute->Mana, (int32)InAttribute->MaxMana)));
					ManaBar->SetPercent(InAttribute->GetManaPercentage());
					break;
				case ECharacterAttributeType::ATTR_MAX_HEALTH:
				case ECharacterAttributeType::ATTR_HEALTH:
					HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), (int32)InAttribute->Health, (int32)InAttribute->MaxHealth)));
					HealthBar->SetPercent(InAttribute->GetHealthPercentage());
					break;
				case ECharacterAttributeType::ATTR_MAX_EXP:
				case ECharacterAttributeType::ATTR_EXP:
					ExpBar->SetPercent(InAttribute->GetEmpircalPercentage());
					break;
				}
			}
		}
	}
}


void UUI_CharacterInfo_Bottom_Panel::BindGameStateDelegate()
{
	if (AMobyGameState* InGameState = GetGameState())
	{
		InGameState->SimpleUpdateAttributesDelegate.AddUObject(this, &UUI_Panel::ResponseUpdateSlots);
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			BindGameStateDelegate();
		});
	}
}

void UUI_CharacterInfo_Bottom_Panel::BindPlayerStateDelegate()
{
	//初始化我们的 PlayerID
	if (AMobyGamePlayerState* InGamePlayerState = GetPlayerState())
	{
		Buff->SetPlayerID(InGamePlayerState->GetPlayerData()->PlayerID);

		InGamePlayerState->SimpleDieDelegate.BindUObject(this, &UUI_CharacterInfo_Bottom_Panel::Die);
		InGamePlayerState->SimpleBornDelegate.BindUObject(this, &UUI_CharacterInfo_Bottom_Panel::Reborn);
	}

	if (AMobyGameState* InState = GetGameState())
	{
		InState->UpdateBuffInfoDelegate.AddLambda([&](int64 InPlayerID, int32 InSlotID, float MaxCD)
		{
			Buff->UpdateCD(InPlayerID, InSlotID, MaxCD);
		});
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			BindPlayerStateDelegate();
		});
	}
}
