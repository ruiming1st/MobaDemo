// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/BoxComponent.h"
#include "../CharacterInstance/MobyGameCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "MobyGame/Common/NumericalCalculationUnit.h"
#include "../AI/AIController/Core/MobyGameAIController.h"
#include "../Damage/BulletDamageType.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootBullet = CreateDefaultSubobject<USceneComponent>(TEXT("BulletRootBullet"));
	BoxDamage = CreateDefaultSubobject<UBoxComponent>(TEXT("BulletNoxDamage"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("BulletProjectileMovement"));

	RootComponent = RootBullet;
	BoxDamage->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	OpenFirePoint= CreateDefaultSubobject<UParticleSystemComponent>(TEXT("OpenFire"));
	Bullet = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet"));
	OpenFirePoint->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Bullet->AttachToComponent(BoxDamage, FAttachmentTransformRules::KeepRelativeTransform);

	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->InitialSpeed = 1600.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->UpdatedComponent = BoxDamage;

	//生命周期
	InitialLifeSpan = 4.0f;

	//节约带宽
	SetReplicates(true);
	bCheck = false;
	bSingleTarget = true;
	Type = EBulletType::BULLET_ATTACK;

	//+DefaultChannelResponses = (Channel = ECC_GameTraceChannel2, DefaultResponse = ECR_Overlap, bTraceType = False, bStaticObject = False, Name = "Bullet")
	BoxDamage->SetCollisionObjectType(ECC_GameTraceChannel2);
	SlotAttribute = NULL;
}


// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	//// Helper macro for calling AddUniqueDynamic() on dynamic multi-cast delegates.  Automatically generates the function name string.

	BoxDamage->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABullet::BeginOverlap);

	if (AttackType == EBulletAttackType::BULLET_CHAIN)
	{
		ProjectileMovement->StopMovementImmediately();
		BoxDamage->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	AttackTargetSystem.Init(GetWorld(), Cast<AMobyGameCharacter>(GetInstigator()));

	if (GetLocalRole() == ROLE_Authority)
	{
		AttackTargetSystem.RemoveFuntion = [&](AMobyGameCharacter* InCharacter)
		{
			//广播
			MultiCastRemoveTarget(InCharacter);
		};

		AttackTargetSystem.AddFuntion = [&](AMobyGameCharacter* InCharacter)
		{
			MultiCastAddTarget(InCharacter);
		};
	}
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AttackType == EBulletAttackType::BULLET_CHAIN)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			AttackTargetSystem.ServerTick(DeltaTime);
		}
		else
		{
			AttackTargetSystem.ClientTick(DeltaTime);
		}
	}
}


UParticleSystem* ABullet::GetHitParticles()
{
	if (HitParticles.Num())
	{
		int32 Index = FMath::RandRange(0, HitParticles.Num() - 1);
		return HitParticles[Index];
	}

	return NULL;
}

void ABullet::SetBoxSize(const FVector& InSize)
{
	BoxDamage->SetBoxExtent(InSize);
}

void ABullet::SetSingleTarget(bool bInSigleTarget)
{
	bSingleTarget = bInSigleTarget;
}

void ABullet::SetRelativePosition(const FVector& InPosition)
{
	BoxDamage->SetRelativeLocation(InPosition);
}

void ABullet::SetOpenFireActive(bool bActive)
{
	OpenFirePoint->SetActive(bActive);
}

void ABullet::SetSlotAttribute(const FSlotAttribute* InSlotAttribute)
{
	SlotAttribute = InSlotAttribute;
}

void ABullet::SetBulletType(EBulletType InType)
{
	Type = InType;
}

