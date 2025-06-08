// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_SkillSlot.h"
#include "MobyGame/MobyGamePlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "../../../Common/MethodUnit.h"
#include "../../../MobyGamePlayerController.h"


struct FPressKey
{
	FPressKey()
	{
		Keys.Add(TEXT("R"));
		Keys.Add(TEXT("E"));
		Keys.Add(TEXT("W"));
		Keys.Add(TEXT("Q"));
	}

	TArray<FString> Keys;
} PressKey;

#define UIFENNV 
void UUI_SkillSlot::NativeConstruct()
{
	Super::NativeConstruct();

	FString KeyString = PressKey.Keys.Pop();

	FString InventoryNumberString = FString::Printf(TEXT("InventoryKey_%s"), *KeyString);
	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction(*InventoryNumberString, IE_Pressed, this, &UUI_SkillSlot::OnClickedWidget);

	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction(*InventoryNumberString, IE_Released, this, &UUI_SkillSlot::OnReleasedClickedWidget);
	SetKeyName(KeyString);

	if (KeyString == TEXT("R"))
	{
		PressKey = FPressKey();
	}

	UpdateLevelButton->OnClicked.AddDynamic(this, &UUI_SkillSlot::OnClickedUpdateLevel);
}

void UUI_SkillSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUI_SkillSlot::OnClickedWidget()
{
	if (IsEnableClickButton())
	{
		if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
		{
			//检查现在CD是否空闲
			if (InPlayerState->IsCDValid(GetSlotID()))
			{
				//判断是否需要显示预提示
				if (InPlayerState->IsSkillTip(GetSlotID()))
				{
					InPlayerState->ShowAttackTip(GetSlotID(), true);
				}
				else
				{
					InPlayerState->Use(GetSlotID());
				}
	
			}

		}
	}

}

void UUI_SkillSlot::OnReleasedClickedWidget()
{
	if (IsEnableClickButton())
	{
		if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
		{
			if (InPlayerState->IsCDValid(GetSlotID()))
			{
				if (InPlayerState->IsSkillTip(GetSlotID()))
				{
					if (AMobyGamePlayerController* InController = Cast<AMobyGamePlayerController>(GetWorld()->GetFirstPlayerController()))
					{
						FHitResult HitResult;
						InController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

						if (HitResult.bBlockingHit)
						{
							InController->ClientCallServerSetSkillRotTarget(HitResult.Location);
						}
					}

					InPlayerState->ShowAttackTip(GetSlotID(), false);
					InPlayerState->Use(GetSlotID());
				}

			}

			//不是需要提示的技能，就是持续技能停止的情况，需要取消PressKey变量
			InPlayerState->ReleaseSkill();
		}
	}
}

void UUI_SkillSlot::OnResponseUpdateLevel()
{
	BaseCache.UpdateLv();
}

void UUI_SkillSlot::UpdateSlot()
{
	Super::UpdateSlot();

	InitPlayerSkillCache();
}

UWidget* UUI_SkillSlot::GetTip()
{
	if (IsEnableClickButton())
	{
		if (UUI_Tip* InTip = Cast<UUI_Tip>(Super::GetTip()))
		{
			MethodUnit::TipRegistration(InTip, &BaseCache);

			return InTip;
		}
	}

	return NULL;
}

void UUI_SkillSlot::OnClickedUpdateLevel()
{
	if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
	{
		//升级
		InPlayerState->UpdateSkillLv(GetSlotID());
	}
}

void UUI_SkillSlot::InitPlayerSkillCache()
{
	if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
	{
		int32 InSkillID = InPlayerState->GetSkillID(GetSlotID());
		if (InSkillID == INDEX_NONE)
		{
			return;
		}

		if (AMobyGameState *InGameState = MethodUnit::GetGameState(GetWorld()))
		{
			if (const FCharacterTable* InCharacterTable = InGameState->GetCharacterTableBySkillID(InSkillID))
			{
				auto RegisterData = [&](const FSkill& InSkill)
				{
					if (const FSlotAttribute* InSlotAttribute = InPlayerState->GetSlotAttributeTemplate(InSkillID))
					{
						if (UUI_Tip *InTip = Cast<UUI_Tip>(Super::GetTip()))
						{
							InTip->SetDescribeText(InSkill.SkillIntroduce);
						}

						BaseCache = *InSlotAttribute;

						//buf lv skill
						MethodUnit::RegisterSlotAttribute(InPlayerState, BaseCache);
					}
				};

				if (InCharacterTable->QSkill.ID == InSkillID)
				{
					RegisterData(InCharacterTable->QSkill);
				}
				else if (InCharacterTable->WSkill.ID == InSkillID)
				{
					RegisterData(InCharacterTable->WSkill);
				}
				else if (InCharacterTable->ESkill.ID == InSkillID)
				{
					RegisterData(InCharacterTable->ESkill);
				}
				else if (InCharacterTable->RSkill.ID == InSkillID)
				{
					RegisterData(InCharacterTable->RSkill);
				}
			}
		}
	}
}

bool UUI_SkillSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	bool bDrop = false;

	if (UDragDropOperation* InDragDropOperation = Cast<UDragDropOperation>(InOperation))
	{
		if (UUI_Slot* MyInventorySlot = Cast<UUI_Slot>(InDragDropOperation->Payload))
		{
			MyInventorySlot->UpdateSlot();

			bDrop = true;
		}
	}

	return bDrop;
}