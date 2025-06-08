// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MobyGame/UI/Core/UI_Base.h"
#include "UI_CharacterIcon.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_CharacterIcon : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathTimeBlock;
public:
	UUI_CharacterIcon(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetDisplayNumber(int32 InNumebr);

	void Die(float InDeathTime);

	void Reborn();

	void SetBrushFromTexture(UTexture2D* NewTexture);
private:

	float DeathTime;
};
