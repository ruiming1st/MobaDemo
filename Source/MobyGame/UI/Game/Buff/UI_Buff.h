// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Core/UI_Info.h"
#include "UI_Buff.generated.h"

class UWrapBox;
class UUI_BuffSlot;
/**
 * 
 */
//本地属性栏上方的buff内容显示条
UCLASS()
class MOBYGAME_API UUI_Buff : public UUI_Info
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UWrapBox* Buf;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_BuffSlot> BuffSlotClass;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	FVector2D BuffSlotIconSize;

public:
	UUI_Buff();

	virtual UPanelWidget* GetSlotArray();

	virtual void NativeConstruct() override;

	void UpdateCD(int64 InPlayerID, int32 InSlotID, float InMaxCD);

	void UpdateCD(int32 InSlotID, float InMaxCD);

	void RemoveCD(int64 InPlayerID, int32 InSlotID);

	void Clear();
};
