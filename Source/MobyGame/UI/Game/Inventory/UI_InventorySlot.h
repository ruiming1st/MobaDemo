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

	//����
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotNumber;

	//��ק��ʾͼ����
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

	//�������
public:
	//�����������Ʒ��������
	void UpdateNumber();
	//���¶�Ӧ��Slot
	virtual void UpdateSlot();
	//��ʼ�������CD��״̬
	virtual void StartUpdateCD();
	//��������CD
	virtual void EndUpdateCD();
	//����Slot
	virtual void ResetSlot();
	//��ȡSlot����
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
