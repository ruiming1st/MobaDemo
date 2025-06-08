// Fill out your copyright notice in the Description page of Project Settings.


#include "MobyGamePlayerCharacter.h"
#include "../../Common/MethodUnit.h"
#include "../../Table/CharacterTable.h"
#include "../../MobyPawn.h"
#include "ThreadManage.h"
#include "../../UI/Game/Character/UI_InformationBar.h"

AMobyGamePlayerCharacter::AMobyGamePlayerCharacter() : Super()
{
	bPressSkillKey = false;

	TipComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TipComponent"));
	TipComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TipComponent->SetHiddenInGame(true);
}

void AMobyGamePlayerCharacter::InitCharacter()
{
	Super::InitCharacter();

	//if (AMobyGameState* InState = MethodUnit::GetGameState(GetWorld()))
	//{
	//	if (FCharacterAttribute* Data = InState->GetCharacterAttribute(GetPlayerID()))
	//	{
	//		MethodUnit::ServerCallAllPlayer<AMobyPawn>(GetWorld(), [&](AMobyPawn* InPawn)->MethodUnit::EServerCallType 
	//		{
	//			if (InPawn->GetPlayerID() == GetPlayerID())
	//			{
	//				if (AMobyGamePlayerState* InPlayerState = InPawn->GetController()->GetPlayerState<AMobyGamePlayerState>())
	//				{
	//					MultiCastWidgetInfo_PlayerName(InPlayerState->GetPlayerData()->PlayerName.ToString());
	//			
	//				}
	//				return MethodUnit::EServerCallType::PROGRESS_COMPLETE;
	//			}

	//			return MethodUnit::EServerCallType::INPROGRESS;
	//		});
	//
	//		MultiCastWidgetInfo_Lv(Data->Lv);
	//	}
	//}
}

void AMobyGamePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)//IsServer()
	{
		GThread::Get()->GetCoroutines().BindLambda(0.5f, [&]()
		{
			MethodUnit::ServerCallAllCharacter<AMobyGameCharacter>(GetWorld(), [&](AMobyGameCharacter* InCharacter)->MethodUnit::EServerCallType
			{
				InCharacter->InitWidgetInfo();

				return MethodUnit::EServerCallType::INPROGRESS;
			});
		});
	}

	if (GetLocalRole() != ROLE_Authority)
	{
		GThread::Get()->GetCoroutines().BindLambda(1.f, [&]()
		{
			InitWidgetInfoOnClient();
		});
	}
}

void AMobyGamePlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//������ʾ����
	if (GetLocalRole() != ENetRole::ROLE_Authority) //�ͻ��˼�����Ƭ��ת
	{
		if (TipComponent)
		{
			if (TipComponent->IsVisible())
			{
				if (GetWorld() && GetWorld()->GetFirstPlayerController())
				{
					FHitResult HitResult;
					GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
					if (HitResult.bBlockingHit)
					{
						FVector TargetLocation = HitResult.Location;
						TargetLocation.Z = TipComponent->GetComponentLocation().Z;

						FRotator R = FRotationMatrix::MakeFromX(TargetLocation - TipComponent->GetComponentLocation()).Rotator();
						TipComponent->SetWorldRotation(R);
					}
				}
			}
		}
	}
}

void AMobyGamePlayerCharacter::SkillAttack(int32 InSkillID)
{
	int32 CharacterID = MethodUnit::GetCharacterID(GetWorld(), GetPlayerID());

	if (CharacterID != INDEX_NONE)
	{
		if (const FCharacterTable* CharacterTable = MethodUnit::GetCharacterTableByCharacterID(GetWorld(), CharacterID))
		{
			if (UAnimMontage* MontageInstance = GetCurrentSkillMontage(InSkillID))
			{
				//���Ŷ���
				MultiCastPlayerAnimMontage(MontageInstance);
			}

		}
	}
}

