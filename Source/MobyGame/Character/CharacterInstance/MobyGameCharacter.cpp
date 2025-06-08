// Copyright Epic Games, Inc. All Rights Reserved.

#include "MobyGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "../../Common/MethodUnit.h"
#include "../../Table/CharacterTable.h"
#include "../../UI/Game/Character/UI_InformationBar.h"
#include "Components/ArrowComponent.h"
#include "ThreadManage.h"
#include "../../MobyPawn.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../Damage/BulletDamageType.h"


AMobyGameCharacter::AMobyGameCharacter() : bAttacking(false), AttackCount(0), PlayerID(INDEX_NONE)
{
	bLockRot = false;
	CurrentRot = GetActorRotation();

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	PlayerName = "Sakura";

	OpenFirePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("OpenFirePoint"));
	OpenFirePoint->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}


void AMobyGameCharacter::InitCharacter()
{
	if (AMobyGameState* InState = MethodUnit::GetGameState(GetWorld()))
	{
		if (FCharacterAttribute* Data = InState->GetCharacterAttribute(PlayerID))
		{
			GetCharacterMovement()->MaxWalkSpeed = Data->WalkSpeed;
			
			MultiCastWidgetInfo(Data->GetHealthPercentage(), Data->GetManaPercentage());	
			MultiCastRegisterCharacter(PlayerID);
		}
	}

	if (AMobyGameAIController* GameAIController = Cast<AMobyGameAIController>(GetController()))
	{
		GameAIController->HomePoint = GetActorLocation();
	}
}


void AMobyGameCharacter::InitWidgetInfo()
{
	//更新血条
	MultiCastWidgetInfo(
		GetCharacterAttribute()->GetHealthPercentage(),
		GetCharacterAttribute()->GetManaPercentage());
}

void AMobyGameCharacter::AddCurrentBullet(ABullet* InInstance)
{
	if (!BulletWeakPtrs.Contains(InInstance))
	{
		BulletWeakPtrs.Add(InInstance);
	}
}

void AMobyGameCharacter::CallBulletEndOpenFire()
{
	if (BulletWeakPtrs.Num())
	{
		TArray<TWeakObjectPtr<ABullet>> RemoveIndex;
		for (auto &Tmp : BulletWeakPtrs)
		{
			if (Tmp.IsValid())
			{
				Tmp->MultiCastEndOpenFireParticle();
			}
			else
			{
				RemoveIndex.Add(Tmp);
			}
		}

		for (auto &Tmp : RemoveIndex)
		{
			BulletWeakPtrs.Remove(Tmp);
		}
	}
}

FVector AMobyGameCharacter::GoHome()
{
	FVector HomePoint = GetHomePoint();

	//1.重置角色位置
	SetActorLocation(HomePoint);

	//2.清除角色上一回合的任务
	if (AMobyGameAIController* GameAIController = Cast<AMobyGameAIController>(GetController()))
	{
		GameAIController->ClearMission();
	}

	//3.重置摄像机
	if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
	{
		InPlayerState->GetPawn<AMobyPawn>()->ServerCallClientSetPawnLocation(HomePoint);
	}

	return HomePoint;
}

void AMobyGameCharacter::NormalAttack(TWeakObjectPtr<AMobyGameCharacter> InTarget)
{
	if (InTarget.IsValid())
	{
		int32 CharacterID = MethodUnit::GetCharacterID(GetWorld(), PlayerID);
		if (CharacterID != INDEX_NONE)
		{
			if (const FCharacterTable* CharacterTable = MethodUnit::GetCharacterTableByCharacterID(GetWorld(), CharacterID))
			{
				if (AttackCount < CharacterTable->NormalAttack.Num())
				{
					if (UAnimMontage* MontageInstance = CharacterTable->NormalAttack[AttackCount])
					{
						if (AttackCount == CharacterTable->NormalAttack.Num() - 1)
						{
							AttackCount = 0;
						}
						else
						{
							AttackCount++;
						}

						//播放动画
						MultiCastPlayerAnimMontage(MontageInstance);

					}

				}
			}
		}
		
		
	}
}

