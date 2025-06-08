#include "MinionsSystem.h"
#include "../../Character/CharacterSpawnPoint.h"
#include "../../Common/MethodUnit.h"

FMinionsSystem::FMinionsSystem()
{

}

FSpawnMinionsTimer::FSpawnMinionsTimer()
	:Time(4.f)
	, Count(0)
{

}

void FSpawnMinionsTimer::Reset()
{
	Time = 0.f;
	Count = 0;
}

void FSpawnMinionsTimer::Tick(float DeltaSeconds)
{
	Time -= DeltaSeconds;
	if (Time <= 0.f)
	{
		Time = 1.0f;

		switch (Count)
		{
		case 0:
		case 1:
		case 2:
			Func();
			break;
		}

		Count++;
		if (Count > 2)
		{
			Count = 0;
			Time = 60.f;
		}
	}
}

void FMinionsSystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Normal.Tick(DeltaSeconds);

	//超级大兵的生成
	if (GetWorld())
	{
		if (AMobyGameState* InGameState = GetWorld()->GetGameState<AMobyGameState>())
		{
			if (InGameState->IsSpawnSuper(ETeamType::TEAM_RED))
			{
				SuperBlue.Tick(DeltaSeconds);
			}
			else
			{
				SuperBlue.Reset();
			}

			if (InGameState->IsSpawnSuper(ETeamType::TEAM_BLUE))
			{
				SuperRed.Tick(DeltaSeconds);
			}
			else
			{
				SuperRed.Reset();
			}
		}
	}
}

void FMinionsSystem::Init(const TArray<ACharacterSpawnPoint*>& InGameCharacters)
{
	//收集位置 和 组
	for (auto &Tmp : InGameCharacters)
	{
		if (Tmp->GetCharacterType() >= ECharacterType::REMOTE_MINION &&
			Tmp->GetCharacterType() <= ECharacterType::SUPER_MINION)
		{
			if (Tmp->GetType() == ETeamType::TEAM_BLUE)
			{
				if (Tmp->GetCharacterType() == ECharacterType::WARRIOR_MINION)
				{
					BlueLocations.Add(Tmp->GetActorLocation());
				}
			}
			else if (Tmp->GetType() == ETeamType::TEAM_RED)
			{
				if (Tmp->GetCharacterType() == ECharacterType::WARRIOR_MINION)
				{
					RedLocations.Add(Tmp->GetActorLocation());
				}
			}
		}
	}

	//绑定函数
	Normal.Func = [&]()
	{
		SpawnWarrior();
		SpawnRemote();
	};

	SuperBlue.Func = [&]()
	{
		SpawnSuper(ETeamType::TEAM_BLUE);
	};

	SuperRed.Func = [&]()
	{
		SpawnSuper(ETeamType::TEAM_RED);
	};
}

void FMinionsSystem::SpawnWarrior()
{
	int32 CharacterID = 11110;

	for (auto &Tmp : RedLocations)
	{
		int64 PlayerID = FMath::RandRange(1000000, 888889999);
		Spawn(PlayerID, CharacterID, Tmp, ETeamType::TEAM_RED, GetCurrentLv());
	}

	for (auto& Tmp : BlueLocations)
	{
		int64 PlayerID = FMath::RandRange(1000000, 888889999);
		Spawn(PlayerID, CharacterID, Tmp, ETeamType::TEAM_BLUE, GetCurrentLv());
	}
}

void FMinionsSystem::SpawnRemote()
{
	int32 CharacterID = 11111;

	for (auto& Tmp : RedLocations)
	{
		int64 PlayerID = FMath::RandRange(1000000, 888889999);
		Spawn(PlayerID, CharacterID, Tmp, ETeamType::TEAM_RED, GetCurrentLv());
	}

	for (auto& Tmp : BlueLocations)
	{
		int64 PlayerID = FMath::RandRange(1000000, 888889999);
		Spawn(PlayerID, CharacterID, Tmp, ETeamType::TEAM_BLUE, GetCurrentLv());
	}
}

void FMinionsSystem::SpawnBig()
{

}

void FMinionsSystem::SpawnSuper(ETeamType InType)
{
	auto SpawnSuperByType = [&](const TArray<FVector> &InLocations)
	{
		if (InLocations.IsValidIndex(0))
		{
			int64 PlayerID = FMath::RandRange(1000000, 888889999);
			Spawn(PlayerID, 11112, InLocations[0], InType, GetCurrentLv());
		}
	};

	switch (InType)
	{
	case ETeamType::TEAM_RED:
	{
		SpawnSuperByType(RedLocations);
		break;
	}
	case ETeamType::TEAM_BLUE:
	{
		SpawnSuperByType(BlueLocations);
		break;
	}
	}
}

