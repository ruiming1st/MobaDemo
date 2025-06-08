#include "TurretsSystem.h"
#include "ThreadManage.h"
#include "../../MobyGameState.h"
#include "../../Character/CharacterInstance/MobyGameArchitectureCharacter.h"

FTurret::FTurret()
	:Type(ECharacterType::FIRST_CLASS_TURRETS)
	, SpawnPoint(FVector::ZeroVector)
	, TurretsWeakPtr(NULL)
	, Time(0.f)
{

}

bool FTurret::IsValid() const
{
	return SpawnPoint != FVector::ZeroVector;
}

FCrystal::FCrystal()
	:FTurret()
	, SpawnTime(120.f)
	, MaxSpawnTime(120.f)
{

}

FTurrets::FTurrets()
{

}

FTurretsGroup::FTurretsGroup()
{

}

FTurretsSystem::FTurretsSystem()
	:bInitialize(false)
{

}

void FTurretsSystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bInitialize)
	{
		//检测蓝队
		CheckGroup(Blue, DeltaSeconds, ETeamType::TEAM_BLUE);
		//检测红队
		CheckGroup(Red, DeltaSeconds, ETeamType::TEAM_RED);

		//检测等级
		CheckGroupLv(Blue, DeltaSeconds, ETeamType::TEAM_BLUE);
		CheckGroupLv(Red, DeltaSeconds, ETeamType::TEAM_RED);

		//检测限制
		//红队
		CheckCrystalLimit(Red);
		CheckMainCrystalLimit(Red);

		//蓝队
		CheckCrystalLimit(Blue);
		CheckMainCrystalLimit(Blue);
	}
}

void FTurretsSystem::Init(const TArray<ACharacterSpawnPoint*>& InGameCharacters)
{
	//初始化
	for (auto &Tmp : InGameCharacters)
	{
		if (Tmp->GetType() == ETeamType::TEAM_BLUE)
		{
			if (Tmp->GetCharacterType() == ECharacterType::MAIN_CRYSTAL_TOWER)
			{
				AllocationMainCrystalGroup(Tmp, Blue);
			}
			else if (Tmp->GetCharacterType() == ECharacterType::CRYSTAL_TOWER)
			{
				AllocationCrystalGroup(Tmp, Blue);
			}
			else if (Tmp->GetCharacterType() >= ECharacterType::FIRST_CLASS_TURRETS && //收集塔
				Tmp->GetCharacterType() <= ECharacterType::BASE_TOWER)
			{
				AllocationTurretsGroup(Tmp, Blue);
			}
			else if (Tmp->GetCharacterType() == ECharacterType::EYE_POOL)
			{
				AllocationEyeGroup(Tmp, Blue);
			}
		}
		else if (Tmp->GetType() == ETeamType::TEAM_RED)
		{
			if (Tmp->GetCharacterType() == ECharacterType::MAIN_CRYSTAL_TOWER)
			{
				AllocationMainCrystalGroup(Tmp, Red);
			}
			else if (Tmp->GetCharacterType() == ECharacterType::CRYSTAL_TOWER)
			{
				AllocationCrystalGroup(Tmp, Red);
			}
			else if (Tmp->GetCharacterType() >= ECharacterType::FIRST_CLASS_TURRETS && //收集塔
				Tmp->GetCharacterType() <= ECharacterType::BASE_TOWER)
			{
				AllocationTurretsGroup(Tmp, Red);
			}
			else if (Tmp->GetCharacterType() == ECharacterType::EYE_POOL)
			{
				AllocationEyeGroup(Tmp, Red);
			}
		}
	}

	//延迟1s生成
	GThread::Get()->GetCoroutines().BindLambda(1.0f, [&]()
	{
		SpawnGroup(Red, ETeamType::TEAM_RED);
		SpawnGroup(Blue, ETeamType::TEAM_BLUE);

		//排序
		Sorting(Red.UP.Turrets, Red.MainCrystal.SpawnPoint);
		Sorting(Red.Mid.Turrets, Red.MainCrystal.SpawnPoint);
		Sorting(Red.Down.Turrets, Red.MainCrystal.SpawnPoint);

		Sorting(Blue.UP.Turrets, Blue.MainCrystal.SpawnPoint);
		Sorting(Blue.Mid.Turrets, Blue.MainCrystal.SpawnPoint);
		Sorting(Blue.Down.Turrets, Blue.MainCrystal.SpawnPoint);

		//开启最前面的塔显示
//////////////////////////////////////////////////////////////////////////
		GThread::Get()->GetCoroutines().BindLambda(1.f, [&]()
		{
			//Red
			//先设置我们的水晶
			SetPossibleAttack(&Red.MainCrystal, false);
			SetPossibleAttack(&Red.UP.Crystal, false);
			SetPossibleAttack(&Red.Mid.Crystal, false);
			SetPossibleAttack(&Red.Down.Crystal, false);
			//再设置我们的普通塔
			SetTurretPossibleAttack(&Red.UP, true);
			SetTurretPossibleAttack(&Red.Mid, true);
			SetTurretPossibleAttack(&Red.Down, true);

			//Blue
			//先设置我们的水晶
			SetPossibleAttack(&Blue.MainCrystal, false);
			SetPossibleAttack(&Blue.UP.Crystal, false);
			SetPossibleAttack(&Blue.Mid.Crystal, false);
			SetPossibleAttack(&Blue.Down.Crystal, false);
			//再设置我们的普通塔
			SetTurretPossibleAttack(&Blue.UP, true);
			SetTurretPossibleAttack(&Blue.Mid, true);
			SetTurretPossibleAttack(&Blue.Down, true);

			bInitialize = true;
		});
	});
}

