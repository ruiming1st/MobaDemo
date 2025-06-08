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

	//����Slot
	virtual void ResetSlot();

public:
	//����ΨһID
	void SetSlotID(int32 InID);
	
	//����ͼƬ
	void UpdateIcon(UTexture2D* InIcon);
	
	virtual void SetIconSize(const FVector2D &InSize);
protected:
	//��ʾ float����
	void DisplayNumber(UTextBlock* TextNumberBlock, float TextNumber);
	//��ʾ int����
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
