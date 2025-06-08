// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_HealthBar.generated.h"

class UProgressBar;
class UUI_Buff;

UCLASS()
class MOBYGAME_API UUI_HealthBar : public UUI_Base
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Health;

	UPROPERTY(meta = (BindWidget))
	UUI_Buff* Buff;

public:
	UFUNCTION()
	void UpdateCD(int64 InPlayerID, int32 InSlotID, float MaxCD);

	UFUNCTION()
	void EndUpdateCD(int64 InPlayerID, int32 InSlotID);

public:
	UUI_HealthBar(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct();
	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetHealth(float InPercentage);
	void SetColor(const FLinearColor& InColor);

	void BindDelegate();

	void RemoveDelegate();

	void SetPlayerID(int64 InPlayerID);

	int64 GetPlayerID();
protected:
	int64 PlayerID;

	//代理是否执行的标记
	FDelegateHandle UpdateDelegateHandle;
	FDelegateHandle EndDelegateHandle;
};