void ABullet::UpdateBullet()
{
	if (AMobyGameCharacter* InstigatorCharacter = Cast<AMobyGameCharacter>(GetInstigator()))
	{
		switch (Type)
		{
		case EBulletType::BULLET_ATTACK:
			break;
		case EBulletType::BULLET_DEFENCE:
			InstigatorCharacter->AddBuff(SlotAttribute, InstigatorCharacter->GetPlayerID());
			break;
		}

		if (SlotAttribute)
		{
			if (AMobyGamePlayerCharacter* NewCharacter = Cast<AMobyGamePlayerCharacter>(InstigatorCharacter))
			{
				if (AMobyGamePlayerState* InPlayerState = MethodUnit::GetPlayerState(GetWorld(), InstigatorCharacter->GetPlayerID()))
				{
					if (!InPlayerState->UseMana(SlotAttribute))
					{
						//终止动画
						NewCharacter->bPressSkillKey = false;
					}
				}
			}
		}
	}
}

void ABullet::MultiCastEndOpenFireParticle_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		SetOpenFireActive(false);
		Bullet->SetActive(false);
	}
}

void ABullet::MultiCastRemoveTarget_Implementation(AMobyGameCharacter* InCharacter)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		AttackTargetSystem.ClientRemove(InCharacter);
	}
}

void ABullet::MultiCastAddTarget_Implementation(AMobyGameCharacter* InCharacter)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		AttackTargetSystem.ClientAdd(InCharacter);
	}
}

void ABullet::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (AMobyGameCharacter* InstigatorCharacter = Cast<AMobyGameCharacter>(GetInstigator()))
	{
		if (Type == EBulletType::BULLET_DEFENCE)
		{
			if (ABullet* InTargetBullet = Cast<ABullet>(OtherActor))
			{
				if (AMobyGameCharacter* TargetCharacter = Cast<AMobyGameCharacter>(InTargetBullet->GetInstigator()))
				{
					if (InTargetBullet->GetBulletType() != EBulletType::BULLET_DEFENCE)
					{
						if (TargetCharacter->GetTeam() != InstigatorCharacter->GetTeam())
						{
							InTargetBullet->Destroy();
						}						
					}
				}
			}
		}
		else
		{
			if (AMobyGameCharacter* TargetCharacter = Cast<AMobyGameCharacter>(OtherActor))
			{
				if (InstigatorCharacter != TargetCharacter)
				{
					bool bTeam = InstigatorCharacter->GetTeam() != TargetCharacter->GetTeam();
					switch (Type)
					{
					case EBulletType::BULLET_GAIN:
						bTeam = InstigatorCharacter->GetTeam() == TargetCharacter->GetTeam();
						break;
					}

					//队伍
					if (bTeam)
					{
						if (GetWorld()->IsServer())
						{
							if (!TargetCharacter->IsDie())
							{
								//检测
								if (bCheck)
								{
									if (AMobyGameAIController* InstigatorCharacterController = Cast<AMobyGameAIController>(InstigatorCharacter->GetController()))
									{
										if (InstigatorCharacterController->GetTarget() != TargetCharacter)
										{
											return;
										}
									}
								}

								float DamageValue = NumericalCalculationUnit::GetDamage(TargetCharacter, InstigatorCharacter, SlotAttribute);

								//播放击中特效
								if (UParticleSystem* InParticle = GetHitParticles())
								{
									if (ProjectileMovement->InitialSpeed == 0)
									{
										TargetCharacter->MultiCastHitParticle(TargetCharacter->GetActorLocation(), InParticle);
									}
									else
									{
										TargetCharacter->MultiCastHitParticle(BoxDamage->GetComponentLocation(), InParticle);
									}

								}

								//传递参数
								if (UClass* InDamageTypeClass = UBulletDamageType::StaticClass())
								{
									if (UBulletDamageType* InDamage = InDamageTypeClass->GetDefaultObject<UBulletDamageType>())
									{
										//注册
										InDamage->SlotData = SlotAttribute;
										UGameplayStatics::ApplyDamage(TargetCharacter,
											DamageValue,
											InstigatorCharacter->GetController(),
											InstigatorCharacter,
											InDamageTypeClass);
									}

								}

								if (bSingleTarget)
								{
									Destroy();
								}
							}
						}
					}
				}
			}
		}
	}
}

