// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_Info.h"
#include "UI_Bar.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Bar : public UUI_Info
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual UPanelWidget* GetSlotArray() { return NULL; }
};
