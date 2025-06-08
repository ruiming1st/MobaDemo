// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Inventory/Drop/UI_NativeOnDrop.h"
#include "UI_ItemBase.generated.h"

class UImage;
class UButton;
struct FSlotTable;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_ItemBase : public UUI_NativeOnDrop
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemButton;

protected:
	virtual void NativeConstruct() override;

public:
	void SetIcon(UTexture2D* InIcon2D);

	UFUNCTION(BlueprintCallable)
	virtual void OnClickedWidget();

	void SetItemID(int32 InID);

public:
	FORCEINLINE int32 GetItemID() const { return ItemID; }
	virtual void UpdateSlot(const FSlotTable *InTable);
private:
	int32 ItemID;
};
