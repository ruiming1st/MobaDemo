// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_GoHome.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_GoHome : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UProgressBar* GoHomeBar;

public:
	UUI_GoHome(const FObjectInitializer& ObjectInitializer);

	void GoingHome();

	void ReadyGoingHome();
	void CancelGoingHome();

	void StartGoHomeAnim();
	void EndGoHomeAnim();

	bool IsDie();
	void Cancel();
protected:
	virtual void NativeConstruct() override;

	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void GoHome();

	void CheckCharacterMove();
private:
	float CountDown;
	bool bStartCountdown;

};
