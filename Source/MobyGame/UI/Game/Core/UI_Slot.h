// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_CD.h"
#include "UI_Slot.generated.h"

class UImage;
class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Slot : public UUI_CD
{
	GENERATED_BODY()

	//键位显示
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotKey;

	UPROPERTY(meta = (BindWidget))
	UTextBlock *SlotCDValue;

	UPROPERTY(meta = (BindWidget))
	UButton *ClickButton;

protected:
	virtual void NativeConstruct() override;
	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	//重置Slot
	virtual void ResetSlot();
	//更新对应的Slot
	virtual void UpdateSlot();
	//开始进入更新CD的状态
	virtual void StartUpdateCD();
	//结束更新CD
	virtual void EndUpdateCD();

public:
	void EnableClickButton(bool bEnable);
	bool IsEnableClickButton();
public:
	//键位设置
	void SetKeyName(const FString& NewKeyName);

	//绘制字体CD
	void DrawSlotCDText(float InSlotCD);
public:

	UFUNCTION(BlueprintCallable)
	virtual void OnClickedWidget() {}
	UFUNCTION(BlueprintCallable)
	virtual void OnReleasedClickedWidget() {}
};
