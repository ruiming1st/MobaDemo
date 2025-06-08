// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Bar.h"
#include "UI_PlayersInfo.generated.h"

class UTextBlock;
class UImage;
class UUI_KillInfo;
class UHorizontalBox;
class UPanelWidget;
struct FPlayerTeamNetPackage;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_PlayersInfo :public UUI_Bar
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidget))
	UUI_CharacterIcon* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* SlotArrayInventory;

	UPROPERTY(meta = (BindWidget))
	UUI_KillInfo* KillInfo;
	
public:
	UUI_PlayersInfo();

	virtual void NativeConstruct() override;

	void UpdateSlot(const FPlayerTeamNetPackage &TeamNetPackage);

	void Die(float InDeathTime);
protected:
	virtual UPanelWidget* GetSlotArray();

private:
	int64 PlayerID;
};