void FTurretsSystem::AllocationMainCrystalGroup(ACharacterSpawnPoint* InPoint, FTurretsGroup& TG)
{
	Allocation(InPoint, &TG.MainCrystal);
}

void FTurretsSystem::AllocationCrystalGroup(ACharacterSpawnPoint* InPoint, FTurretsGroup& TG)
{
	if (InPoint->GetRoadType() == ERoadType::ROAD_UP)
	{
		Allocation(InPoint, &TG.UP.Crystal);
	}
	else if (InPoint->GetRoadType() == ERoadType::ROAD_MID)
	{
		Allocation(InPoint, &TG.Mid.Crystal);
	}
	else if (InPoint->GetRoadType() == ERoadType::ROAD_DOWN)
	{
		Allocation(InPoint, &TG.Down.Crystal);
	}
}

void FTurretsSystem::AllocationTurretsGroup(ACharacterSpawnPoint* InPoint, FTurretsGroup& TG)
{
	if (InPoint->GetRoadType() == ERoadType::ROAD_UP)
	{
		TG.UP.Turrets.Add(FTurret());
		FTurret &T = TG.UP.Turrets.Last();
		Allocation(InPoint, &T);
	}
	else if (InPoint->GetRoadType() == ERoadType::ROAD_MID)
	{
		TG.Mid.Turrets.Add(FTurret());
		FTurret& T = TG.Mid.Turrets.Last();
		Allocation(InPoint, &T);
	}
	else if (InPoint->GetRoadType() == ERoadType::ROAD_DOWN)
	{
		TG.Down.Turrets.Add(FTurret());
		FTurret& T = TG.Down.Turrets.Last();
		Allocation(InPoint, &T);
	}
}

void FTurretsSystem::AllocationEyeGroup(ACharacterSpawnPoint* InPoint, FTurretsGroup& TG)
{
	Allocation(InPoint, &TG.Eye);
}

AMobyGameArchitectureCharacter* FTurretsSystem::SpawnTurret(FTurret* InTurret, const ETeamType InType, int32 InCharacterID)
{
	if (InTurret)
	{
		int64 InPlayerID = FMath::RandRange(10000, 999999999);
		InTurret->TurretsWeakPtr = Cast<AMobyGameArchitectureCharacter>(Spawn(InPlayerID, InCharacterID, InTurret->SpawnPoint, InType));

		return InTurret->TurretsWeakPtr.Get();
	}

	return NULL;
}

void FTurretsSystem::SpawnGroup(FTurretsGroup& InGroup, const ETeamType InType)
{
	//是否有条件生成主水晶
	if (InGroup.MainCrystal.IsValid())
	{
		SpawnTurret(&InGroup.MainCrystal, InType, 22222);
	}

	//眼
	if (InGroup.Eye.IsValid())
	{
		SpawnTurret(&InGroup.Eye, InType, 22223);
	}

	//上路
	SpawnTurrets(InGroup.UP, InType);

	//中路
	SpawnTurrets(InGroup.Mid, InType);

	//下路
	SpawnTurrets(InGroup.Down, InType);
}

