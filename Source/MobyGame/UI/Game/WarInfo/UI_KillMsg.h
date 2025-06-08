// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_KillMsg.generated.h"

class UTextBlock;
class UImage;
struct FKillNetPackage;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_KillMsg : public UUI_Base
{
	GENERATED_BODY()
	
	//»÷É±Õß
	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillerNameText;

	//±»É±Õß
	UPROPERTY(meta = (BindWidget))
	UTextBlock* KilledNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KilledTypeText;

	UPROPERTY(meta = (BindWidget))
	UImage* KilledIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* KillerIcon;

public:
	virtual void NativeConstruct() override;
	void UpdateSlot();

protected:
	void Kill(const FKillNetPackage& NetPackage, const FText& killName);
	void SerialKill(const FKillNetPackage& NetPackage, const FText& killName);
	void AchievementKill(const FKillNetPackage& NetPackage, const FText& killName);

	void Show();
	void BindDelegate();
};
