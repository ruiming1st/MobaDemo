// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Panel.h"
#include "../../Core/UI_Info.h"
#include "UI_Character_Top_Panel.generated.h"

class UUI_CharacterInfo;
class UUniformGridPanel;
class UUI_Buff;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Character_Top_Panel : public UUI_Info
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UUI_CharacterInfo* CharacterInfo;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* SlotArrayInventory;

	UPROPERTY(meta = (BindWidget))
	UUI_Buff* Buff;

public:
	virtual void NativeConstruct() override;

	virtual UPanelWidget* GetSlotArray();
protected:
	void BindPlayerStateDelegate();
	void BindGameStateDelegate();
};