void FTurretsSystem::SpawnTurrets(FTurrets& InGroup, const ETeamType InType)
{
	//上路高地水晶
	if (InGroup.Crystal.IsValid())
	{
		SpawnTurret(&InGroup.Crystal, InType, 22221);
	}

	//生成塔
	for (auto& Tmp : InGroup.Turrets)
	{
		if (Tmp.IsValid())
		{
			SpawnTurret(&Tmp, InType, 22220);
		}
	}
}

void FTurretsSystem::CheckRecovery(FTurret* InTurret, float DeltaSeconds)
{
	if (InTurret)
	{
		if (InTurret->TurretsWeakPtr.IsValid())
		{
			if (!InTurret->TurretsWeakPtr->IsDie())
			{
				if (FCharacterAttribute *InAttribute = InTurret->TurretsWeakPtr->GetCharacterAttribute())
				{
					if (InAttribute->Health < InAttribute->MaxHealth)
					{
						InTurret->Time += DeltaSeconds;
						if (InTurret->Time >= 4.f)
						{
							InTurret->Time = 0.f;
							InAttribute->Health += 6.f;
							if (InAttribute->Health > InAttribute->MaxHealth)
							{
								InAttribute->Health = InAttribute->MaxHealth;
							}
						}
					}
				}
			}
		}
	}
}

bool FTurretsSystem::CheckDeath(FTurrets* InTurret, float DeltaSeconds, int32 InCharacterID, const ETeamType InType)
{
	if (InTurret)
	{
		if (!InTurret->Crystal.TurretsWeakPtr.IsValid() || InTurret->Crystal.TurretsWeakPtr->IsDie())
		{
			InTurret->Crystal.SpawnTime -= DeltaSeconds;
			if (InTurret->Crystal.SpawnTime <= 0.f)
			{
				InTurret->Crystal.SpawnTime = InTurret->Crystal.MaxSpawnTime;
				SpawnTurret(&InTurret->Crystal, InType, InCharacterID);
			}

			return true;
		}
	}

	return false;
}

bool FTurretsSystem::CheckMainCrystalDeath(FCrystal* InTurret)
{
	if (InTurret)
	{
		if (!InTurret->TurretsWeakPtr.IsValid() || InTurret->TurretsWeakPtr->IsDie())
		{
			return true;
		}
	}

	return false;
}

void FTurretsSystem::Allocation(ACharacterSpawnPoint* InPoint, FTurret* InSlot)
{
	if (InPoint && InSlot)
	{
		InSlot->Type = InPoint->GetCharacterType();
		InSlot->SpawnPoint = InPoint->GetActorLocation();
	}
}

void FTurretsSystem::CheckGroup(FTurretsGroup& InGroup, float DeltaSeconds, const ETeamType InType)
{
	if (AMobyGameState* InGameState = GetWorld()->GetGameState<AMobyGameState>())
	{
		//检测我们的水晶是否死亡
		auto CheckCrystalDeath = [&](FTurrets* InTurret)
		{
			if (InTurret)
			{
				if (InTurret->Crystal.IsValid())
				{
					if (!InTurret->Crystal.TurretsWeakPtr.IsValid() || InTurret->Crystal.TurretsWeakPtr->IsDie())
					{
						InGameState->SetSpawnSuper(InType, true);
					}
					else
					{
						InGameState->SetSpawnSuper(InType, false);
					}
				}
			}
		};

		if (!InGameState->IsGameOver())
		{
			//主水晶
			if (!CheckMainCrystalDeath(&InGroup.MainCrystal))
			{
				//主水晶恢复
				CheckRecovery(&InGroup.MainCrystal, DeltaSeconds);

				//检查水晶
				CheckRecovery(&InGroup.UP.Crystal, DeltaSeconds);
				CheckDeath(&InGroup.UP, DeltaSeconds, 22221, InType);
				CheckCrystalDeath(&InGroup.UP);

				CheckRecovery(&InGroup.Mid.Crystal, DeltaSeconds);
				CheckDeath(&InGroup.Mid, DeltaSeconds, 22221, InType);
				CheckCrystalDeath(&InGroup.UP);

				CheckRecovery(&InGroup.Down.Crystal, DeltaSeconds);
				CheckDeath(&InGroup.Down, DeltaSeconds, 22221, InType);
				CheckCrystalDeath(&InGroup.UP);
			}
			else
			{
				if (InType == ETeamType::TEAM_BLUE)
				{
					InGameState->GameOver(ETeamType::TEAM_RED);
				}
				else if (InType == ETeamType::TEAM_RED)
				{
					InGameState->GameOver(ETeamType::TEAM_BLUE);
				}
			}
		}
	}
}