UAnimMontage* AMobyGamePlayerCharacter::GetCurrentSkillMontage(int32 InSkillID)
{
	int32 CharacterID = MethodUnit::GetCharacterID(GetWorld(), GetPlayerID());
	if (const FCharacterTable* CharacterTable = MethodUnit::GetCharacterTableByCharacterID(GetWorld(), CharacterID))
	{
		if (CharacterTable->QSkill.ID == InSkillID)
		{
			return CharacterTable->QSkill.SkillMontage;
		}
		else if (CharacterTable->WSkill.ID == InSkillID)
		{
			return CharacterTable->WSkill.SkillMontage;
		}
		else if (CharacterTable->ESkill.ID == InSkillID)
		{
			return CharacterTable->ESkill.SkillMontage;
		}
		else if (CharacterTable->RSkill.ID == InSkillID)
		{
			return CharacterTable->RSkill.SkillMontage;
		}
		
	}

	return NULL;
}

void AMobyGamePlayerCharacter::AddExp(float InExp)
{
	if (FCharacterAttribute* InCharacterAttribute = GetCharacterAttribute())
	{
		if (AMobyGameState *InGameState = GetWorld()->GetGameState<AMobyGameState>())
		{
			if (AMobyPawn *InPawn = MethodUnit::GetPawn(GetWorld(), GetPlayerID()))
			{
				InCharacterAttribute->EmpircalValue += InExp;
				if (InCharacterAttribute->EmpircalValue >= InCharacterAttribute->MaxEmpircalValue)
				{
					if (!InCharacterAttribute->IsLvUpperLimit())
					{
						InCharacterAttribute->EmpircalValue -= InCharacterAttribute->MaxEmpircalValue;
						InCharacterAttribute->UpdateLevel();

						InGameState->RequestCharacterAttribute(GetPlayerID(), GetPlayerID(), ECharacterAttributeType::ATTR_NONE);

						//�����Ӽ��ܵ�
						if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
						{
							InPlayerState->GetPlayerData()->SkillPoint++;
						}

						//����Ѫ��
						MultiCastWidgetInfo(
							InCharacterAttribute->GetHealthPercentage(),
							InCharacterAttribute->GetManaPercentage());

						//�㲥�ȼ�
						InGameState->NetMulticastCharacterAttribute(this, InCharacterAttribute->Lv, ECharacterAttributeType::ATTR_LV);

						//�ͻ�����ʾ���������� ��ʾ���ܼӵ����
						if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
						{
							InPlayerState->ShowSkillLv();
						}
					}
					else
					{
						//���������
						InCharacterAttribute->EmpircalValue = InCharacterAttribute->MaxEmpircalValue;
					}
				}
				else
				{
					InGameState->RequestCharacterAttribute(GetPlayerID(), GetPlayerID(), ECharacterAttributeType::ATTR_EXP);

					//�ͻ�����ʾ���Ǿ���ֵ����
					InPawn->ServerCallClientSpawnDrawExpText(GetActorLocation(), InExp, 0.5f, FLinearColor::Yellow);
				}
			}
		}
	}
}

void AMobyGamePlayerCharacter::MultiCastWidgetInfo_Lv_Implementation(const int32 &InCharacterLv)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		if (UUI_InformationBar * InformationBar = Cast<UUI_InformationBar>(GetWidget()->GetUserWidgetObject()))
		{
			InformationBar->SetLv(InCharacterLv);
		}
	}
}

void AMobyGamePlayerCharacter::MultiCastWidgetInfo_PlayerName_Implementation(const FString &InPlayerName)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		if (UUI_InformationBar * InformationBar = Cast<UUI_InformationBar>(GetWidget()->GetUserWidgetObject()))
		{
			InformationBar->SetTitle(InPlayerName);
		}
	}
}

void AMobyGamePlayerCharacter::InitWidgetInfoOnClient()
{
	//�ͻ��˼���֮ǰδ����Player
	SetTitle(PlayerName);
}

void AMobyGamePlayerCharacter::Die()
{
	DieDelegate.ExecuteIfBound();
}

void AMobyGamePlayerCharacter::SetSkillTip(UMaterialInterface* InMaterialInterface)
{
	if (InMaterialInterface)
	{
		if (TipComponent)
		{
			TipComponent->SetMaterial(0, InMaterialInterface);
			TipComponent->SetHiddenInGame(false);
		}

	}
	else
	{
		if (TipComponent)
		{
			TipComponent->SetHiddenInGame(true);
		}
	}
}
