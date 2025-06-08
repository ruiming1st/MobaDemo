// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"

AMonsterAIController::AMonsterAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> MonsterBehavior_Tree(TEXT("/Game/TopDownCPP/AI/Monster/MonsterBH"));
	BTAsset = MonsterBehavior_Tree.Object;
}

void AMonsterAIController::InitContorller()
{
	Super::InitController();
}

AMobyGameCharacter* AMonsterAIController::FindTarget()
{
	//��Χ��ͬ�鱻����������Ŀ��
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMobyGameCharacter::StaticClass(), OutActors);

	if (!GetTarget())
	{
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
							if (InCharacter->GetTeam() == InOwnerCharacter->GetTeam())
							{
								float Distance = FVector::Dist(Tmp->GetActorLocation(), InOwnerCharacter->GetActorLocation());
								if (Distance <= 1000.f)
								{
									if (AMonsterAIController* MonsterController = Cast<AMonsterAIController>(InCharacter->GetController()))
									{
										if (MonsterController->GetTarget())
										{
											return MonsterController->GetTarget();
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
	else
	{
		return GetTarget();
	}

	return NULL;
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckRange();
}

void AMonsterAIController::CheckRange()
{
	//Ŀ���Զ����Ŀ�겻���ڣ���ص�������
	if (!GetTarget())
	{
		SimpleMoveToLocation(HomePoint);
	}
	else if (GetPawn())
	{
		float Distance = FVector::Dist(HomePoint, GetPawn()->GetActorLocation());
		if (Distance >= 1000.f)
		{
			SetTargetForce(NULL);
			SimpleMoveToLocation(HomePoint);
		}
			
	}
}