//随着时间进行升级
void FTurretsSystem::CheckGroupLv(FTurretsGroup& InGroup, float DeltaSeconds, const ETeamType InType)
{
	auto CheckLv = [&](FTurret *InTurret)
	{
		if (InTurret)
		{
			if (InTurret->IsValid())
			{
				if (InTurret->TurretsWeakPtr.IsValid())
				{
					if (!InTurret->TurretsWeakPtr->IsDie())
					{
						if (auto InAttri = InTurret->TurretsWeakPtr->GetCharacterAttribute())
						{
							if (InAttri->Lv < GetCurrentLv())
							{
								int32 InNewLvlerp = GetCurrentLv() - InAttri->Lv;
								for (int i = 0; i < InNewLvlerp; i++)
								{
									InAttri->UpdateLevel();
								}
							}
						}
					}
				}
			}
		}
	};

	auto CheckLvs = [&](FTurrets* InTurrets)
	{
		for (auto &Tmp : InTurrets->Turrets)
		{
			CheckLv(&Tmp);
		}
	};

	//主水晶
	CheckLv(&InGroup.MainCrystal);

	//水晶
	CheckLv(&InGroup.UP.Crystal);
	CheckLv(&InGroup.Mid.Crystal);
	CheckLv(&InGroup.Down.Crystal);

	//塔
	CheckLvs(&InGroup.UP);
	CheckLvs(&InGroup.Mid);
	CheckLvs(&InGroup.Down);
}

//塔的层级限制
void FTurretsSystem::RemoveTurret(TWeakObjectPtr<AMobyGameArchitectureCharacter> InTurretsWeakPtr, const ETeamType InType)
{
	auto CheckOwnerTurret = [&](FTurrets& InTurret)->bool
	{
		bool bRemove = false;
		TArray<FTurret> RemoveTurret;
		for (auto &Tmp : InTurret.Turrets)
		{
			if (Tmp.TurretsWeakPtr == InTurretsWeakPtr || !Tmp.TurretsWeakPtr.IsValid())
			{
				RemoveTurret.Add(Tmp);
			}
		}

		for (auto &Tmp : RemoveTurret)
		{
			InTurret.Turrets.Remove(Tmp);
			bRemove = true;
		}

		return bRemove;
	};

	auto FuncPossibleAttack = [&](FTurrets& InTurret)
	{
		if (CheckOwnerTurret(InTurret))
		{
			//死亡的塔已被移除, 需要设置一个组里的最远的塔可被攻击
			SetTurretPossibleAttack(&InTurret, true);
		}
	};

	auto RemoveTurretInDiffTeam = [&](FTurretsGroup& InGroup)
	{
		FuncPossibleAttack(InGroup.UP);
		FuncPossibleAttack(InGroup.Mid);
		FuncPossibleAttack(InGroup.Down);
	};

	if (InType == ETeamType::TEAM_BLUE)
	{
		RemoveTurretInDiffTeam(Blue);
	}
	else if (InType == ETeamType::TEAM_RED)
	{
		RemoveTurretInDiffTeam(Red);
	}
}

void FTurretsSystem::SetPossibleAttack(FTurret* InTurret, bool bPossibleAttack)
{
	if (InTurret)
	{
		if (InTurret->TurretsWeakPtr.IsValid())
		{
			InTurret->TurretsWeakPtr->SetPossibleAttack(bPossibleAttack);
		}
	}
}

void FTurretsSystem::SetTurretPossibleAttack(FTurrets* InTurret, bool bPossibleAttack)
{
	if (InTurret)
	{
		if (InTurret->Turrets.IsValidIndex(0))
		{
			SetPossibleAttack(&InTurret->Turrets[0], bPossibleAttack);
		}
	}
}

