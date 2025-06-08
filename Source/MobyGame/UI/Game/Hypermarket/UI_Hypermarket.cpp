// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Hypermarket.h"
#include "Components/CheckBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "ThreadManage.h"
#include "Item/UI_Item.h"
#include "Components/UniformGridSlot.h"
#include "MobyGame/Table/SlotTable.h"
#include "UI_EquipmentCombination.h"

UUI_Hypermarket::UUI_Hypermarket(const FObjectInitializer& ObjectInitializer)
{
	Time = 0.f;
}

void UUI_Hypermarket::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction(TEXT("InventoryKey_P"), IE_Pressed, this, &UUI_Hypermarket::OnNativeKey);

	TopButton->OnClicked.AddDynamic(this, &UUI_Hypermarket::OnClickedWidget);

	All->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxAll);
	Consumables->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxConsumables);
	AuxiliaryProps->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxAuxiliaryProps);
	PhysicalAttack->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxPhysicalAttack);
	MagicAttack->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxMagicAttack);
	PhysicalPenetration->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxPhysicalPenetration);
	MagicPenetration->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxMagicPenetration);
	Crit->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxCrit);
	Armor->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxArmor);
	MagicResistance->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxMagicResistance);
	AttackSpeed->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxAttackSpeed);
	Shoes->OnCheckStateChanged.AddDynamic(this, &UUI_Hypermarket::CheckBoxShoes);

	CheckBoxArray.Add(All);
	CheckBoxArray.Add(Consumables);
	CheckBoxArray.Add(AuxiliaryProps);
	CheckBoxArray.Add(PhysicalAttack);
	CheckBoxArray.Add(MagicAttack);
	CheckBoxArray.Add(PhysicalPenetration);
	CheckBoxArray.Add(MagicPenetration);
	CheckBoxArray.Add(Crit);
	CheckBoxArray.Add(Armor);
	CheckBoxArray.Add(MagicResistance);
	CheckBoxArray.Add(AttackSpeed);
	CheckBoxArray.Add(Shoes);

	//默认先隐藏
	SetVisibility(ESlateVisibility::Hidden);
}

void UUI_Hypermarket::OnNativeKey()
{
	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);

		UpdateItem(ESlotType::SLOT_ALL);
	}
	else
	{
		OnClickedWidget();
	}
}

void UUI_Hypermarket::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//实时监测金币是否足够，不够把物品变灰
	Time += InDeltaTime;
	if (Time >= 0.2f)
	{
		Time = 0.f;

		if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
		{
			for (auto &Tmp : ItemGrid->GetAllChildren())
			{
				if (UUI_Item* InItem = Cast<UUI_Item>(Tmp))
				{
					if (InPlayerState->IsScopeStore())
					{
						if (InPlayerState->GetPlayerData()->Gold < InItem->GetItemCostGold())
						{
							InItem->SetIsEnabled(false);
						}
						else
						{
							InItem->SetIsEnabled(true);
						}
					}

					
				}
			}
		
	
		}
	}
}

void UUI_Hypermarket::UpdateItem(ESlotType Type)
{
	ItemGrid->ClearChildren();

	if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
	{
		if (const TArray<FSlotTable*>* InSlotTableArray = InPlayerState->GetSlotTablesTemplate())
		{
			TArray<const FSlotTable*> InTableArray;
			for (const auto& Tmp : *InSlotTableArray)
			{
				if (Tmp->SlotType.Contains(Type))
				{
					InTableArray.Add(Tmp);
				}
			}

			for (int32 i = 0; i < InTableArray.Num(); i++)
			{
				if (UUI_Item* InSlotWidget = CreateWidget<UUI_Item>(GetWorld(), ItemClass))
				{
					if (UUniformGridSlot* GridSlot = ItemGrid->AddChildToUniformGrid(InSlotWidget))
					{
						//i = 0 , 1  2 3  ,4,5
						//(0,0) (0,1)
						//(1,0) (1,1)
						//(2,0) (2,1)
						GridSlot->SetRow(FMath::FloorToInt((float)i / 2.f));
						if (i & 0x1)
						{
							GridSlot->SetColumn(1);
						}
						else
						{
							GridSlot->SetColumn(0);
						}

						GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
						GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

						InSlotWidget->UpdateSlot(InTableArray[i]);

						InSlotWidget->CallEquipmentSynthesisDelegate.BindUObject(this, &UUI_Hypermarket::OnCallUpdateEquipmentCombination);
					}
				}
			}
		}
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			UpdateItem(ESlotType::SLOT_ALL);
		});
	}
}

