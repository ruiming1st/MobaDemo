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

//ServerCallClient ����UpLvNetPackage�����ݽ��пͻ���UI����
void UUI_Skill::BindPlayerStateDelegate()
{
	if (AMobyGamePlayerState* InState = GetPlayerState())
	{
		//�Ƿ��������ǵĽ�ɫ����
		InState->SimpleLockSkillDelegate.BindLambda([&](bool bCharacterLockSkill)
		{
			CallSlotBreak<UUI_SkillSlot>([&](UUI_SkillSlot* InSlot) ->bool
			{
				InSlot->SetIsEnabled(!bCharacterLockSkill);
				return true;
			});
		});

		//��������ʾ�������ǵ���������
		InState->ShowSkillUpDelegate.BindLambda([&](const TArray<int32>& InDataPackage)
		{
			//��ȫ��������
			CallSlotBreak<UUI_SkillSlot>([&](UUI_SkillSlot* InSlot) ->bool
			{
				InSlot->GetUpdateLevelButton()->SetVisibility(ESlateVisibility::Visible);
				InSlot->GetUpdateLevelButton()->SetIsEnabled(true);

				return true;
			});

			//������ ָ����Щbutton����
			for (const auto &Tmp : InDataPackage)
			{
				//��ȫ��������
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

		//��������
		InState->UpdateSkillLvDelegate.BindLambda([&](const FSkillUpNetPackage &InUpNetPackage)
		{
			//�Ծ��������в���
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
				//�Ծ��������в���
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