void AMobyGameCharacter::MultiCastRegisterCharacter_Implementation(const int64& InPlayerID)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		PlayerID = InPlayerID;

		if (UUI_HealthBar* InHealthBar = Cast<UUI_HealthBar>(Widget->GetUserWidgetObject()))
		{
			InHealthBar->SetPlayerID(PlayerID);
		}
	}
}

void AMobyGameCharacter::MultiCastHitParticle_Implementation(const FVector& InHitLocation, UParticleSystem* InParInstance)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InParInstance, InHitLocation);
	}
}

void AMobyGameCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		if (AMobyGameState* InState = MethodUnit::GetGameState(GetWorld()))
		{
			InState->UpdateCharacterAILocation(PlayerID, GetActorLocation());
		}

		if (bLockRot)
		{
			SetActorRotation(CurrentRot);
		}
	}
}

float AMobyGameCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (GetLocalRole() == ROLE_Authority)
	{
		if (!IsDie())
		{
			if (AMobyGameCharacter* InDamageCauser = Cast<AMobyGameCharacter>(DamageCauser))
			{
				if (InDamageCauser->GetTeam() != GetTeam())//队伍不一样才能产生伤害
				{
					if (AMobyGameState* InState = MethodUnit::GetGameState(GetWorld()))
					{
						if (FCharacterAttribute *InCharacterData = GetCharacterAttribute())//数据
						{
							if (UBulletDamageType* BulletDamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UBulletDamageType>())
							{
								const	FSlotAttribute* SlotData = BulletDamageType->SlotData;

								//击飞
								if (SlotData)
								{
									if (SlotData->Buff)
									{
										AddBuff(SlotData->Buff, InDamageCauser->GetPlayerID());

										//击飞
										if (SlotData->Buff->EffectType == ESkillEffectType::EFFECT_STRIKE)
										{
											LaunchCharacter(DamageCauser->GetActorForwardVector() * 2000.f, true, true);
										}
										else if (SlotData->Buff->EffectType == ESkillEffectType::EFFECT_LIMIT)
										{
											SetLimit(true);
										}
										else if (SlotData->Buff->EffectType == ESkillEffectType::EFFECT_SILENT)
										{
											SetSilent(true);
										}
										else if (SlotData->Buff->EffectType == ESkillEffectType::EFFECT_VERTIGO)
										{
											SetVertigo(true);
										}
									}
								}


								//攻击角色
								GetCharacterAttribute()->Health -= Damage;

								if (GetCharacterAttribute()->Health < 0.f)
								{
									GetCharacterAttribute()->Health = 0.f;
								}

								//广播 隐藏血条
								ServerCallClientTakeDamage(IsDie());

								MultiCastWidgetInfo(GetCharacterAttribute()->GetHealthPercentage(), GetCharacterAttribute()->GetManaPercentage());

								//更新UI
								InState->RequestCharacterAttribute(PlayerID, PlayerID, ECharacterAttributeType::ATTR_HEALTH);


								//发送描述伤害结构
								{
									if (InState->IsPlayer(PlayerID))
									{
										if (AMobyPawn* InPawn = MethodUnit::GetPawn(GetWorld(), PlayerID))
										{

											InPawn->ServerCallClientSpawnDrawText(GetActorLocation(), Damage, Damage / GetCharacterAttribute()->MaxHealth, FLinearColor::Red);
										}
									}
									else
									{
										if (AMobyPawn* InPawn = MethodUnit::GetPawn(GetWorld(), InDamageCauser->GetPlayerID()))
										{
											InPawn->ServerCallClientSpawnDrawText(GetActorLocation(), Damage, Damage / GetCharacterAttribute()->MaxHealth, FLinearColor::Red);
										}
									}
								}

								if (IsDie())
								{
									int32 CharacterID = InState->GetCharacterID(PlayerID);
									//广播死亡动画
									if (const FCharacterTable* InTable = InState->GetCharacterTableTemplate(CharacterID))
									{
										//先处理死亡
										Die();

										int32 Index = FMath::RandRange(0, InTable->Death.Num() - 1);
										if (InTable->Death.IsValidIndex(Index))
										{
											MultiCastPlayerAnimMontage(InTable->Death[Index]);
										}

										//处理死亡动画等待 bug
										//float WaitTime = InTable->Death[Index]->GetPlayLength() + 0.8f;
										//GThread::Get()->GetCoroutines().BindUObject(WaitTime, this, &AMobyGameCharacter::Die);
									}

									if (InState->IsPlayer(PlayerID))
									{
										//消除成就
										InState->Death(PlayerID);

										//通知客户端 死亡 处理死亡效果 屏幕变灰等
										if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
										{
											InPlayerState->ServerCallClientDie(InState->GetRebornTime(this));
										}
									}

									//结算奖励
									InState->SettlementDeath(InDamageCauser->GetPlayerID(), PlayerID);

								}
								else
								{
									//添加助攻
									if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
									{
										if (InState->IsPlayer(InDamageCauser->GetPlayerID()))
										{
											InPlayerState->AddAssist(InDamageCauser->GetPlayerID());
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

	return 0;
}



void AMobyGameCharacter::SetNewHealth(float InHP)
{
	if (UUI_HealthBar* InHealthBar = Cast<UUI_HealthBar>(Widget->GetUserWidgetObject()))
	{
		InHealthBar->SetHealth(InHP);
	}

	if (InHP <= 0.f) //死亡
	{
		Widget->SetVisibility(false);
	}
}

void AMobyGameCharacter::SetNewMana(float InMP)
{
	if (UUI_InformationBar* InformationBar = Cast<UUI_InformationBar>(Widget->GetUserWidgetObject()))
	{
		InformationBar->SetMana(InMP);
	}
}

void AMobyGameCharacter::SetNewLv(float InLv)
{
	if (UUI_InformationBar* InformationBar = Cast<UUI_InformationBar>(Widget->GetUserWidgetObject()))
	{
		InformationBar->SetLv((int32)InLv);
	}
}

void AMobyGameCharacter::SetTitle(FName InName)
{
	if (UUI_InformationBar* InformationBar = Cast<UUI_InformationBar>(Widget->GetUserWidgetObject()))
	{
		InformationBar->SetTitle(InName.ToString());
	}
}

void AMobyGameCharacter::SetVertigo(bool bSet)
{
	//屏蔽技能
	SetSilent(bSet);

	//屏蔽移动
	SetLimit(bSet);

	//屏蔽旋转
	LockRot(bSet);

	//停止攻击
	if (AMobyGameAIController *InController = GetController<AMobyGameAIController>())
	{
		InController->SetTargetForce(NULL);
		InController->SetStopAttack(bSet);
	}
}

void AMobyGameCharacter::SetSilent(bool bSet)
{
	if (AMobyGamePlayerState *InGamePlayer = GetPlayerState())
	{
		InGamePlayer->SetLockSkill(bSet);
	}
}

void AMobyGameCharacter::SetLimit(bool bSet)
{
	if (bSet)
	{
		ResetSpeed(0.f);
	}
	else
	{
		ResetSpeed(INDEX_NONE);
	}
}

FVector AMobyGameCharacter::GetHomePoint()
{
	if (AMobyGameAIController* GameAIController = Cast<AMobyGameAIController>(GetController()))
	{
		return GameAIController->HomePoint;
	}

	return GetActorLocation();
}

void AMobyGameCharacter::MultiCastWidgetInfo_Health_Implementation(float InHPPercentage)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		if (UUI_InformationBar * InformationBar = Cast<UUI_InformationBar>(Widget->GetUserWidgetObject()))
		{
			InformationBar->SetHealth(InHPPercentage);
		}
	}
}

void AMobyGameCharacter::MultiCastWidgetInfo_Mana_Implementation(float InManaPercentage)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		if (UUI_InformationBar * InformationBar = Cast<UUI_InformationBar>(Widget->GetUserWidgetObject()))
		{
			InformationBar->SetMana(InManaPercentage);
		}
	}
}

void AMobyGameCharacter::MultiCastWidgetInfo_Implementation(float InHPPercentage, float InManaPercentage)
{	
	if (GetLocalRole() != ROLE_Authority)
	{
		//通过GetUserWidgetObject获取的类是蓝图类，父类是UUI_InformationBar，因此要转一下
		if (UUI_InformationBar * InformationBar = Cast<UUI_InformationBar>(Widget->GetUserWidgetObject()))
		{
			InformationBar->SetHealth(InHPPercentage);
			InformationBar->SetMana(InManaPercentage);
		}
		else if (UUI_HealthBar * HealthBar = Cast<UUI_HealthBar>(Widget->GetUserWidgetObject()))
		{
			HealthBar->SetHealth(InHPPercentage);
		
		}

		if (InHPPercentage <= 0.f)//死亡  客户端
		{
			Widget->SetVisibility(false);
		}
	}
}

void AMobyGameCharacter::MultiCastReborn_Implementation()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		GetCharacterAttribute()->Reset();

		MultiCastWidgetInfo(GetCharacterAttribute()->GetHealthPercentage(), GetCharacterAttribute()->GetManaPercentage());

		if (AMobyGameState *InState = MethodUnit::GetGameState(GetWorld()))
		{
			//通知更新UI
			InState->RequestCharacterAttribute(PlayerID, PlayerID, ECharacterAttributeType::ATTR_HEALTH);
		}

		//回家
		GoHome();
	}
	else
	{
		Widget->SetVisibility(true);

		//通知UI重生
		if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
		{
			InPlayerState->Reborn();

			//重置相机位置
			//InPlayerState->GetPawn()->SetActorLocation(GetActorLocation());
		}
	}

	StopAnimMontage();
}



FCharacterAttribute* AMobyGameCharacter::GetCharacterAttribute()
{
	if (AMobyGameState* InState = MethodUnit::GetGameState(GetWorld()))
	{
		return InState->GetCharacterAttribute(PlayerID);
	}

	return NULL;
}

void AMobyGameCharacter::AddBuff(const FSlotAttribute* SlotData, int64 InPlayerID)
{
	//暂时先这么写
	if (SlotData)
	{
		//添加属性到我们的固定角色身上
		if (SlotData->Type == ESlotAttributeType::SLOTATTRIBUTETYPE_CONTINUED)
		{
			if (AMobyGamePlayerState* InPlayerState = GetPlayerState())//自己
			{
				InPlayerState->AddBuff(SlotData, InPlayerID);
			}
		}
	}
}

void AMobyGameCharacter::RegisterCharacter(const int64& InPlayerID, int32 CharacterID, FName InName)
{
	PlayerID = InPlayerID;
	PlayerName = InName;

	if (AMobyGameState *InState = MethodUnit::GetGameState(GetWorld()))
	{
		//重置血量和蓝条
		InState->AddCharacterData(PlayerID, CharacterID);
		InState->AddCharacterAILocation(InPlayerID, CharacterID, GetActorLocation());

		//如果直接InitCharacter，虽然角色身上挂载的Widget已经生成，但是InformationBar类是在蓝图里配的，事实上是先执行了InitCharacter然后才初始化InformationBar，因此血条和蓝条不是满状态
		//InitCharacter();
		GThread::Get()->GetCoroutines().BindUObject(2.f, this, &AMobyGameCharacter::InitCharacter);

	}

}

void AMobyGameCharacter::SetClientWidgetVisibility_Implementation(bool bVisibility)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		SetWidgetVisibility(bVisibility);
	}
}

void AMobyGameCharacter::ServerCallClientTakeDamage_Implementation(bool bDie)
{
	Hurt(bDie);
}

void AMobyGameCharacter::SetWidgetVisibility(bool bVisibility)
{
	if (Widget)
	{
		Widget->SetVisibility(bVisibility);
	}
}

void AMobyGameCharacter::Hurt(bool bDie)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		if (!bDie)
		{
			SetWidgetVisibility(true);
		}
		else
		{
			SetWidgetVisibility(false);
		}
	}
}