void FTurretsSystem::CheckCrystalLimit(FTurretsGroup& InGroup)
{
	auto SetPossibleAttackRoadCrystal = [&](FTurrets &InTurrets)
	{
		if (CheckTurretDeath(&InTurrets))
		{
			SetPossibleAttackCrystal(&InTurrets.Crystal, false, true);
		}
	};

	SetPossibleAttackRoadCrystal(InGroup.UP);
	SetPossibleAttackRoadCrystal(InGroup.Mid);
	SetPossibleAttackRoadCrystal(InGroup.Down);
}

void FTurretsSystem::CheckMainCrystalLimit(FTurretsGroup& InGroup)
{
	if (CheckGroupDeath(InGroup))
	{
		SetPossibleAttackCrystal(&InGroup.MainCrystal, false, true);
	}
	else
	{
		SetPossibleAttackCrystal(&InGroup.MainCrystal, true, false);
	}
}

bool FTurretsSystem::CheckTurretDeath(FTurrets* InTurret) //只检测塔 用于判断可否攻击水晶
{
	bool bAllDeath = true;
	if (InTurret)
	{
		for (auto& Tmp : InTurret->Turrets)
		{
			if (Tmp.TurretsWeakPtr.IsValid())
			{
				if (!Tmp.TurretsWeakPtr->IsDie())
				{
					bAllDeath = false;
				}
			}
		}
	}

	return bAllDeath;
}

//同时判断塔和水晶
ETurretDeathType FTurretsSystem::CheckTurretsDeath(FTurrets* InTurret)
{
	if (!InTurret)
	{
		return ETurretDeathType::TURRET_ERROR;
	}

	if (CheckTurretDeath(InTurret))
	{
		if (InTurret->Crystal.IsValid()) //是否有意义
		{
			if (InTurret->Crystal.TurretsWeakPtr.IsValid())
			{
				if (InTurret->Crystal.TurretsWeakPtr->IsDie())
				{
					return ETurretDeathType::TURRET_DIE;
				}
			}
			else
			{
				return ETurretDeathType::TURRET_DIE;
			}
		}
		else
		{
			return ETurretDeathType::TURRET_NO_SET;
		}
	}

	return ETurretDeathType::TURRET_NO_DIE;
}

void FTurretsSystem::SetPossibleAttackCrystal(FTurret* InTurret, bool bComparisonValue, bool bValue)
{
	if (InTurret && InTurret->TurretsWeakPtr.IsValid())
	{
		if (!InTurret->TurretsWeakPtr->IsDie())
		{
			if (InTurret->TurretsWeakPtr->IsPossibleAttack() == bComparisonValue)
			{
				InTurret->TurretsWeakPtr->SetPossibleAttack(bValue);
			}
		}
	}
}

bool FTurretsSystem::CheckGroupDeath(FTurretsGroup& InGroup)
{
	ETurretDeathType UP = CheckTurretsDeath(&InGroup.UP);
	ETurretDeathType Mid = CheckTurretsDeath(&InGroup.Mid);
	ETurretDeathType Down = CheckTurretsDeath(&InGroup.Down);

	if (UP == ETurretDeathType::TURRET_DIE ||
		Mid == ETurretDeathType::TURRET_DIE ||
		Down == ETurretDeathType::TURRET_DIE)
	{
		return true;
	}

	return false;
}

void FTurretsSystem::Sorting(TArray<FTurret>& Turrets, const FVector& InLocation)
{
	if (InLocation != FVector::ZeroVector)
	{
		TArray<FTurret> InTurrets = Turrets;
		Turrets.Empty();

		RecursiveSorting(InLocation, InTurrets, Turrets);
	}
}

void FTurretsSystem::RecursiveSorting(const FVector& InLocation, TArray<FTurret>& InTurrets, TArray<FTurret>& OutTurrets)
{
	float DistanceTmp = 0.f;
	FTurret* InTurretPtr = NULL;
	for (auto& Tmp : InTurrets)
	{
		float Distance = FVector::Dist(InLocation, Tmp.SpawnPoint);
		if (Distance > DistanceTmp)
		{
			DistanceTmp = Distance;
			InTurretPtr = &Tmp;
		}
	}

	if (InTurretPtr)
	{
		OutTurrets.Add(*InTurretPtr);
		FTurret& InNewTurret = OutTurrets.Last();
		InTurrets.Remove(InNewTurret);
		if (InTurrets.Num())
		{
			RecursiveSorting(InLocation, InTurrets, OutTurrets);
		}
	}
}
