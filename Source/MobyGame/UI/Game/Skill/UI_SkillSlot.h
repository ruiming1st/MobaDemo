// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_Slot.h"
#include "MobyGame/Table/SlotAttribute.h"
#include "UI_SkillSlot.generated.h"

class UProgressBar;
class UButton;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_SkillSlot : public UUI_Slot
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UButton* UpdateLevelButton;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SkillLv;
public:
	virtual void NativeConstruct() override;

	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void OnClickedWidget();
	virtual void OnReleasedClickedWidget();
public:
	void OnResponseUpdateLevel();

	virtual void UpdateSlot();

	virtual UWidget* GetTip();
protected:
	UFUNCTION()
	void OnClickedUpdateLevel();

	void InitPlayerSkillCache();
protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
public:
	FORCEINLINE UProgressBar* GetSkillLv() { return SkillLv; }
	FORCEINLINE UButton* GetUpdateLevelButton() { return UpdateLevelButton; }

private:
	UPROPERTY()
	FSlotAttribute BaseCache;
};
