// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_CharacterInfo.h"
#include "MobyGame/Data/MisData.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ThreadManage.h"
#include "MobyGame/MobyGameState.h"
#include "UI_CharacterIcon.h"

UUI_CharacterInfo::UUI_CharacterInfo(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UUI_CharacterInfo::NativeConstruct()
{
	//在此处绑定了代理ResponseUpdateSlot
	Super::NativeConstruct();
}

void UUI_CharacterInfo::SetDisplayNumber(int32 Number)
{
	if (CharacterIcon)
	{
		CharacterIcon->SetDisplayNumber(Number);
	}
}

void UUI_CharacterInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUI_CharacterInfo::ResponseUpdateSlots(int64 InPlayerID)
{
	if (InPlayerID == GetPlayerID())
	{
		if (AMobyGameState* InGameState = GetGameState())
		{
			//更新属性
			if (const FCharacterAttribute* InAttribute = GetGameState()->GetCharacterAttribute(InPlayerID))
			{
				PhysicalAttack->SetText(FText::FromString(FString::FromInt((int32)InAttribute->PhysicalAttack)));
				Armor->SetText(FText::FromString(FString::FromInt((int32)InAttribute->Armor)));
				MagicAttack->SetText(FText::FromString(FString::FromInt((int32)InAttribute->MagicAttack)));
				MagicDefense->SetText(FText::FromString(FString::FromInt((int32)InAttribute->MagicDefense)));
				PhysicalPenetration->SetText(FText::FromString(FString::FromInt((int32)InAttribute->PhysicalPenetration)));
				MagicPenetration->SetText(FText::FromString(FString::FromInt((int32)InAttribute->MagicPenetration)));
				WalkSpeed->SetText(FText::FromString(FString::FromInt((int32)InAttribute->WalkSpeed)));
				AttackSpeed->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), InAttribute->AttackSpeed)));

				//更新其他属性
				if (const FCharacterTable* InTable = GetGameState()->GetCharacterTableTemplate(InAttribute->ID))
				{
					CharacterIcon->SetBrushFromTexture(InTable->CharacterIcon);
				}
			}
		}
	}
}

void UUI_CharacterInfo::Die(float InDeathTime)
{
	if (CharacterIcon)
	{
		CharacterIcon->Die(InDeathTime);
	}
}

void UUI_CharacterInfo::Reborn()
{
	if (CharacterIcon)
	{
		CharacterIcon->Reborn();
	}
}

void UUI_CharacterInfo::ResponseUpdateSlot(int64 InPlayerID, const ECharacterAttributeType Type)
{
	if (InPlayerID == GetPlayerID())
	{
		if (AMobyGameState* InGameState = GetGameState())
		{
			if (const FCharacterAttribute* InAttribute = GetGameState()->GetCharacterAttribute(InPlayerID))
			{
				switch (Type)
				{
				case ECharacterAttributeType::ATTR_PHYSICAL_ATTACK:
					PhysicalAttack->SetText(FText::FromString(FString::FromInt((int32)InAttribute->PhysicalAttack)));
					break;
				case ECharacterAttributeType::ATTR_ARMOR:
					Armor->SetText(FText::FromString(FString::FromInt((int32)InAttribute->Armor)));
					break;
				case ECharacterAttributeType::ATTR_MAGIC_ATTACK:
					MagicAttack->SetText(FText::FromString(FString::FromInt((int32)InAttribute->MagicAttack)));
					break;
				case ECharacterAttributeType::ATTR_MAGIC_DEFENSE:
					MagicDefense->SetText(FText::FromString(FString::FromInt((int32)InAttribute->MagicDefense)));
					break;
				case ECharacterAttributeType::ATTR_PHYSICAL_PENETRATION:
					PhysicalPenetration->SetText(FText::FromString(FString::FromInt((int32)InAttribute->PhysicalPenetration)));
					break;
				case ECharacterAttributeType::ATTR_MAGIC_PENETRATION:
					MagicPenetration->SetText(FText::FromString(FString::FromInt((int32)InAttribute->MagicPenetration)));
					break;
				case ECharacterAttributeType::ATTR_WALK_SPEED:
					WalkSpeed->SetText(FText::FromString(FString::FromInt((int32)InAttribute->WalkSpeed)));
					break;
				case ECharacterAttributeType::ATTR_ATTACK_SPEED:
					AttackSpeed->SetText(FText::FromString(FString::FromInt((int32)InAttribute->AttackSpeed)));
					break;
				}
			}
		}
	}
}
