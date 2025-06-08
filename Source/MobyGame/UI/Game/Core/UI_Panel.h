// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Core/UI_Base.h"
#include "UI_Panel.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Panel : public UUI_Base
{
	GENERATED_BODY()
	
public:
	UUI_Panel(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void RequestUpdateSlot(const ECharacterAttributeType Type);

	UFUNCTION()
	virtual void ResponseUpdateSlots(int64 InPlayerID) {}

	void SetPlayerID(const int64& InPlayerID);

	FORCEINLINE int64 GetPlayerID() const { return PlayerID; }
protected:
	UFUNCTION()
	virtual void ResponseUpdateSlot(int64 InPlayerID, const ECharacterAttributeType Type) {}

	void BindDelegate();
private:
	int64 PlayerID;
};
