// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_SlotElement.h"
#include "UI_CD.generated.h"

class UImage;
class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_CD : public UUI_SlotElement
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage *SlotCD;

	//���ַ���CDѰ��
	UPROPERTY(EditDefaultsOnly, Category = UI)
	FName SlotMatCDName;

	//���ַ���CDѰ��
	UPROPERTY(EditDefaultsOnly, Category = UI)
	FName SlotClearValueName;

	//CD��̬����
	UPROPERTY()
	class UMaterialInstanceDynamic* CDMaterialDynamic;

public:
	virtual void SetIconSize(const FVector2D& InSize);

protected:
	virtual void NativeConstruct() override;
	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//����Slot
	virtual void ResetSlot();
public:
	//���Ʋ���CD
	void DrawSlotCDMat(float InSlotCD);
};
