// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_Info.generated.h"

class UPanelWidget;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Info : public UUI_Base
{
	GENERATED_BODY()

public:
	virtual void LayoutSlot(const TArray<int32>& InKeys) {}

	virtual UPanelWidget* GetSlotArray() { return NULL; }
protected:
	template<class T>
	void CallSlotBreak(TFunction<bool(T*)> InventorySlotLamada);
};

