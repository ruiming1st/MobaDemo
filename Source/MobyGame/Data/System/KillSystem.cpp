#include "KillSystem.h"

void FKillSystem::AddKiller(const int64& InPlayerID)
{
	if (!IsExistPlayer(InPlayerID))
	{
		Players.Add(FKillModel(InPlayerID));
	}

}

void FKillSystem::AddKiller(const int64& InKillerPlayer, const int64& InKilledPlayer)
{
	if (CurrentKillState.Contains(InKillerPlayer))
	{
		for (auto& Tmp : CurrentKillState)
		{
			if (Tmp.CurrentKiller == InKillerPlayer)
			{
				Tmp.ResetTime();
				break;
			}
		}
	}
	else
	{
		CurrentKillState.Add(FKill(InKillerPlayer, InKilledPlayer));
	}
}

bool FKillSystem::IsExistPlayer(const int64& InPlayerID)
{
	for (auto &Tmp : Players)
	{
		if (Tmp.Killer == InPlayerID)
		{
			return true;
		}
	}

	return false;
}

void FKillSystem::Tick(float DeltaSeconds)
{
	TArray<FKill> RemoveSlots;
	for (auto &Tmp : CurrentKillState)
	{
		Tmp.KillTime -= DeltaSeconds;
		if (Tmp.KillTime <= 0.f)
		{
			RemoveSlots.Add(Tmp);
		}
	}

	for (auto &Tmp : RemoveSlots)
	{
		CurrentKillState.Remove(Tmp);
	}
}

void FKillSystem::Kill(const int64& InKillerPlayer, const int64& InKilledPlayer)
{
	AddKiller(InKillerPlayer, InKilledPlayer);
	uint8 CallKillFunctionCount = 0;
	for (auto &Tmp : CurrentKillState)
	{
		if (Tmp.CurrentKiller == InKillerPlayer)
		{
			Tmp.KillNumber++;

			switch (Tmp.KillNumber)
			{
			case 2:
				DoubleKills.Add(Tmp.CurrentKiller);
				DoubleKillsFunction(InKillerPlayer, InKilledPlayer);
				CallKillFunctionCount = 1;
				break;
			case 3:
				TripleKills.Add(Tmp.CurrentKiller);
				TripleKillsFunction(InKillerPlayer, InKilledPlayer);
				CallKillFunctionCount = 1;
				break;
			case 4:
				QuadraKills.Add(Tmp.CurrentKiller);
				QuadraKillsFunction(InKillerPlayer, InKilledPlayer);
				CallKillFunctionCount = 1;
				break;
			case 5:
				PentaKills.Add(Tmp.CurrentKiller);
				PentaKillsFunction(InKillerPlayer, InKilledPlayer);
				CallKillFunctionCount = 1;
				break;
			default:
				break;
			}

			//执行一血，后续不执行
			if (Blood == INDEX_NONE)
			{
				Blood = InKillerPlayer;
				BloodFunction(InKillerPlayer, InKilledPlayer);
				CallKillFunctionCount = 1;
			}

			//无人能敌 超神	
			for (auto &TmpPlayer : Players)
			{

				if (TmpPlayer.Killer == InKillerPlayer)
				{
					TmpPlayer.KillNumber++;
					if (CallKillFunctionCount == 0)
					{
						if (TmpPlayer.KillNumber >= 3 && TmpPlayer.KillNumber < 6)
						{
							Invincible.Add(InKillerPlayer);
							InvincibleFunction(InKillerPlayer, InKilledPlayer);
							CallKillFunctionCount = 1;
						}
						else if (TmpPlayer.KillNumber >= 6)
						{
							Legendary.Add(InKillerPlayer);
							LegendaryFunction(InKillerPlayer, InKilledPlayer);
							CallKillFunctionCount = 1;
						}

						break;
					}
				}
			}


			if (CallKillFunctionCount == 0)
			{
				KillFunction(InKillerPlayer, InKilledPlayer);
			}

			break;
		}
	}

	AceFunction(InKillerPlayer, InKilledPlayer);
}

void FKillSystem::Death(const int64& InKilledPlayer)
{
	for (auto &Tmp : Players)
	{
		if (Tmp.Killer == InKilledPlayer)
		{
			Tmp.KillNumber = 0;
			Tmp.Killed = INDEX_NONE;
			break;
		}
	}

	for (auto &Tmp : CurrentKillState)
	{
		if (Tmp.CurrentKiller == InKilledPlayer)
		{
			Tmp.KillTime = 0.f;
			break;
		}
	}
}

FKillSystem::FKillSystem() : Blood(INDEX_NONE)
{

}

FKillSystem::FKillModel::FKillModel(const int64& InKiller) : KillNumber(0), Killer(InKiller), Killed(INDEX_NONE)
{

}

FKillSystem::FKill::FKill(const int64& InKillerPlayer, const int64& InKilledPlayer) : CurrentKiller(InKillerPlayer), CurrentKilled(InKilledPlayer), KillNumber(0)
{
	ResetTime();
}

void FKillSystem::FKill::ResetTime()
{
	KillTime = 30.f;
}