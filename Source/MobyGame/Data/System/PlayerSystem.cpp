#include "PlayerSystem.h"
#include "../../Character/CharacterSpawnPoint.h"
#include "../../Common/MethodUnit.h"
#include "../../Character/CharacterInstance/MobyGamePlayerCharacter.h"

FPlayerCharacterGroup::FPlayerCharacterGroup()
	:ChangeTimeType(EChangeTypeTime::STAGE_I)
{

}

FPlayerSystem::FPlayerSystem()
{

}

void FPlayerSystem::Tick(float DeltaSeconds)
{
	//检测我们的组
	CheckGroup(Blue, DeltaSeconds);
	CheckGroup(Red, DeltaSeconds);
}

AMobyGameCharacter* FPlayerSystem::SpawnCharacterInGroup(FPlayerCharacterGroup& InGroup, int64 InPlayerID, int32 InCharacterID, ETeamType InTeam)
{
	for (auto& Tmp : InGroup.Characters)
	{
		if (Tmp.Character == NULL)
		{
			Tmp.Character = Spawn(InPlayerID, InCharacterID, Tmp.SpawnPoint, InTeam, Tmp.PlayerName);

			//代理绑定
			if (AMobyGamePlayerCharacter *InPlayerCharacter = Cast<AMobyGamePlayerCharacter>(Tmp.Character))
			{
				InPlayerCharacter->DieDelegate.BindRaw(&Tmp, &FPlayerCharacter::ResetDeathTime);
			}

			return Tmp.Character.Get();
		}
	}

	return NULL;
}

AMobyGameCharacter* FPlayerSystem::Spawn(int64 InPlayerID, int32 InCharacterID, const FVector& InLocation, ETeamType Type, FName& InName, int32 InCurrentLv /*= 1*/)
{
	AMobyGameCharacter* MobyGameCharacter = NULL;
	if (AMobyGameState* InGameState = MethodUnit::GetGameState(GetWorld()))
	{
		const FCharacterTable* InTable = InGameState->GetCharacterTableTemplate(InCharacterID);
		if (InTable)
		{
			if (InTable->CharacterClass)
			{
				MobyGameCharacter = GetWorld()->SpawnActor<AMobyGameCharacter>(InTable->CharacterClass, InLocation, FRotator::ZeroRotator);
				if (MobyGameCharacter)
				{
					MobyGameCharacter->RegisterCharacter(InPlayerID, InCharacterID, InName);
					MobyGameCharacter->SetTeam(Type);
					MobyGameCharacter->SetCharacterType(InTable->CharacterType);
					MobyGameCharacter->MultiCastRegisterCharacter(InPlayerID);

					if (FCharacterAttribute* InCharacterAttribute = InGameState->GetCharacterAttribute(InPlayerID))
					{
						for (int32 i = 1; i < InCurrentLv; i++)
						{
							InCharacterAttribute->UpdateLevel();
						}
					}
				}
			}
		}
	}

	return MobyGameCharacter;
}

void FPlayerSystem::CheckGroup(FPlayerCharacterGroup& InGroup, float DeltaSeconds)
{
	//游戏时间是不是到了我们的限制阶段
	if (GetWorld()->GetRealTimeSeconds() >= 60.f * 0.3f)
	{
		if (InGroup.ChangeTimeType == EChangeTypeTime::STAGE_I)
		{
			ChangeTime(InGroup, 5.f);
			InGroup.ChangeTimeType = EChangeTypeTime::STAGE_II;
		}
	}
	else if (GetWorld()->GetRealTimeSeconds() >= 60.f * 18.f)
	{
		if (InGroup.ChangeTimeType == EChangeTypeTime::STAGE_II)
		{
			ChangeTime(InGroup, 10.f);
			InGroup.ChangeTimeType = EChangeTypeTime::STAGE_III;
		}
	}
	else if (GetWorld()->GetRealTimeSeconds() >= 60.f * 30.f)
	{
		if (InGroup.ChangeTimeType == EChangeTypeTime::STAGE_III)
		{
			ChangeTime(InGroup, 15.f);
			InGroup.ChangeTimeType = EChangeTypeTime::STAGE_IV;
		}
	}
	else if (GetWorld()->GetRealTimeSeconds() >= 60.f * 40.f)
	{
		if (InGroup.ChangeTimeType == EChangeTypeTime::STAGE_IV)
		{
			ChangeTime(InGroup, 20.f);
			InGroup.ChangeTimeType = EChangeTypeTime::STAGE_V;
		}
	}

	//死亡后开始计时 
	for (auto& Tmp : InGroup.Characters)
	{
		if (Tmp.Character != NULL)
		{
			if (Tmp.Character->IsDie())
			{
				Tmp.Time -= DeltaSeconds;
				if (Tmp.Time <= 0.f)
				{
					//玩家重生
					Tmp.Character->MultiCastReborn();
					Tmp.ResetDeathTime();
				}
			}
		}
	}
}

void FPlayerSystem::ChangeTime(FPlayerCharacterGroup& InGroup, float InNewTime)
{
	for (auto &Tmp : InGroup.Characters)
	{
		Tmp.MaxTime += (Tmp.MaxTime + InNewTime);
	}
}

float FPlayerSystem::GetRebornTime(const AMobyGameCharacter* InCharacter)
{
	auto GetTime = [&](FPlayerCharacterGroup InGroup)->float
	{
		for (auto& Tmp : InGroup.Characters)
		{
			if (Tmp.Character == InCharacter)
			{
				return Tmp.Time;
			}
		}

		return INDEX_NONE;
	};

	float NewTime = GetTime(Red);
	if (NewTime == INDEX_NONE)
	{
		NewTime = GetTime(Blue);
	}

	return NewTime;
}

AMobyGameCharacter* FPlayerSystem::SpawnCharacter(int64 InPlayerID, int32 InCharacterID, ETeamType Type)
{
	if (Type == ETeamType::TEAM_BLUE)
	{
		return SpawnCharacterInGroup(Blue, InPlayerID, InCharacterID, Type);
	}
	else if (Type == ETeamType::TEAM_RED)
	{
		return SpawnCharacterInGroup(Red, InPlayerID, InCharacterID, Type);
	}

	return NULL;
}

//采集位置
void FPlayerSystem::Init(const TArray<ACharacterSpawnPoint*>& InGameCharacters)
{
	auto InitPlayerPoint = [](FPlayerCharacterGroup& InGroup, const FVector& NewLocation, FName PlayerName)
	{
		InGroup.Characters.Add(FPlayerCharacter());
		FPlayerCharacter& InCharacter = InGroup.Characters.Last();
		InCharacter.SpawnPoint = NewLocation;
		InCharacter.PlayerName = PlayerName;
	};

	for (auto& Tmp : InGameCharacters)
	{
		if (Tmp->GetCharacterType() == ECharacterType::PLAYER)
		{
			if (Tmp->GetType() == ETeamType::TEAM_RED)
			{
				InitPlayerPoint(Red, Tmp->GetActorLocation(), Tmp->GetPlayerName());
			}
			else if (Tmp->GetType() == ETeamType::TEAM_BLUE)
			{
				InitPlayerPoint(Blue, Tmp->GetActorLocation(), Tmp->GetPlayerName());
			}
		}
	}
}

FPlayerCharacter::FPlayerCharacter()
	:SpawnPoint(FVector::ZeroVector)
	, Character(NULL)
	, Time(11.f)
	, MaxTime(11.f)
{
	
}

//随着游戏进行，游戏角色死亡重生时间变长，惩罚变大
void FPlayerCharacter::ResetDeathTime()
{
	Time = MaxTime;
}