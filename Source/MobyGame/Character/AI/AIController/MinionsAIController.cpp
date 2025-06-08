// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionsAIController.h"
#include "Kismet/GameplayStatics.h"
#include "../../CharacterInstance/MobyGameCharacter.h"

#define MAX_TMP 99999999
//入侵者信息
struct FAITargetRef
{
	FAITargetRef() : InTarget(NULL), RefCount(0), Distance(0.f)
	{}

	AMobyGameCharacter* InTarget;
	int32 RefCount;
	float Distance;
};

struct FAITargetRefs
{
	friend class AMinionsAIController;
	FAITargetRefs()
	{}

	void InitRef(const AMobyGameCharacter* InTarget);

	AMobyGameCharacter* GetTarget();
protected:
	int32 GetRefCountBase();
	bool InitTargetRef(TArray<FAITargetRef>& TargetRefs, const AMobyGameCharacter* InTarget);
	int32 GetBaseLineSingle(const TArray<FAITargetRef>& TargetRefs);
	void GetRefCountBaseRefTargets(int32 BaseLine, const TArray<FAITargetRef>& InRefs, TArray<const FAITargetRef*>& InTargetRefs);
	AMobyGameCharacter* GetNearestTargets(TArray<const FAITargetRef*>& TargetRefs);
private:
	TArray<FAITargetRef> Player;
	TArray<FAITargetRef> Tower;
	TArray<FAITargetRef> Minion;
	TArray<FAITargetRef> Monster;
};


AMobyGameCharacter* FAITargetRefs::GetTarget()
{
	AMobyGameCharacter* Target = NULL;

	//First step: Search target which has least refcount.
	//所有组别的敌人的最少的引用计数，利用此计数找出不同组别满足最小计数的敌人，后续第二个步骤按优先级查找
	const int32 CountBase = GetRefCountBase();
	if (CountBase != MAX_TMP)
	{
		//Second step.
		TArray<const FAITargetRef*> InMinion;
		GetRefCountBaseRefTargets(CountBase, Minion, InMinion);
		Target = GetNearestTargets(InMinion);
		if (Target)
		{
			return Target;
		}

		TArray<const FAITargetRef*> InMonster;
		GetRefCountBaseRefTargets(CountBase, Monster, InMonster);
		Target = GetNearestTargets(InMonster);
		if (Target)
		{
			return Target;
		}

		TArray<const FAITargetRef*> InTower;
		GetRefCountBaseRefTargets(CountBase, Tower, InTower);
		Target = GetNearestTargets(InTower);
		if (Target)
		{
			return Target;
		}

		TArray<const FAITargetRef*> InPlayer;
		GetRefCountBaseRefTargets(CountBase, Player, InPlayer);
		Target = GetNearestTargets(InPlayer);
		if (Target)
		{
			return Target;
		}
	}

	return Target;
}

int32 FAITargetRefs::GetRefCountBase()
{
	TArray<int32> BaseLine;
	BaseLine.Add(GetBaseLineSingle(Minion));
	BaseLine.Add(GetBaseLineSingle(Monster));
	BaseLine.Add(GetBaseLineSingle(Tower));
	BaseLine.Add(GetBaseLineSingle(Player));

	int32 CountBase = MAX_TMP;
	for (auto &Tmp : BaseLine)
	{
		if (CountBase > Tmp)
		{
			CountBase = Tmp;
		}
	}

	return CountBase;
}

void FAITargetRefs::InitRef(const AMobyGameCharacter* InTarget)
{
	if (InitTargetRef(Minion, InTarget)) {}
	else if (InitTargetRef(Tower, InTarget)) {}
	else if (InitTargetRef(Monster, InTarget)) {}
	else if (InitTargetRef(Player, InTarget)) {}
}

//引用计数
bool FAITargetRefs::InitTargetRef(TArray<FAITargetRef>& TargetRefs, const AMobyGameCharacter* InTarget)
{
	for (auto &Tmp : TargetRefs)
	{
		if (Tmp.InTarget == InTarget)
		{
			Tmp.RefCount++;
			return true;
		}
	}

	return false;
}

//返回最小的引用计数
int32 FAITargetRefs::GetBaseLineSingle(const TArray<FAITargetRef>& TargetRefs)
{
	int32 CountBase = MAX_TMP;
	for (auto &Tmp : TargetRefs)
	{
		if (CountBase > Tmp.RefCount)
		{
			CountBase = Tmp.RefCount;
		}
	}

	return CountBase;
}

void FAITargetRefs::GetRefCountBaseRefTargets(int32 BaseLine, const TArray<FAITargetRef>& InRefs, TArray<const FAITargetRef*>& InTargetRefs)
{
	for (auto& Tmp : InRefs)
	{
		if (Tmp.RefCount == BaseLine)
		{
			InTargetRefs.Add(&Tmp);
		}
	}
}

AMobyGameCharacter* FAITargetRefs::GetNearestTargets(TArray<const FAITargetRef*>& TargetRefs)
{
	AMobyGameCharacter* NearestTarget = NULL;
	float Distance = MAX_TMP;
	for (auto& Tmp : TargetRefs)
	{
		if (Tmp)
		{
			if (Tmp->Distance < Distance)
			{
				Distance = Tmp->Distance;
				NearestTarget = Tmp->InTarget;
			}
		}
	}

	return NearestTarget;
}

AMinionsAIController::AMinionsAIController()
{
	MissionTargetPtr = NULL;
}

void AMinionsAIController::InitController()
{
	Super::InitController();
}

