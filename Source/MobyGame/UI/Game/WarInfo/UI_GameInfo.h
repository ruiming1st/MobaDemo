// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_GameInfo.generated.h"

class UTextBlock;
class UUI_KillInfo;
/**
 *
 */
UCLASS()
class MOBYGAME_API UUI_GameInfo : public UUI_Base
{
	GENERATED_BODY()

		//����ɱ������
		UPROPERTY(meta = (BindWidget))
		UTextBlock *NumberOfEnemyKilledText;

	//�Ѿ�ɱ������
	UPROPERTY(meta = (BindWidget))
		UTextBlock* NumberOfFriendlyKilledText;

	//�Ѿ�ɱ������
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TimeText;

	//�Ѿ�ɱ������
	UPROPERTY(meta = (BindWidget))
		UUI_KillInfo* KillInfo;

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void BindDelegate();
private:
	FString GetCurrentCount(float NewTimeCount);
};
