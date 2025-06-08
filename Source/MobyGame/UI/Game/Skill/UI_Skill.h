// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_Bar.h"
#include "UI_Skill.generated.h"

class UHorizontalBox;
class UUI_SkillSlot;
class UPanelWidget;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Skill : public UUI_Bar
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* SlotArraySkill;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_SkillSlot> SkillSlotClass;

protected:
	virtual UPanelWidget* GetSlotArray();
	virtual void NativeConstruct() override;
	virtual void LayoutSlot(const TArray<int32>& InKeys);

	void BindPlayerStateDelegate();
};