void AMinionsAIController::BeginPlay()
{
	Super::BeginPlay();//过0.1s后执行虚函数初始化Controller，运行AI行为树，此时下面的BTAsset已经获取所以没有问题

	BTAsset = AIBTAsset;
}

bool AMinionsAIController::CheckRange()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMobyGameCharacter::StaticClass(), OutActors);

	if (AMobyGameCharacter* InOwnerCharacter = Cast<AMobyGameCharacter>(GetPawn()))
	{
		for (auto &Tmp : OutActors)
		{
			if (Tmp != InOwnerCharacter)
			{
				if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(Tmp))
				{
					if (!InCharacter->IsDie())
					{
						//队伍不同
						if (InOwnerCharacter->GetTeam() != InCharacter->GetTeam())
						{
							float Distance = FVector::Dist(Tmp->GetActorLocation(), InOwnerCharacter->GetActorLocation());
							if (Distance <= 2000.f)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

AMobyGameCharacter* AMinionsAIController::FindTarget()
{
	AMobyGameCharacter* InTargetCharacter = NULL;
	//根据优先级寻找
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMobyGameCharacter::StaticClass(), OutActors);

	//收集我们的任务目标
	FindMissonTarget(OutActors);

	FAITargetRefs Targets;

	if (AMobyGameCharacter* InOwnerCharacter = Cast<AMobyGameCharacter>(GetPawn()))
	{
		//I.收集
		for (auto &Tmp : OutActors)
		{
			if (Tmp != InOwnerCharacter)
			{
				if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(Tmp))
				{
					if (!InCharacter->IsDie())
					{
						//队伍不同
						if (InOwnerCharacter->GetTeam() != InCharacter->GetTeam())
						{
							float Distance = FVector::Dist(Tmp->GetActorLocation(), InOwnerCharacter->GetActorLocation());
							if (Distance <= 2000.f)
							{
								if (InCharacter->GetCharacterType() >= ECharacterType::REMOTE_MINION	&& InCharacter->GetCharacterType() <= ECharacterType::SUPER_MINION)
								{
									Targets.Minion.Add(FAITargetRef());
									Targets.Minion.Last().Distance = Distance;
									Targets.Minion.Last().InTarget = InCharacter;
								}
								else if (InCharacter->GetCharacterType() >= ECharacterType::FIRST_CLASS_TURRETS && InCharacter->GetCharacterType() <= ECharacterType::BASE_TOWER)
								{
									Targets.Tower.Add(FAITargetRef());
									Targets.Tower.Last().Distance = Distance;
									Targets.Tower.Last().InTarget = InCharacter;

								}
								else if (InCharacter->GetCharacterType() == ECharacterType::PLAYER)
								{
									Targets.Player.Add(FAITargetRef());
									Targets.Player.Last().Distance = Distance;
									Targets.Player.Last().InTarget = InCharacter;
								}
								else if (InCharacter->GetCharacterType() >= ECharacterType::WILD_MONSTER && InCharacter->GetCharacterType() <= ECharacterType::WILD_BOSS_MONSTER)//判断野怪
								{
									Targets.Monster.Add(FAITargetRef());
									Targets.Monster.Last().Distance = Distance;
									Targets.Monster.Last().InTarget = InCharacter;
								}

							}
						}
					}
				}
			}
		
		}

		//II.查询同伴目标数
		for (auto &Tmp : OutActors)
		{
			if (Tmp != InOwnerCharacter)
			{
				if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(Tmp))
				{
					if (!InCharacter->IsDie())
					{
						//队伍相同的队友
						if (InOwnerCharacter->GetTeam() == InCharacter->GetTeam())
						{
							if (InCharacter->GetCharacterType() >= ECharacterType::REMOTE_MINION	&& InCharacter->GetCharacterType() <= ECharacterType::SUPER_MINION)
							{
								if (AMobyGameAIController* MinionsAIController = Cast<AMobyGameAIController>(InCharacter->GetController()))
								{
									Targets.InitRef(MinionsAIController->GetTarget());									
								}
							}
						}
					}
				}
			}
		}
	}

	InTargetCharacter = Targets.GetTarget();

	if (!InTargetCharacter)
	{
		return GetMissionTarget();
	}

	return InTargetCharacter;
}

AMobyGameCharacter* AMinionsAIController::GetMissionTarget()
{
	return MissionTargetPtr.Get();
}

void AMinionsAIController::ResetMissionTargetPtr()
{
	MissionTargetPtr = NULL;
}

void AMinionsAIController::FindMissonTarget(TArray<AActor*>& OutActors)
{
	if (!MissionTargetPtr.IsValid() || MissionTargetPtr->IsDie())
	{
		if (AMobyGameCharacter* InOwnerCharacter = Cast<AMobyGameCharacter>(GetPawn()))
		{
			float TmpDistance = 999999.f;
			for (auto &Tmp : OutActors)
			{
				if (Tmp != InOwnerCharacter)
				{
					if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(Tmp))
					{

						if (!InCharacter->IsDie())
						{
							if (InCharacter->GetCharacterType() >= ECharacterType::FIRST_CLASS_TURRETS && InCharacter->GetCharacterType() <= ECharacterType::MAIN_CRYSTAL_TOWER)
							{
								//队伍不同
								if (InOwnerCharacter->GetTeam() != InCharacter->GetTeam())
								{
									float Distance = FVector::Dist(Tmp->GetActorLocation(), InOwnerCharacter->GetActorLocation());
									if (Distance <= TmpDistance)
									{
										TmpDistance = Distance;
										MissionTargetPtr = InCharacter;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
