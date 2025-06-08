#include "AssistSystem.h"

FAssistPlayer::FAssistPlayer() : PlayerID(INDEX_NONE), AssistTime(0.f)
{

}

void FAssistSystem::Tick(float DeltaSeconds)
{
	TArray<FAssistPlayer> RemoveSlots;
	for (auto &Tmp : Assists)
	{
		Tmp.AssistTime -= DeltaSeconds;
		if (Tmp.AssistTime <= 0)
		{
			RemoveSlots.Add(Tmp);
		}
	}

	for (auto &Tmp : RemoveSlots)
	{
		Assists.Remove(Tmp);
	}
}

void FAssistSystem::AddAssist(const int64& InPlayerID)
{
	if (InPlayerID != INDEX_NONE)
	{
		FAssistPlayer AssistPlayer;
		AssistPlayer.PlayerID = InPlayerID;
		AssistPlayer.AssistTime = 10.f;

		if (Assists.Contains(AssistPlayer))
		{
			int32 index = Assists.Find(AssistPlayer);
			if (index != INDEX_NONE)
			{
				//已经存在，只需要更新时间
				Assists[index].AssistTime = 10.f;
			}
		}
		else
		{
			Assists.Add(AssistPlayer);
		}
	}
}

const FAssistPlayer* FAssistSystem::GetLastAssist()
{
	FAssistPlayer* InAssistPlayer = NULL;

	float LastTime = 0.f;
	for (auto &Tmp : Assists)
	{
		if (LastTime < Tmp.AssistTime)
		{
			LastTime = Tmp.AssistTime;
			InAssistPlayer = &Tmp;
		}
	}

	return InAssistPlayer;
}