void AMobyGameCharacter::Die()
{
	Destroy();
}

void AMobyGameCharacter::ResetSpeed(float InSpeed)
{
	if (GetWorld()->IsServer())
	{
		if (FCharacterAttribute* OriginalData = GetCharacterAttribute())
		{
			if (InSpeed == INDEX_NONE)
			{
				GetCharacterMovement()->MaxWalkSpeed = OriginalData->WalkSpeed;
			}
			else
			{
				GetCharacterMovement()->MaxWalkSpeed = InSpeed;
			}
		}
	}
}

FVector AMobyGameCharacter::GetFirePointLocation()
{
	return OpenFirePoint->GetComponentLocation();
}

FRotator AMobyGameCharacter::GetFirePointRotation()
{
	return OpenFirePoint->GetComponentRotation();
}

void AMobyGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMobyGameCharacter, TeamType, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AMobyGameCharacter, PlayerName, COND_SimulatedOnly);
}

bool AMobyGameCharacter::IsDie()
{
	if (FCharacterAttribute* InAttribute = GetCharacterAttribute())
	{
		if (InAttribute->Health <= 0.f)
		{
			return true;
		}
	}

	
	return false;
}

void AMobyGameCharacter::SetTeam(ETeamType InTeamType)
{
	TeamType = InTeamType;
}

