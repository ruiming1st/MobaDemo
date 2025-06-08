// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_Slot.h"
#include "../Core/Interface/UI_TipInterface.h"
#include "UI_InventorySlot.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UDragDropOperation;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_InventorySlot : public UUI_Slot, public IUI_TipInterface
{
	GENERATED_BODY()

	friend class UUI_Inventory;

	//数量
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotNumber;

	//拖拽显示图标类
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUI_ICODragDrog> ICODragDrogClass;

public:
	UUI_InventorySlot();

	virtual UWidget* GetTip();

	virtual void NativeConstruct() override;
	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void OnClickedWidget();
	virtual void OnReleasedClickedWidget();

	//数据相关
public:
	//如果是消费物品更新数量
	void UpdateNumber();
	//更新对应的Slot
	virtual void UpdateSlot();
	//开始进入更新CD的状态
	virtual void StartUpdateCD();
	//结束更新CD
	virtual void EndUpdateCD();
	//重置Slot
	virtual void ResetSlot();
	//获取Slot数量
	int32 GetSlotNumber();

	bool IsDie();

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);

private:
	bool bDrag;

};
