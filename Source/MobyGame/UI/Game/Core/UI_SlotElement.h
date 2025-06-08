// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_SlotElement.generated.h"

class UImage;
class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_SlotElement : public UUI_Base
{
	GENERATED_BODY()
		
	UPROPERTY(meta = (BindWidget))
		UImage *SlotIcon;

protected:
	virtual void NativeConstruct() override;
	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UUI_SlotElement(const FObjectInitializer& ObjectInitializer);

	//重置Slot
	virtual void ResetSlot();

public:
	//设置唯一ID
	void SetSlotID(int32 InID);
	
	//绘制图片
	void UpdateIcon(UTexture2D* InIcon);
	
	virtual void SetIconSize(const FVector2D &InSize);
protected:
	//显示 float字体
	void DisplayNumber(UTextBlock* TextNumberBlock, float TextNumber);
	//显示 int字体
	void DisplayNumber(UTextBlock* TextNumberBlock, int32 TextNumber);

public:
	FORCEINLINE int32 GetSlotID() const { return BuildSlot.SlotID; }
protected:
	struct FBuildSlot
	{
		FBuildSlot()
			:MaxCD(1.f)
			,SlotID(INDEX_NONE)
		{}
		float MaxCD;
		int32 SlotID;
	} BuildSlot;
};
