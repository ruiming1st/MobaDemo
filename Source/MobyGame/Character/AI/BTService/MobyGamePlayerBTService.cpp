// Fill out your copyright notice in the Description page of Project Settings.


#include "MobyGamePlayerBTService.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "../AIController/Core/MobyGameAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UMobyGamePlayerBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (BlackBoardKey_Target.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() &&
		BlackBoardKey_Distance.SelectedKeyType == UBlackboardKeyType_Float::StaticClass() &&
		BlackBoardKey_Location.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass() &&
		BlackBoardKey_Death.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass())
	{
		if (AMobyGameAIController* OwnerController = Cast<AMobyGameAIController>(OwnerComp.GetOwner()))
		{
			if (AMobyGameCharacter* OwnerCharacter = Cast<AMobyGameCharacter>(OwnerController->GetPawn()))
			{
				if (UBlackboardComponent* MyBlackBoard = OwnerComp.GetBlackboardComponent())
				{
					MyBlackBoard->SetValueAsBool(BlackBoardKey_Death.SelectedKeyName, OwnerCharacter->IsDie());
					if (OwnerCharacter->IsDie())
					{
						return;
					}

					AMobyGameCharacter* InTarget = Cast<AMobyGameCharacter>(MyBlackBoard->GetValueAsObject(BlackBoardKey_Target.SelectedKeyName));
					if (!InTarget)
					{
						InTarget = OwnerController->FindTarget();
						if (InTarget)
						{
							MyBlackBoard->SetValueAsObject(BlackBoardKey_Target.SelectedKeyName, InTarget);
						}
					}

					float Distance = 999999.0f;
					if (InTarget)
					{
						Distance = FVector::Dist(InTarget->GetActorLocation(), OwnerCharacter->GetActorLocation());
						//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("%f"), Distance));
						if (Distance > 2000.f || InTarget->IsDie())
						{
							OwnerController->SetTargetForce(NULL);
							Distance = 999999.0f;

							//һ��ʧȥĿ�� ����Ҫ�����趨Ŀ��
							if (InTarget->IsDie())
							{
								MyBlackBoard->SetValueAsVector(BlackBoardKey_Location.SelectedKeyName, OwnerCharacter->GetActorLocation());
							}
							else
							{
								//����Ҫ�����趨Ŀ��
								//MyBlackBoard->SetValueAsVector(BlackBoardKey_Location.SelectedKeyName, InTarget->GetActorLocation());
							}
						}
						else
						{
							float RangeAttack = OwnerCharacter->GetCharacterAttribute()->RangeAttack;
							if (Distance > RangeAttack)
							{
								MyBlackBoard->SetValueAsVector(BlackBoardKey_Location.SelectedKeyName, InTarget->GetActorLocation());
							}
							else
							{
								//ƫ�ƽ��� ����ת��
								FVector D = InTarget->GetActorLocation() - OwnerCharacter->GetActorLocation();
								D.Normalize();
								FVector Pos = OwnerCharacter->GetActorLocation() + D * 20;
								MyBlackBoard->SetValueAsVector(BlackBoardKey_Location.SelectedKeyName, Pos);
							}
						}
					}

					MyBlackBoard->SetValueAsFloat(BlackBoardKey_Distance.SelectedKeyName, Distance);
				}
			}
		}
	}
}

void UMobyGamePlayerBTService::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		BlackBoardKey_Target.ResolveSelectedKey(*BBAsset);
		BlackBoardKey_Distance.ResolveSelectedKey(*BBAsset);	
		BlackBoardKey_Location.ResolveSelectedKey(*BBAsset);
		BlackBoardKey_Death.ResolveSelectedKey(*BBAsset);
	}
}