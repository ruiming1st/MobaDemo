// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretsAIController.h"
#include "../../CharacterInstance/MobyGameCharacter.h"
#include "../../CharacterInstance/MobyGameTurretsCharacter.h"
#include "Kismet/GameplayStatics.h"

ATurretsAIController::ATurretsAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> TurretsBehavior_Tree(TEXT("/Game/TopDownCPP/AI/Turrets/TurretsBH"));
	BTAsset = TurretsBehavior_Tree.Object;
}

void ATurretsAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetTarget())
	{
		if (AMobyGameTurretsCharacter* OnwerCharacter = GetPawn<AMobyGameTurretsCharacter>())
		{
			FRotator R = FRotationMatrix::MakeFromX(GetTarget()->GetActorLocation() - GetPawn()->GetActorLocation()).Rotator();
			OnwerCharacter->TowersRotator = FVector2D(R.Pitch, R.Yaw);
			if (GetPawn()->GetActorRotation() != FRotator::ZeroRotator)
			{
				//矫正
				FVector2D NewRot = FVector2D(GetPawn()->GetActorRotation().Pitch, GetPawn()->GetActorRotation().Yaw);
				OnwerCharacter->TowersRotator -= NewRot;
			}
		}
	}
	
}

AMobyGameCharacter* ATurretsAIController::FindTarget()
{
	//根据优先级寻找
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMobyGameCharacter::StaticClass(), OutActors);

	if (AMobyGameCharacter* InOwnerCharacter = Cast<AMobyGameCharacter>(GetPawn()))
	{

		if (!bIntruder)
		{
			float OwnerDistance = 999999.f;
			AMobyGameCharacter* TargetActor = NULL;

			for (auto &Tmp : OutActors)
			{
				
				if (Tmp != InOwnerCharacter)
				{
		
					if (AMobyGameCharacter* InTargetCharacter = Cast<AMobyGameCharacter>(Tmp))
					{
						if (!InTargetCharacter->IsDie())
						{

							//队伍不一样
							if (InTargetCharacter->GetTeam() != InOwnerCharacter->GetTeam())
							{

								float Distance = FVector::Dist(Tmp->GetActorLocation(), InOwnerCharacter->GetActorLocation());
								float AttackRange = InOwnerCharacter->GetCharacterAttribute()->RangeAttack;
								if (Distance <= AttackRange)
								{
									if (Distance < OwnerDistance)
									{
										OwnerDistance = Distance;
										TargetActor = InTargetCharacter;

										bIntruder = true;
									}
								}
							}
						}
					}			
				}
			}
			return TargetActor;
		}

		//搜集结构
		struct FAICharacterTarget
		{
			FAICharacterTarget()
				:Player(NULL)
				, Minion(NULL)
				, Monster(NULL)
				, PlayerReferenceDistance(999999.f)
				, MinionReferenceDistance(999999.f)
				, MonsterReferenceDistance(999999.f)
			{}

			AMobyGameCharacter* Player;
			AMobyGameCharacter* Minion;
			AMobyGameCharacter* Monster;
			float PlayerReferenceDistance;
			float MinionReferenceDistance;
			float MonsterReferenceDistance;
		}Targets;

		for (auto& Tmp : OutActors)
		{
			if (Tmp != InOwnerCharacter)
			{
				if (AMobyGameCharacter* InTargetCharacter = Cast<AMobyGameCharacter>(Tmp))
				{
					if (!InTargetCharacter->IsDie())
					{
						//队伍不一样
						if (InTargetCharacter->GetTeam() != InOwnerCharacter->GetTeam())
						{
							float Distance = FVector::Dist(Tmp->GetActorLocation(), InOwnerCharacter->GetActorLocation());
							float RangeAttack = InOwnerCharacter->GetCharacterAttribute()->RangeAttack;
							if (Distance <= RangeAttack)
							{
								if (InTargetCharacter->GetCharacterType() >= ECharacterType::REMOTE_MINION &&
									InTargetCharacter->GetCharacterType() <= ECharacterType::SUPER_MINION)//判断小兵
								{
									if (Distance < Targets.MinionReferenceDistance)
									{
										Targets.MinionReferenceDistance = Distance;
										Targets.Minion = InTargetCharacter;
									}
								}
								else if (InTargetCharacter->GetCharacterType() == ECharacterType::PLAYER)//判断玩家
								{
									if (Distance < Targets.PlayerReferenceDistance)
									{
										Targets.PlayerReferenceDistance = Distance;
										Targets.Player = InTargetCharacter;
									}
								}
								else if (InTargetCharacter->GetCharacterType() >= ECharacterType::WILD_MONSTER)//判断野怪
								{
									if (Distance < Targets.MonsterReferenceDistance)
									{
										Targets.MonsterReferenceDistance = Distance;
										Targets.Monster = InTargetCharacter;
									}
								}
							}
						}
					}
				}
			}
		}

		//根据优先级返回对象
		if (Targets.Monster)
		{
			return Targets.Monster;
		}
		else if (Targets.Minion)
		{
			return Targets.Minion;
		}
		else if (Targets.Player)
		{
			return Targets.Player;
		}
	}

	return NULL;
}

void ATurretsAIController::SetTarget(AMobyGameCharacter* InTarget)
{
	Super::SetTargetForce(InTarget);

	if (!InTarget)
	{
		bIntruder = false;
	}

}

void ATurretsAIController::InitController()
{
	Super::InitController();
}
