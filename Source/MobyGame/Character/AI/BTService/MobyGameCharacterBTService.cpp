// Fill out your copyright notice in the Description page of Project Settings.


#include "MobyGameCharacterBTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "../AIController/Core/MobyGameAIController.h"


void UMobyGameCharacterBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (BlackboardKey_Target.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() &&
		BlackboardKey_Distance.SelectedKeyType == UBlackboardKeyType_Float::StaticClass() &&
		BlackboardKey_Location.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass() && BlackboardKey_Death.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
	{
		if (AMobyGameAIController* OwnerController = Cast<AMobyGameAIController>(OwnerComp.GetOwner()))
		{
			if (AMobyGameCharacter* OwnerCharacter = Cast<AMobyGameCharacter>(OwnerController->GetPawn()))
			{
				if (UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent())
				{
					MyBlackBoard->SetValueAsBool(BlackboardKey_Death.SelectedKeyName, OwnerCharacter->IsDie());
					if (OwnerCharacter->IsDie())
					{
						return;
					}

					AMobyGameCharacter* InTarget = Cast<AMobyGameCharacter>(MyBlackBoard->GetValueAsObject(BlackboardKey_Target.SelectedKeyName));

					float RangeAttack = OwnerCharacter->GetCharacterAttribute()->RangeAttack;

					auto IsMissionTarget = [&]()->bool
					{
						if (InTarget && InTarget == OwnerController->GetMissionTarget())
						{
							return true;
						}

						return false;
					};

					if (!IsMissionTarget())
					{
						if (!InTarget || InTarget->IsDie())
						{
							InTarget = OwnerController->FindTarget();
							if (InTarget)
							{
								MyBlackBoard->SetValueAsObject(BlackboardKey_Target.SelectedKeyName, InTarget);
							}
						}
					}
					else
					{
						InTarget = OwnerController->FindTarget();
						if (InTarget)
						{
							if (!IsMissionTarget())
							{
								float Distance = FVector::Dist(InTarget->GetActorLocation(), OwnerCharacter->GetActorLocation());

								if (Distance > RangeAttack)
								{
									InTarget = OwnerController->GetMissionTarget();
								}
							}
						}

						MyBlackBoard->SetValueAsObject(BlackboardKey_Target.SelectedKeyName, InTarget);
					}

					float Distance = 999999.0f;
					auto CheckRangeAttack = [&]()
					{
						if (Distance > RangeAttack)
						{
							MyBlackBoard->SetValueAsVector(BlackboardKey_Location.SelectedKeyName, InTarget->GetActorLocation());
						}
						else
						{
							FVector D = InTarget->GetActorLocation() - OwnerCharacter->GetActorLocation();
							D.Normalize();
							FVector Pos = OwnerCharacter->GetActorLocation() + D * 20;

							MyBlackBoard->SetValueAsVector(BlackboardKey_Location.SelectedKeyName, Pos);
						}
					};

					//一旦传入对象，更新location，以便角色旋转
					if (InTarget)
					{
						Distance = FVector::Dist(InTarget->GetActorLocation(), OwnerCharacter->GetActorLocation());
						if (IsMissionTarget())
						{
							CheckRangeAttack();
						}
						else
						{
							if (Distance > 1000.f)
							{
								OwnerController->SetTargetForce(NULL);
								Distance = 999999.0f;
							}
							else
							{
								CheckRangeAttack();
							}
						}

					}

					MyBlackBoard->SetValueAsFloat(BlackboardKey_Distance.SelectedKeyName, Distance);
				}
			}		
		}
	}
}

void UMobyGameCharacterBTService::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		BlackboardKey_Target.ResolveSelectedKey(*BBAsset);
		BlackboardKey_Distance.ResolveSelectedKey(*BBAsset);
		BlackboardKey_Location.ResolveSelectedKey(*BBAsset);
		BlackboardKey_Death.ResolveSelectedKey(*BBAsset);

	}
}
