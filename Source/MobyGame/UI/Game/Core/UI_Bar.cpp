// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Bar.h"
#include "MobyGame/MobyGamePlayerState.h"
#include "UI_Slot.h"
#include "ThreadManage.h"

void UUI_Bar::NativeConstruct()
{
	Super::NativeConstruct();

	GThread::Get()->GetCoroutines().BindLambda(0.3f, [&]()
	{
		//�����֪ͨ�ͻ��˻�ȡ׼���õ����ݣ���ʼ��Slot
		if (AMobyGamePlayerState * InPlayerState = GetPlayerState())
		{
			InPlayerState->InitSlotsDelegate.AddLambda([&](const TArray<int32> &InKeys)
			{
				LayoutSlot(InKeys);
			});

			InPlayerState->UpdateSlotDelegate.AddLambda([&](int32 InID)
			{
				CallSlotBreak<UUI_Slot>([&](UUI_Slot* InSlot)->bool
				{
					if (InSlot->GetSlotID() == InID)
					{
						InSlot->UpdateSlot();
						return false;
					}

					return true;
				});
			});

			//��ʼ����CD
			InPlayerState->StartUpdateCDSlotDelegate.AddLambda([&](int32 InID)
			{
				CallSlotBreak<UUI_Slot>([&](UUI_Slot* InSlot)->bool
				{
					if (InSlot->GetSlotID() == InID)
					{
						InSlot->StartUpdateCD();
						return false;
					}

					return true;
				});
			});

			//��������CD
			InPlayerState->EndUpdateCDSlotDelegate.AddLambda([&](int32 InID)
			{
				CallSlotBreak<UUI_Slot>([&](UUI_Slot* InSlot)->bool
				{
					if (InSlot->GetSlotID() == InID)
					{
						InSlot->EndUpdateCD();
						return false;
					}

					return true;
				});
			});
		}
	});
}
