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
	//绑定开火点，不为true会子弹会随机产生，出现乱飞现象
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

		//只在服务器上进行发射子弹产生伤害，不进行同步
		if (Character->GetWorld()->IsServer())
		{
			if (AMobyGameCharacter* NewCharacter = Cast<AMobyGameCharacter>(MeshComp->GetOuter()))
			{
				if (AMobyGameAIController* OnwerController = Cast<AMobyGameAIController>(NewCharacter->GetController()))
				{
					if (OnwerController->GetTarget())
					{
						//将x轴方向对准目标玩家
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

			//不能采用客户端模拟的方案，因为会导致客户端模拟不精准的bug，需要都在服务器上运行
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

					//拿到技能数据
					if (UAnimMontage *InMontage = Cast<UAnimMontage>(Animation))
					{
						if (AMobyGamePlayerState *InPlayerState = NewCharacter->GetPlayerState())
						{
							//使用notifyattack属性中配置的SkillID找属性，没填就通过蒙太奇找技能并找到技能属性
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