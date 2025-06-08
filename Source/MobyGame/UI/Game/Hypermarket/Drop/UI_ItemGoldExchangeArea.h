// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../Core/UI_Base.h"
#include "UI_ItemGoldExchangeArea.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_ItemGoldExchangeArea : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SellText;

public:
	virtual void NativeConstruct() override;

	void SetSellText(const FText& InContent);
protected:
	//ÍÏ×§ÏìÓ¦
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
