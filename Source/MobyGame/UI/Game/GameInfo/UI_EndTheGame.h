// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_EndTheGame.generated.h"

class UButton;
class UTexture2D;
class UImage;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_EndTheGame : public UUI_Base
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UButton* ReturnGameButton;

	UPROPERTY(meta = (BindWidget))
	UImage* Background;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	UTexture2D* VictoryTexture;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	UTexture2D* FailTexture;

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ReturnGame();

	void Victory();
	void Fail();
};