void UUI_Hypermarket::OnClickedWidget()
{
	SetVisibility(ESlateVisibility::Hidden);

	if (ItemGrid)
	{
		ItemGrid->ClearChildren();
	}
	
}

void UUI_Hypermarket::CheckBoxAll(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	All->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_ALL);
}

void UUI_Hypermarket::CheckBoxConsumables(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	Consumables->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_CONSUMABLES);

}

void UUI_Hypermarket::CheckBoxAuxiliaryProps(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	AuxiliaryProps->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_AUXILIARYPROPS);

}

void UUI_Hypermarket::CheckBoxPhysicalAttack(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	PhysicalAttack->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_PHYSICALATTACK);

}

void UUI_Hypermarket::CheckBoxMagicAttack(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	MagicAttack->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_MAGICATTACK);

}

void UUI_Hypermarket::CheckBoxPhysicalPenetration(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	PhysicalPenetration->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_PHYSICALPENETRATION);

}

void UUI_Hypermarket::CheckBoxMagicPenetration(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	MagicPenetration->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_MAGICPENETRATION);

}

void UUI_Hypermarket::CheckBoxCrit(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	Crit->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_CRIT);

}

void UUI_Hypermarket::CheckBoxArmor(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	Armor->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_ARMOR);

}

void UUI_Hypermarket::CheckBoxMagicResistance(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	MagicResistance->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_MAGICRESISTANCE);

}

void UUI_Hypermarket::CheckBoxAttackSpeed(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	AttackSpeed->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_ATTACKSPEED);

}

void UUI_Hypermarket::CheckBoxShoes(bool bIsChecked)
{
	SetCheckBoxArray(ECheckBoxState::Unchecked);
	Shoes->SetCheckedState(ECheckBoxState::Checked);
	UpdateItem(ESlotType::SLOT_SHOES);

}

void UUI_Hypermarket::SetCheckBoxArray(ECheckBoxState CheckBoxState)
{
	for (auto &Tmp : CheckBoxArray)
	{
		Tmp->SetCheckedState(CheckBoxState);
	}
}

FReply UUI_Hypermarket::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();

	if (HasMouseCapture())
	{
		if (UWidgetTree* InWidgetTree = Cast<UWidgetTree>(GetOuter()))
		{
			if (UWidget* InParentWidget = Cast<UWidget>(InWidgetTree->GetOuter()))
			{
				const FGeometry& InParentGeometry = InParentWidget->GetTickSpaceGeometry();
				const FVector2D& NewPostion = InMouseEvent.GetScreenSpacePosition();

				FVector2D NewOffsetA = InParentGeometry.AbsoluteToLocal(NewPostion) - Offset;
				FVector2D NewOffsetB = InParentGeometry.GetLocalSize() - GetDesiredSize();
				UCanvasPanelSlot* NewPanelSlot = Cast<UCanvasPanelSlot>(Slot);
				if (NewPanelSlot)
				{
					NewPanelSlot->SetPosition(FVector2D(FMath::Clamp(NewOffsetA.X, 0.f, NewOffsetB.X), FMath::Clamp(NewOffsetA.Y, 0.f, NewOffsetB.Y)));
					Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
				}
			}
		}
	}

	return Reply;
}

FReply UUI_Hypermarket::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton || InMouseEvent.IsTouchEvent())
	{
		Offset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		if (Offset.Y <= 80.f)
		{
			Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
		}
		else
		{
			Reply.ReleaseMouseCapture();
		}
	}

	return Reply;
}

FReply UUI_Hypermarket::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = FReply::Handled();

	Reply.ReleaseMouseCapture();
	return Reply;
}

void UUI_Hypermarket::OnCallUpdateEquipmentCombination(int32 InItemSlotID)
{
	if (const FSlotTable* InSlotTable = GetPlayerState()->GetSlotTableTemplate(InItemSlotID))
	{
		EquipmentCombination->UpdateSlot(InSlotTable);
	}
}
