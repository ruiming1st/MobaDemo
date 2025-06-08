// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Attack.h"
#include "../Item/Bullet.h"
#include "../CharacterInstance/MobyGameCharacter.h"
#include "../AI/AIController/Core/MobyGameAIController.h"
#include "MobyGame/Common/MethodUnit.h"

UAnimNotify_Attack::UAnimNotify_Attack()
{
	InSocketName = TEXT("OpenFire");
	bCheck = false;
	bSingleTarget = true;
	//�󶨿���㣬��Ϊtrue���ӵ�����������������ҷ�����
	bBind = true;
	BoxSize = FVector(32.f);
	RelativeLocation = FVector::ZeroVector;
	LifeTime = 4.f;
	SlotID = INDEX_NONE;
	BulletType = EBulletType::BULLET_ATTACK;
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(196, 142, 255, 255);
#endif // WITH_EDITORONLY_DATA
}

FString UAnimNotify_Attack::GetNotifyName_Implementation() const
{
	if (BulletClass)
	{
		return BulletClass->GetName();
	}

	return Super::GetNotifyName_Implementation();
}


void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!BulletClass)
	{
		return;
	}

	if (AActor* Character = Cast<AActor>(MeshComp->GetOuter()))
	{
		FVector ComponentLocation = MeshComp->GetSocketLocation(InSocketName);
		FRotator ComponentRotation = MeshComp->GetSocketRotation(InSocketName);

		//ֻ�ڷ������Ͻ��з����ӵ������˺���������ͬ��
		if (Character->GetWorld()->IsServer())
		{
			if (AMobyGameCharacter* NewCharacter = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
			{
				if (AMobyGameAIController* OnwerController = Cast<AMobyGameAIController>(NewCharacter->GetController()))
				{
					if (OnwerController->GetTarget())
					{
						//��x�᷽���׼Ŀ�����
						ComponentRotation = FRotationMatrix::MakeFromX(OnwerController->GetTarget()->GetActorLocation() - ComponentLocation).Rotator();

						//if (NewCharacter->GetCharacterType() >= ECharacterType::FIRST_CLASS_TURRETS &&
						//	NewCharacter->GetCharacterType() <= ECharacterType::BASE_TOWER)
						//{
						//	if (NewCharacter->GetActorRotation() != FRotator::ZeroRotator)
						//	{
						//		ComponentRotation -= NewCharacter->GetActorRotation();
						//	}
						//}
					}
				}
			}

			//���ܲ��ÿͻ���ģ��ķ�������Ϊ�ᵼ�¿ͻ���ģ�ⲻ��׼��bug����Ҫ���ڷ�����������
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.Instigator = Cast<APawn>(Character);
			if (ABullet* Bullet = Character->GetWorld()->SpawnActor<ABullet>(BulletClass, ComponentLocation, ComponentRotation, ActorSpawnParameters))
			{
				Bullet->SetCheck(bCheck);
				Bullet->SetLifeSpan(LifeTime);
				Bullet->SetBoxSize(BoxSize);
				Bullet->SetRelativePosition(RelativeLocation);
				Bullet->SetSingleTarget(bSingleTarget);
				Bullet->SetBulletType(BulletType);

				if (AMobyGameCharacter* NewCharacter = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
				{
					if (bBind)
					{
						Bullet->AttachToComponent(NewCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
					}

					NewCharacter->AddCurrentBullet(Bullet);

					//�õ���������
					if (UAnimMontage *InMontage = Cast<UAnimMontage>(Animation))
					{
						if (AMobyGamePlayerState *InPlayerState = NewCharacter->GetPlayerState())
						{
							//ʹ��notifyattack���������õ�SkillID�����ԣ�û���ͨ����̫���Ҽ��ܲ��ҵ���������
							if (SkillID != INDEX_NONE)
							{
								if (const FSlotAttribute *InSlotAttribute1 = InPlayerState->GetSlotAttributeBySkillID(SkillID))
								{
									Bullet->SetSlotAttribute(InSlotAttribute1);
								}
							}
							else
							{
								if (const FSlotAttribute* InSlotAttribute2 = MethodUnit::GetSlotAttributeByPlayer(InPlayerState, InMontage))
								{
									Bullet->SetSlotAttribute(InSlotAttribute2);
								}
							}
						}
					}
				}

				Bullet->UpdateBullet();
			}
		}
	}
}