// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_TeamInfo.generated.h"

class UUI_PlayersInfo;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_TeamInfo : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* EnemyTeam;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* FriendlyTeam;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_PlayersInfo> PlayersInfoClass;
	
public:
	virtual void NativeConstruct() override;

protected:
	void OnClickedWidget();

	void OnReleasedClickedWidget();
	void BindDelegate();
};
