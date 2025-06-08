// Fill out your copyright notice in the Description page of Project Settings.

#include "NonMobyGamePlayer.h"
#include "ThreadManage.h"

void ANonMobyGamePlayer::Die()
{
	GThread::Get()->GetCoroutines().BindLambda(2.f, [&]()
	{
		//服务器移除相关属性
		if (AMobyGameState *InGameState = GetWorld()->GetGameState<AMobyGameState>())
		{
			//地图 角色坐标
			InGameState->RemoveCharacterAILocation(GetPlayerID());

			//移除数据库角色数据
			InGameState->RemoveCharacterData(GetPlayerID());
		}

		Destroy();
	});
}