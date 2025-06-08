// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../Core/UI_Base.h"
#include "UI_NativeOnDrop.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_NativeOnDrop : public UUI_Base
{
	GENERATED_BODY()

protected:
	//ÍÏ×§ÏìÓ¦
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
};
