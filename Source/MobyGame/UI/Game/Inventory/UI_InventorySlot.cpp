// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_InventorySlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DragDrop/UI_ICODragDrog.h"
#include "Blueprint/DragDropOperation.h"
#include "Input/Reply.h"
#include "MobyGame/Data/MisData.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

static int32 InventoryNumber = 1;

UUI_InventorySlot::UUI_InventorySlot()
{
	bDrag = false;
}

UWidget* UUI_InventorySlot::GetTip()
{
	return VerifyTip(GetPlayerState(), Cast<UUI_Tip>(Super::GetTip()), GetSlotID());
}

void UUI_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	FString InventoryNumberString = FString::Printf(TEXT("InventoryKey_%i"), InventoryNumber);
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction(*InventoryNumberString, IE_Pressed, this, &UUI_InventorySlot::OnClickedWidget);
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction(*InventoryNumberString, IE_Released, this, &UUI_InventorySlot::OnReleasedClickedWidget);
	SetKeyName(FString::FromInt(InventoryNumber));

	InventoryNumber++;
	if (InventoryNumber > 6)
	{
		InventoryNumber = 1;
	}
}

void UUI_InventorySlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!bDrag)
	{
		Super::NativeTick(MyGeometry, InDeltaTime);
	}
}

int32 UUI_InventorySlot::GetSlotNumber()
{
	if (FSlotData *InSlotData = GetPlayerState()->GetInventorySlotData(GetSlotID()))
	{
		return InSlotData->Number;
	}

	return INDEX_NONE;
}

bool UUI_InventorySlot::IsDie()
{
	if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
	{
		if (FCharacterAttribute* InCharacterAttribute = InPlayerState->GetCharacterAttribute())
		{
			return InCharacterAttribute->Health <= 0.f;
		}
	}

	return false;
}

void UUI_InventorySlot::OnClickedWidget()
{
	if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
	{
		//没有死才可以使用道具
		if (!IsDie())
		{
			//物品栏里面有没有相应内容
			if (InPlayerState->IsInventorySlotValid(GetSlotID()))
			{
				//检查现在CD是否空闲
				if (InPlayerState->IsCDValid(GetSlotID()))
				{
					InPlayerState->Use(GetSlotID());
				}
			}
		}
	}
}

void UUI_InventorySlot::OnReleasedClickedWidget()
{

}

void UUI_InventorySlot::UpdateSlot()
{
	Super::UpdateSlot();

	if (FSlotData *InSlotData = GetPlayerState()->GetInventorySlotData(GetSlotID()))
	{
		UpdateNumber();

		if (InSlotData->Number == 0)
		{
			ResetSlot();
		}
	}
	bDrag = false;
}

void UUI_InventorySlot::StartUpdateCD()
{
	Super::StartUpdateCD();
}

void UUI_InventorySlot::UpdateNumber()
{
	if (FSlotData *InSlotData = GetPlayerState()->GetInventorySlotData(GetSlotID()))
	{
		DisplayNumber(SlotNumber, InSlotData->Number);
	}
}

void UUI_InventorySlot::EndUpdateCD()
{
	Super::EndUpdateCD();

	UpdateNumber();
}

void UUI_InventorySlot::ResetSlot()
{
	Super::ResetSlot();

	SlotNumber->SetVisibility(ESlateVisibility::Hidden);
}

FReply UUI_InventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton || InMouseEvent.IsTouchEvent())
	{
		FReply Reply = FReply::Handled();
		TSharedPtr<SWidget> SlateWidgetDrag = GetCachedWidget();
		if (SlateWidgetDrag.IsValid())
		{
			Reply.DetectDrag(SlateWidgetDrag.ToSharedRef(), EKeys::RightMouseButton);
			return Reply;
		}
	}

	return FReply::Handled();
}

void UUI_InventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (ICODragDrogClass)
	{
		if (FSlotData *InSlotData = GetPlayerState()->GetInventorySlotData(GetSlotID()))
		{
			if (InSlotData->SlotID != INDEX_NONE)
			{

				if (UUI_ICODragDrog* ICODragDrog = CreateWidget<UUI_ICODragDrog>(GetWorld(), ICODragDrogClass))
				{
					if (UDragDropOperation* InDropOperation = NewObject<UDragDropOperation>(GetTransientPackage(), UDragDropOperation::StaticClass()))
					{
						InDropOperation->SetFlags(RF_StrongRefOnFrame);
						ICODragDrog->DrawICON(InSlotData->SlotICO);
						InDropOperation->DefaultDragVisual = ICODragDrog;
						InDropOperation->Payload = this;
						OutOperation = InDropOperation;

						ResetSlot();//隐藏自己

						bDrag = true;
					}
				}
			}
		}
	}

	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool UUI_InventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	bool bDrop = false;

	if (UDragDropOperation* InDragDropOperation = Cast<UDragDropOperation>(InOperation))
	{
		if (UUI_InventorySlot* MyInventorySlot = Cast<UUI_InventorySlot>(InDragDropOperation->Payload))
		{	
			//B拖拽至A, 服务器不同步最大CD，需要客户端交换
			const FSlotData *InventorySlotA = GetPlayerState()->GetInventorySlotData(GetSlotID());
			const FSlotData *InventorySlotB = GetPlayerState()->GetInventorySlotData(MyInventorySlot->GetSlotID());
			
			if (InventorySlotA && InventorySlotB)
			{
				if (InventorySlotA->SlotID == INDEX_NONE)//移动
				{
					BuildSlot.MaxCD = MyInventorySlot->BuildSlot.MaxCD;
				}
				else //交换
				{
					float TmpCD = MyInventorySlot->BuildSlot.MaxCD;
					MyInventorySlot->BuildSlot.MaxCD = BuildSlot.MaxCD;
					BuildSlot.MaxCD = TmpCD;
				}
			}
			
			GetPlayerState()->ClientCallServerUpdateInventory(MyInventorySlot->GetSlotID(), GetSlotID());
			bDrop = true;
		}	
	}
	
	return bDrop;
}

void UUI_InventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

}

void UUI_InventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{

}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif