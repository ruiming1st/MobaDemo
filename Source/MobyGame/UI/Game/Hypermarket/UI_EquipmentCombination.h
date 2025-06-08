// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Inventory/Drop/UI_NativeOnDrop.h"
#include "../../../Table/SlotTable.h"
#include "UI_EquipmentCombination.generated.h"

class UCanvasPanel;
class UUI_ItemSlot;

typedef uint8 FSlotLayer;

USTRUCT(BlueprintType)
struct FDrawEquipLine
{
	GENERATED_USTRUCT_BODY()

	// | 
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UTexture2D* VerticalLine;

	//——
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UTexture2D* HorizontalLine;

	//_|_
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UTexture2D* TurnLeftAndRightVertically;

	//|-
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UTexture2D* TurnRight;

	//-|
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UTexture2D* TurnLeft;

	//-|-
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UTexture2D* TLine;
};
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_EquipmentCombination : public UUI_NativeOnDrop
{
	GENERATED_BODY()

	//装备合成路线的背景画布
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* EquipmentPanel;

	UPROPERTY(EditDefaultsOnly, Category = Param)
	FVector2D StartPosition;

	UPROPERTY(EditDefaultsOnly, Category = Param)
	FVector2D IconSize;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_ItemSlot> ItemSlotClass;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	FDrawEquipLine LineTexture;
public:
	UUI_EquipmentCombination();
	virtual void NativeConstruct() override;

	void UpdateSlot(const FSlotTable *InTable);

protected:
	int32 GetLayerDepth(const FSlotTable* InTable, int32 InDepth = 1);
	void RecursiveUpdateSlot(const FSlotTable* InTable, const FVector2D &InStartPostion, FSlotLayer InSlotLayer);
};
