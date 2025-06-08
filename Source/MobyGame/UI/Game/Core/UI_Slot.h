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

	//��λ��ʾ
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
	//����Slot
	virtual void ResetSlot();
	//���¶�Ӧ��Slot
	virtual void UpdateSlot();
	//��ʼ�������CD��״̬
	virtual void StartUpdateCD();
	//��������CD
	virtual void EndUpdateCD();

public:
	void EnableClickButton(bool bEnable);
	bool IsEnableClickButton();
public:
	//��λ����
	void SetKeyName(const FString& NewKeyName);

	//��������CD
	void DrawSlotCDText(float InSlotCD);
public:

	UFUNCTION(BlueprintCallable)
	virtual void OnClickedWidget() {}
	UFUNCTION(BlueprintCallable)
	virtual void OnReleasedClickedWidget() {}
};
