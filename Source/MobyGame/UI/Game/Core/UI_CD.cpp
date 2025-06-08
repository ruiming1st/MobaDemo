// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_CD.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Image.h"
#include "MobyGame/MobyGamePlayerState.h"

void UUI_CD::NativeConstruct()
{
	Super::NativeConstruct();

	CDMaterialDynamic = SlotCD->GetDynamicMaterial();
}

void UUI_CD::SetIconSize(const FVector2D& InSize)
{
	Super::SetIconSize(InSize);

	SlotCD->SetBrushSize(InSize);
}

void UUI_CD::ResetSlot()
{
	Super::ResetSlot();

	SlotCD->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_CD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUI_CD::DrawSlotCDMat(float InSlotCD)
{
	if (CDMaterialDynamic)
	{
		if (InSlotCD > 0.0f && InSlotCD < 1.0f)
		{
			CDMaterialDynamic->SetScalarParameterValue(SlotClearValueName, true);
			SlotCD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			CDMaterialDynamic->SetScalarParameterValue(SlotClearValueName, false);
			SlotCD->SetVisibility(ESlateVisibility::Hidden);
		}

		CDMaterialDynamic->SetScalarParameterValue(SlotMatCDName, InSlotCD);
	}
}
