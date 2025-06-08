// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionsAIController.h"
#include "Kismet/GameplayStatics.h"
#include "../../CharacterInstance/MobyGameCharacter.h"

#define MAX_TMP 99999999
//��������Ϣ
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
	//�������ĵ��˵����ٵ����ü��������ô˼����ҳ���ͬ���������С�����ĵ��ˣ������ڶ������谴���ȼ�����
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

//���ü���
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

//������С�����ü���
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
	Super::BeginPlay();//��0.1s��ִ���麯����ʼ��Controller������AI��Ϊ������ʱ�����BTAsset�Ѿ���ȡ����û������

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
						//���鲻ͬ
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
	//�������ȼ�Ѱ��
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMobyGameCharacter::StaticClass(), OutActors);

	//�ռ����ǵ�����Ŀ��
	FindMissonTarget(OutActors);

	FAITargetRefs Targets;

	if (AMobyGameCharacter* InOwnerCharacter = Cast<AMobyGameCharacter>(GetPawn()))
	{
		//I.�ռ�
		for (auto &Tmp : OutActors)
		{
			if (Tmp != InOwnerCharacter)
			{
				if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(Tmp))
				{
					if (!InCharacter->IsDie())
					{
						//���鲻ͬ
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
								else if (InCharacter->GetCharacterType() >= ECharacterType::WILD_MONSTER && InCharacter->GetCharacterType() <= ECharacterType::WILD_BOSS_MONSTER)//�ж�Ұ��
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

		//II.��ѯͬ��Ŀ����
		for (auto &Tmp : OutActors)
		{
			if (Tmp != InOwnerCharacter)
			{
				if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(Tmp))
				{
					if (!InCharacter->IsDie())
					{
						//������ͬ�Ķ���
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
								//���鲻ͬ
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
