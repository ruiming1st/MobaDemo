// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_EquipmentCombination.h"
#include "Components/CanvasPanel.h"
#include "MobyGame/Table/SlotTable.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

struct FDrawSlot
{
	//绘制画布里面元素的坐标
	struct FSlot
	{
		FSlot(UCanvasPanelSlot* InSlot) : Slot(InSlot)
		{}

		void SetPosition(const FVector2D &InPosition);
		FVector2D GetSize() const;
	private:
		UCanvasPanelSlot* Slot;
	};

	struct FTable : public FSlot 
	{
		FTable(UCanvasPanelSlot* InSlot, const FSlotTable* InTableSlot) : FSlot(InSlot),TableSlot(InTableSlot)
		{}

		const FSlotTable* GetSlotTable();
	private:
		const FSlotTable* TableSlot;
	};

	TArray<FSlot> LREndPanel;
	TArray<FSlot> VLinePanel;
	TArray<FSlot> HLinePanel;
	TArray<FSlot> TLinePanel;
	TArray<FSlot> LRVPanel;
	TArray<FTable> ItemPanel;
};

const FSlotTable* FDrawSlot::FTable::GetSlotTable()
{
	return TableSlot;
}

void FDrawSlot::FSlot::SetPosition(const FVector2D &InPosition)
{
	if (Slot)
	{
		Slot->SetPosition(InPosition);
	}
}

FVector2D FDrawSlot::FSlot::GetSize() const
{
	return Slot != NULL ? Slot->GetSize() : FVector2D::ZeroVector;
}

UUI_EquipmentCombination::UUI_EquipmentCombination()
{
	StartPosition = FVector2D(204.f, 28.f);
	IconSize = FVector2D(32.f);
}

void UUI_EquipmentCombination::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_EquipmentCombination::UpdateSlot(const FSlotTable *InTable)
{
	EquipmentPanel->ClearChildren();

	if (InTable)
	{
		//算出深度
		int32 InLayer = GetLayerDepth(InTable);

		if (UUI_ItemSlot* InMainSlotWidget = CreateWidget<UUI_ItemSlot>(GetWorld(), ItemSlotClass))
		{
			InMainSlotWidget->UpdateSlot(InTable);
			if (UCanvasPanelSlot* InPanelSlot = EquipmentPanel->AddChildToCanvas(InMainSlotWidget))
			{
				InPanelSlot->SetSize(IconSize);
				InPanelSlot->SetAlignment(FVector2D(0.5f, 0.f));
				InPanelSlot->SetPosition(StartPosition);

				RecursiveUpdateSlot(InTable, StartPosition, --InLayer);
			}
		}
	}
}

int32 UUI_EquipmentCombination::GetLayerDepth(const FSlotTable* InTable, int32 InDepth)
{
	InDepth++;

	TArray<int32> Depths;
	for (auto &Tmp : InTable->Childrens)
	{
		if (const FSlotTable* InSlotTable = GetPlayerState()->GetSlotTableTemplate(Tmp))
		{
			Depths.Add(GetLayerDepth(InSlotTable, InDepth));
		}
	}

	//找出最深的一个节点
	for (auto &Tmp : Depths)
	{
		if (Tmp > InDepth)
		{
			InDepth = Tmp;
		}
	}

	return InDepth;
}

