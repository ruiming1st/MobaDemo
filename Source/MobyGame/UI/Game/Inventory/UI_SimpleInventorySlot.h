// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_SlotElement.h"
#include "../Core/Interface/UI_TipInterface.h"
#include "MobyGame/Data/MisData.h"
#include "UI_SimpleInventorySlot.generated.h"

class UImage;
class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_SimpleInventorySlot : public UUI_SlotElement, public IUI_TipInterface
{
	GENERATED_BODY()
	
	//数量
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotNumber;

public:
	virtual void NativeConstruct() override;

	void UpdateSlot(const FSlotData* InSlotData);

	virtual UWidget* GetTip();

	//重置Slot
	virtual void ResetSlot()override;

	//如果是消费物品更新数量
	void UpdateNumber(int32 InSlotNumber);
private:
	FSlotData ClientSlotData;
};
