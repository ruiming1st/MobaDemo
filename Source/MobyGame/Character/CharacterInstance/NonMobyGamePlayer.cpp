// Fill out your copyright notice in the Description page of Project Settings.

#include "NonMobyGamePlayer.h"
#include "ThreadManage.h"

void ANonMobyGamePlayer::Die()
{
	GThread::Get()->GetCoroutines().BindLambda(2.f, [&]()
	{
		//�������Ƴ��������
		if (AMobyGameState *InGameState = GetWorld()->GetGameState<AMobyGameState>())
		{
			//��ͼ ��ɫ����
			InGameState->RemoveCharacterAILocation(GetPlayerID());

			//�Ƴ����ݿ��ɫ����
			InGameState->RemoveCharacterData(GetPlayerID());
		}

		Destroy();
	});
}