void UUI_EquipmentCombination::RecursiveUpdateSlot(const FSlotTable* InTable, const FVector2D &InStartPostion, FSlotLayer InSlotLayer)
{
	if (InSlotLayer < 0)
	{
		InSlotLayer = 0;
	}

	int32 NumberChildrens = InTable->Childrens.Num();
	FDrawSlot DrawSlot;

	auto SpawnSlot = [&](TArray<FDrawSlot::FSlot> &PanelSlots, UTexture2D* InTexture, int32 InLayer = 1) 
	{
		if (UImage *InImage = NewObject<UImage>(this))
		{
			InImage->SetBrushFromTexture(InTexture);
			if (UCanvasPanelSlot* InPanelSlot = EquipmentPanel->AddChildToCanvas(InImage))
			{
				InPanelSlot->SetSize(FVector2D(IconSize.X * InLayer, IconSize.Y));
				InPanelSlot->SetAlignment(FVector2D(0.5f, 0.f));

				PanelSlots.Add(FDrawSlot::FSlot(InPanelSlot));
			}

		}
	};

	auto SpawnItem = [&](TArray<FDrawSlot::FTable> &PanelSlots, const FSlotTable* InTable)
	{
		if (UUI_ItemSlot* InMainSlotWidget = CreateWidget<UUI_ItemSlot>(GetWorld(), ItemSlotClass))
		{
			InMainSlotWidget->UpdateSlot(InTable);
			if (UCanvasPanelSlot* InPanelSlot = EquipmentPanel->AddChildToCanvas(InMainSlotWidget))
			{
				InPanelSlot->SetSize(IconSize);
				InPanelSlot->SetAlignment(FVector2D(0.5f, 0.f));

				PanelSlots.Add(FDrawSlot::FTable(InPanelSlot, InTable));
			}
		}
	};

	if (NumberChildrens == 1)
	{
		SpawnSlot(DrawSlot.VLinePanel, LineTexture.VerticalLine);
	}
	else if (NumberChildrens >= 2)
	{
		SpawnSlot(DrawSlot.LREndPanel, LineTexture.TurnRight);
		SpawnSlot(DrawSlot.LREndPanel, LineTexture.TurnLeft);
		SpawnSlot(DrawSlot.LRVPanel, LineTexture.TurnLeftAndRightVertically);

		//T
		for (int32 i = 0; i < NumberChildrens - 2; i++)
		{
			SpawnSlot(DrawSlot.TLinePanel, LineTexture.TLine);
		}

		for (int32 i = 0; i < NumberChildrens; i++)
		{
			SpawnSlot(DrawSlot.HLinePanel, LineTexture.HorizontalLine, InSlotLayer);
		}
	}

	//生成item
	for (auto &Tmp : InTable->Childrens)
	{
		if (const FSlotTable* InSlotTable = GetPlayerState()->GetSlotTableTemplate(Tmp))
		{
			SpawnItem(DrawSlot.ItemPanel, InSlotTable);
		}
	}

	int32 ValidNumber = DrawSlot.HLinePanel.Num() +
		DrawSlot.LREndPanel.Num() +
		DrawSlot.LRVPanel.Num() +
		DrawSlot.TLinePanel.Num() +
		DrawSlot.VLinePanel.Num();

	//排列
	if (NumberChildrens == 1)
	{
		FDrawSlot::FSlot InVLin = DrawSlot.VLinePanel.Pop();
		FDrawSlot::FSlot InItem = DrawSlot.ItemPanel.Pop();
		FVector2D NewPos = FVector2D(InStartPostion.X, InStartPostion.Y + InVLin.GetSize().Y);
		InVLin.SetPosition(NewPos);
		NewPos = FVector2D(NewPos.X, NewPos.Y + InVLin.GetSize().Y);
		InItem.SetPosition(NewPos);
	}
	else if (NumberChildrens >= 2)
	{
		FDrawSlot::FSlot InLRVLin = DrawSlot.LRVPanel.Pop();
		FVector2D NewPos = FVector2D(InStartPostion.X, InStartPostion.Y + InLRVLin.GetSize().Y);
		InLRVLin.SetPosition(NewPos);

		float HWidth = DrawSlot.HLinePanel[0].GetSize().X;
		FVector2D R = NewPos;
		FVector2D L = NewPos;

		auto Add = [&](int32 i, TArray<FDrawSlot::FSlot> &InPanels, FSlotLayer InSlotLayer)
		{
			FVector2D InNewValidPosition = FVector2D::ZeroVector;

			FDrawSlot::FSlot InNewSlot = InPanels.Pop();
			FDrawSlot::FTable InItem = DrawSlot.ItemPanel.Pop();
			float NestSpacing = HWidth / 2 + InNewSlot.GetSize().X / 2;

			if (i & 0x1)
			{
				L = FVector2D(L.X - NestSpacing, L.Y);
				InNewValidPosition = L;
			}
			else
			{
				R = FVector2D(R.X + NestSpacing, R.Y);
				InNewValidPosition = R;
			}

			InNewSlot.SetPosition(InNewValidPosition);

			FVector2D InNewItemPosition = FVector2D(InNewValidPosition.X, InNewValidPosition.Y + InItem.GetSize().Y);

			InItem.SetPosition(InNewItemPosition);

			RecursiveUpdateSlot(InItem.GetSlotTable(), InNewItemPosition, InSlotLayer -= 2);
		};

		//绘制水平
		for (int32 i = 0; i < ValidNumber; i++)
		{
			FVector2D InNewValidPosition = FVector2D::ZeroVector;

			if (DrawSlot.HLinePanel.Num())
			{
				FDrawSlot::FSlot InHor = DrawSlot.HLinePanel.Pop();
				float NestSpacing = InHor.GetSize().X / 2.f + InLRVLin.GetSize().X / 2.f;

				if (i & 0x1)
				{
					L = FVector2D(L.X - NestSpacing, L.Y);
					InNewValidPosition = L;
				}
				else
				{
					R = FVector2D(R.X + NestSpacing, R.Y);
					InNewValidPosition = R;
				}

				InHor.SetPosition(InNewValidPosition);
			}
		
					
			//绘制T
			if (DrawSlot.TLinePanel.Num())
			{
				Add(i, DrawSlot.TLinePanel, InSlotLayer);
			}
		}

		//绘制拐点
		for (int32 i = 0; i < 2; i++)
		{
			Add(i, DrawSlot.LREndPanel, InSlotLayer);
		}
	}
}
