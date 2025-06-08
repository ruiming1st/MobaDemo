// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Inventory/Drop/UI_NativeOnDrop.h"
#include "UI_Hypermarket.generated.h"

class UCheckBox;
class UUniformGridPanel;
class UUI_Item;
class UButton;
class UUI_EquipmentCombination;
enum class ESlotType : uint8;
/**
 * 
 */
UCLASS()
class MOBYGAME_API UUI_Hypermarket : public UUI_NativeOnDrop
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UCheckBox* All;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* Consumables;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* AuxiliaryProps;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* PhysicalAttack;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* MagicAttack;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* PhysicalPenetration;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* MagicPenetration;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* Crit;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* Armor;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* MagicResistance;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* AttackSpeed;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* Shoes;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* ItemGrid;

	UPROPERTY(meta = (BindWidget))
	UButton* TopButton;

	UPROPERTY(meta = (BindWidget))
	UUI_EquipmentCombination* EquipmentCombination;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_Item> ItemClass;

public:
	UUI_Hypermarket(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void NativeConstruct() override;

	//Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void UpdateItem(ESlotType Type);

protected:
	UFUNCTION(BlueprintCallable)
	void OnClickedWidget();

	UFUNCTION(BlueprintCallable)
	void OnNativeKey();

	UFUNCTION(BlueprintCallable)
	void OnCallUpdateEquipmentCombination(int32 InItemSlotID);

protected:
	UFUNCTION()
	void CheckBoxAll(bool bIsChecked);

	UFUNCTION()
	void CheckBoxConsumables(bool bIsChecked);

	UFUNCTION()
	void CheckBoxAuxiliaryProps(bool bIsChecked);

	UFUNCTION()
	void CheckBoxPhysicalAttack(bool bIsChecked);

	UFUNCTION()
	void CheckBoxMagicAttack(bool bIsChecked);

	UFUNCTION()
	void CheckBoxPhysicalPenetration(bool bIsChecked);

	UFUNCTION()
	void CheckBoxMagicPenetration(bool bIsChecked);

	UFUNCTION()
	void CheckBoxCrit(bool bIsChecked);

	UFUNCTION()
	void CheckBoxArmor(bool bIsChecked);

	UFUNCTION()
	void CheckBoxMagicResistance(bool bIsChecked);

	UFUNCTION()
	void CheckBoxAttackSpeed(bool bIsChecked);

	UFUNCTION()
	void CheckBoxShoes(bool bIsChecked);

protected:
	void SetCheckBoxArray(ECheckBoxState CheckBoxState);

protected:
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
private:
	TArray<UCheckBox*> CheckBoxArray;
	FVector2D Offset;

	float Time;
};
