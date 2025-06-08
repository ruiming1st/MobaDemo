// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI_HealthBar.h"
#include "UI_InformationBar.generated.h"

/**
 * 
 */

class UTextBlock;
UCLASS()
class MOBYGAME_API UUI_InformationBar : public UUI_HealthBar
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Lv;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* Mana;

public:

	virtual void NativeConstruct() override;

	void SetTitle(const FString &Msg);
	void SetLv(const int32 &InLv);
	void SetMana(float InPercentage);
};
