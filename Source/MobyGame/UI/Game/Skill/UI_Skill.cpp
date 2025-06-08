// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Skill.h"
#include "UI_SkillSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"

UPanelWidget* UUI_Skill::GetSlotArray()
{
	return SlotArraySkill;
}

void UUI_Skill::NativeConstruct()
{
	Super::NativeConstruct();

	BindPlayerStateDelegate();
}

void UUI_Skill::LayoutSlot(const TArray<int32>& InKeys)
{
	if (!SlotArraySkill->GetChildrenCount())
	{
		if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
		{

			TArray<int32> IDs;
			InPlayerState->GetSkillAllKeys(IDs);

			if (IDs.Num())
			{
				if (SkillSlotClass)
				{
					int32 RowNumber = IDs.Num();
					for (int32 MyRow = 0; MyRow < RowNumber; MyRow++)
					{
						if (UUI_SkillSlot* SlotWidget = CreateWidget<UUI_SkillSlot>(GetWorld(), SkillSlotClass))
						{
							if (UHorizontalBoxSlot* GridSlot = SlotArraySkill->AddChildToHorizontalBox(SlotWidget))
							{
								GridSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
								GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
								GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
							}
							SlotWidget->SetSlotID(IDs[MyRow]);

							//Update
							SlotWidget->UpdateSlot();
						}
					}
				}
			}
		}
	}
}

//ServerCallClient 根据UpLvNetPackage的内容进行客户端UI操作
void UUI_Skill::BindPlayerStateDelegate()
{
	if (AMobyGamePlayerState* InState = GetPlayerState())
	{
		//是否屏蔽我们的角色技能
		InState->SimpleLockSkillDelegate.BindLambda([&](bool bCharacterLockSkill)
		{
			CallSlotBreak<UUI_SkillSlot>([&](UUI_SkillSlot* InSlot) ->bool
			{
				InSlot->SetIsEnabled(!bCharacterLockSkill);
				return true;
			});
		});

		//升级后提示亮起我们的升级按键
		InState->ShowSkillUpDelegate.BindLambda([&](const TArray<int32>& InDataPackage)
		{
			//先全部亮起来
			CallSlotBreak<UUI_SkillSlot>([&](UUI_SkillSlot* InSlot) ->bool
			{
				InSlot->GetUpdateLevelButton()->SetVisibility(ESlateVisibility::Visible);
				InSlot->GetUpdateLevelButton()->SetIsEnabled(true);

				return true;
			});

			//升级后 指定哪些button亮起
			for (const auto &Tmp : InDataPackage)
			{
				//先全部亮起来
				CallSlotBreak<UUI_SkillSlot>([&](UUI_SkillSlot* InSlot) ->bool
				{
					if (Tmp == InSlot->GetSlotID())
					{
						InSlot->GetUpdateLevelButton()->SetIsEnabled(false);
						return false;
					}

					return true;
				});
			}
		});

		//管理升级
		InState->UpdateSkillLvDelegate.BindLambda([&](const FSkillUpNetPackage &InUpNetPackage)
		{
			//对具体对象进行操作
			CallSlotBreak<UUI_SkillSlot>([&](UUI_SkillSlot* InSlot) ->bool
			{
				if (InSlot->GetSlotID() == InUpNetPackage.SlotID)
				{
					InSlot->GetSkillLv()->SetPercent((float)InUpNetPackage.Lv / 3.f);
					InSlot->GetUpdateLevelButton()->SetIsEnabled(InUpNetPackage.bEnableCurrentSlot);
					InSlot->EnableClickButton(true);

					InSlot->OnResponseUpdateLevel();
					return false;
				}

				return true;
			});

			if (InUpNetPackage.bHidAllSlot)
			{
				//对具体对象进行操作
				CallSlotBreak<UUI_SkillSlot>([&](UUI_SkillSlot* InSlot) ->bool
				{
					InSlot->GetUpdateLevelButton()->SetVisibility(ESlateVisibility::Hidden);
					return true;
				});
			}
		});
	}
	else
	{
		GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
		{
			BindPlayerStateDelegate();
		});
	}

}
