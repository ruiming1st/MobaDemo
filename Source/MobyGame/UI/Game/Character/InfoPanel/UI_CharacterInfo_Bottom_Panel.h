// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Panel.h"
#include "UI_CharacterInfo_Bottom_Panel.generated.h"

class UUI_CharacterInfo;
class UTextBlock;
class UUI_Skill;
class UUI_Inventory;
class UProgressBar;
class UUI_Buff;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_CharacterInfo_Bottom_Panel : public UUI_Panel
{
	GENERATED_BODY()

	//包含各种属性的控件
	UPROPERTY(meta = (BindWidget))
	UUI_CharacterInfo* CharacterInfo;

	UPROPERTY(meta = (BindWidget))
	UUI_Skill* SkillInventory;

	UPROPERTY(meta = (BindWidget))
	UUI_Inventory* ItemInventory;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Lv;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ManaText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ExpBar;

	UPROPERTY(meta = (BindWidget))
	UUI_Buff* Buff;
public:
	virtual void NativeConstruct() override;

	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void ResponseUpdateSlots(int64 InPlayerID);

	UFUNCTION()
	void Die(float InTime);

	UFUNCTION()
	void Reborn();
protected:
	void ResponseUpdateSlot(int64 InPlayerID, const ECharacterAttributeType Type);

	void BindGameStateDelegate();
	void BindPlayerStateDelegate();
};
