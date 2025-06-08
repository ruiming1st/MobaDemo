// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../Core/UI_Panel.h"
#include "UI_CharacterInfo.generated.h"

class UTextBlock;
class UImage;
enum class ECharacterAttributeType :uint8;
class UUI_CharacterIcon;
/**
 *
 */
UCLASS()
class MOBYGAME_API UUI_CharacterInfo : public UUI_Panel
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PhysicalAttack;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Armor;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PhysicalPenetration;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackSpeed;

	UPROPERTY(meta = (BindWidget))
	UUI_CharacterIcon* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MagicAttack;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MagicDefense;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MagicPenetration;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WalkSpeed;
public:
	UUI_CharacterInfo(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void ResponseUpdateSlots(int64 InPlayerID);

	void Die(float InDeathTime);

	void Reborn();

	void SetDisplayNumber(int32 Numebr);
protected:
	virtual void ResponseUpdateSlot(int64 InPlayerID, const ECharacterAttributeType Type);

};