void AMobyGameCharacter::SetCharacterType(ECharacterType InCharacterType)
{
	CharacterType = InCharacterType;
}

void AMobyGameCharacter::LockRot(bool bInLockRot)
{
	bLockRot = bInLockRot;
}

void AMobyGameCharacter::ResetCurrentRot()
{
	CurrentRot = GetActorRotation();
}

//此动画在客户端视角范围内才会同步
void AMobyGameCharacter::MultiCastPlayerAnimMontage_Implementation(UAnimMontage* InMontage, float InPlayRate /*= 1.0f*/, FName StartSectionName /*= NAME_None*/)
{
	if (InMontage)
	{
		PlayAnimMontage(InMontage, InPlayRate, StartSectionName);
	}
}

void AMobyGameCharacter::MultiCastPlayerStopAnimMontage_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		StopAnimMontage();
	}
}

void AMobyGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	//产生一个Controller，因为CharacterMoveToOnServer函数要获得MobyGameCharacter的Controller(MobyPawn.cpp)
	if (GetLocalRole() == ROLE_Authority)
	{
		SpawnDefaultController();
	}
	else
	{
		//染色 获取的gamestate是通过firstplayercontroller获取的，获取的是主机玩家的PlayerState
		//主机玩家先产生，因此调用这段代码时一定可以获取PlayerState，可以正常染色

		GThread::Get()->GetCoroutines().BindLambda(2.f, [&]()
		{
			if (AMobyGamePlayerState* InPlayerState = MethodUnit::GetPlayerState(GetWorld()))
			{
				if (UUI_HealthBar* InHealthBar = Cast<UUI_HealthBar>(Widget->GetUserWidgetObject()))
				{
					//InPlayerState->GetPlayerData()->TeamType是主机玩家，每个角色和主机玩家的队伍做比较
					if (InPlayerState->GetPlayerData()->Team == TeamType)
					{
						InHealthBar->SetColor(FColor::Green);

					}
					else if (TeamType == ETeamType::TEAM_NEUTRAL)
					{
						InHealthBar->SetColor(FColor::Orange);
					}
					else
					{
						InHealthBar->SetColor(FColor::Red);
					}
				}
			}
		});
	
	}
}

void AMobyGameCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetLocalRole() != ENetRole::ROLE_Authority)
	{
		if (UUI_HealthBar* InHealthBar = Cast<UUI_HealthBar>(Widget->GetUserWidgetObject()))
		{
			InHealthBar->RemoveDelegate();
		}
	}
}

AMobyGamePlayerState* AMobyGameCharacter::GetPlayerState()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		return MethodUnit::GetPlayerState(GetWorld(), PlayerID);
	}

	return MethodUnit::GetPlayerState(GetWorld());
}