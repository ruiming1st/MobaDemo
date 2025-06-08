// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_PlayersInfo.h"
#include "../../Inventory/UI_InventorySlot.h"
#include "Components/HorizontalBox.h"
#include "../../Inventory/UI_SimpleInventorySlot.h"
#include "MobyGame/Data/MisData.h"

UUI_PlayersInfo::UUI_PlayersInfo()
{
	PlayerID = INDEX_NONE;
}

void UUI_PlayersInfo::NativeConstruct()
{
	Super::NativeConstruct();

}

void UUI_PlayersInfo::UpdateSlot(const FPlayerTeamNetPackage& TeamNetPackage)
{
	//玩家信息的解析
	PlayerID = TeamNetPackage.PlayerInfo.PlayerID;
	CharacterIcon->SetBrushFromTexture(TeamNetPackage.PlayerInfo.PlayerIcon);
	LevelText->SetText(FText::FromString(FString::Printf(TEXT("%2d"), TeamNetPackage.PlayerInfo.CharacterLevel)));

	//玩家击杀信息的解析
	KillInfo->UpdateSlot(TeamNetPackage.PlayerInfo.KillInfo);

	//背包信息解析
	int32 i = 0;
	CallSlotBreak<UUI_SimpleInventorySlot>([&](UUI_SimpleInventorySlot* InSlot)->bool
	{
		InSlot->SetSlotID(TeamNetPackage.SlotDataNetPackage.SlotIDs[i]);
		InSlot->UpdateSlot(&TeamNetPackage.SlotDataNetPackage.Slots[i]);
		i++;
		return true;
	});
}

void UUI_PlayersInfo::Die(float InDeathTime)
{
	CharacterIcon->Die(InDeathTime);
}

UPanelWidget* UUI_PlayersInfo::GetSlotArray()
{
	return SlotArrayInventory;
}
