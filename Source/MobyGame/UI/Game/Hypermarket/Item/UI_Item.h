// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_ItemBase.h"
#include "UI_Item.generated.h"

class UImage;
class UTextBlock;
struct FSlotTable;

DECLARE_DELEGATE_OneParam(FCallEquipmentSynthesisDelegate, int32)
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Item : public UUI_ItemBase
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Gold;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemIntroduction;

public:
	UUI_Item(const FObjectInitializer& ObjectInitializer);
	FCallEquipmentSynthesisDelegate CallEquipmentSynthesisDelegate;

	int32 GetItemCostGold();
protected:
	virtual void NativeConstruct() override;

	virtual void OnClickedWidget();

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
public:
	void SetItemIntroduction(const FText&InContent);
	void SetItemName(const FString& InName);
	void SetItemGold(const int32 InGold);

	virtual void UpdateSlot(const FSlotTable *InTable);

private:
	const FSlotTable* Table;
};
