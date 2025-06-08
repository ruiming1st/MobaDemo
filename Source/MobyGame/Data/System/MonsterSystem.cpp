#include "MonsterSystem.h"
#include "../../Character/CharacterSpawnPoint.h"
#include "../../Common/MethodUnit.h"
#include "ThreadManage.h"

FMonsterSystem::FMonsterSystem()
{

}

FMonster::FMonster()
	:SpawnPoint(FVector::ZeroVector)
{

}

FMonsterGroup::FMonsterGroup()
	: Type(ECharacterType::WILD_MONSTER)
	, bDieGroup(false)
	, SpawnTime(60.f)
	, MaxSpawnTime(60.f)
{

}

void FMonsterSystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//生命周期管理
	for (auto& Tmp : Group)
	{
		if (!Tmp.bDieGroup)
		{
			bool bDieGroup = true;
			for (auto& TmpMonster : Tmp.Monster)
			{
				if (TmpMonster.MonsterWeakPtr.IsValid())
				{
					if (!TmpMonster.MonsterWeakPtr->IsDie())
					{
						bDieGroup = false;
						break;
					}
				}
			}

			if (bDieGroup)
			{
				Tmp.bDieGroup = true;
				Tmp.SpawnTime = Tmp.MaxSpawnTime;
			}
		}
	}

	//生成
	for (auto& Tmp : Group)
	{
		if (Tmp.bDieGroup)
		{
			Tmp.SpawnTime -= DeltaSeconds;
			if (Tmp.SpawnTime <= 0.f)
			{
				SpawnMonster(Tmp);
				Tmp.bDieGroup = false;
			}
		}
	}
}

void FMonsterSystem::AllocationGroup(ACharacterSpawnPoint* InPoint)
{
	auto AddMonster = [&](FMonsterGroup &InNewGroup)
	{
		InNewGroup.Monster.Add(FMonster());
		FMonster& Ms = InNewGroup.Monster.Last();
		Ms.SpawnPoint = InPoint->GetActorLocation();
	};

	bool bInGroup = false;
	for (auto &Tmp : Group)
	{
		bool bGroup = false;
		for (auto & MonsterTmp : Tmp.Monster)
		{
			float Distance = FVector::Dist(MonsterTmp.SpawnPoint, InPoint->GetActorLocation());
			if (Distance <= 1000.f)
			{
				bGroup = true;
				break;
			}
		}

		if (bGroup)
		{
			AddMonster(Tmp);
			bInGroup = true;
		}
	}

	if (!bInGroup)
	{
		Group.Add(FMonsterGroup());
		FMonsterGroup& Ms = Group.Last();

		//时间初始化
		Ms.SpawnTime = InPoint->GetSpawnTime();
		Ms.MaxSpawnTime = InPoint->GetSpawnTime();

		AddMonster(Ms);
	}
}

void FMonsterSystem::Init(const TArray<ACharacterSpawnPoint*>& InGameCharacters)
{
	for (auto &Tmp : InGameCharacters)
	{
		if (Tmp->GetCharacterType() >= ECharacterType::WILD_MONSTER &&
			Tmp->GetCharacterType() <= ECharacterType::WILD_BOSS_MONSTER)
		{
			AllocationGroup(Tmp);
		}
	}

	//GThread::Get()->GetCoroutines().BindLambda(60.f, [&]()
	//{
	//	//生成
	//	for (auto& Tmp : Group)
	//	{
	//		SpawnMonster(Tmp);
	//	}
	//});	
}

//
void FMonsterSystem::SpawnMonster(FMonsterGroup& Instance)
{
	for (auto &Tmp : Instance.Monster)
	{
		int64 PlayerID = FMath::RandRange(1000000, 888889999);
		switch (Instance.Type)
		{
		case ECharacterType::WILD_MONSTER:
			Tmp.MonsterWeakPtr = Spawn(PlayerID, 33330, Tmp.SpawnPoint, ETeamType::TEAM_NEUTRAL, GetCurrentLv());
			break;
		case ECharacterType::WILD_BOSS_MONSTER:
			Tmp.MonsterWeakPtr = Spawn(PlayerID, 33330, Tmp.SpawnPoint, ETeamType::TEAM_NEUTRAL, GetCurrentLv());
			break;
		}
	}
}