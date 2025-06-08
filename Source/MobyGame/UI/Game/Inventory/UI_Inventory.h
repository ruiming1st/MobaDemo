// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_Bar.h"
#include "UI_Inventory.generated.h"



class UUniformGridPanel;
class UUI_InventorySlot;

/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Inventory : public UUI_Bar
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel *SlotArrayInventory;

	UPROPERTY(EditDefaultsOnly,Category = UI)
	TSubclassOf<UUI_InventorySlot> InventorySlotClass;

protected:
	virtual UPanelWidget* GetSlotArray();
	virtual void NativeConstruct() override;
	virtual void LayoutSlot(const TArray<int32>& InKeys);
};
