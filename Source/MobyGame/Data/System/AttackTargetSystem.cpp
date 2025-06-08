#include "AttackTargetSystem.h"
#include "Kismet/GameplayStatics.h"
#include "../../Character/CharacterInstance/MobyGameCharacter.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "../../Character/Damage/BulletDamageType.h"

UParticleSystem* FAttackTargetSystem::ParticleSystemInstance = NULL;

FAttackTarget::FAttackTarget(const FVector& InLocation, AMobyGameCharacter* InTaget)
	:Point(InLocation)
	, ID(FGuid::NewGuid())
	, Target(InTaget)
	, ParticleSystem(NULL)
{

}

//申请资源，务必释放
FAttackTarget::~FAttackTarget()
{
	if (ParticleSystem.IsValid())
	{
		ParticleSystem->DestroyComponent();
	}
}

FAttackTargetSystem::FAttackTargetSystem()
	:RefreshValue(0.f)
	, World(NULL)
{
	if (!ParticleSystemInstance)
	{
		//从磁盘读取特效
		ParticleSystemInstance = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(),
			NULL, TEXT("ParticleSystem'/Game/Turrets/BP/Line.Line'")));
	}
}

FAttackTargetSystem::~FAttackTargetSystem()
{

}
void FAttackTarget::Tick(float DeltaSeconds)
{
	if (ParticleSystem.IsValid())
	{
		if (Target.IsValid())
		{
			if (!Target->IsDie())
			{
				ParticleSystem->SetBeamSourcePoint(0, Point, 0);
				ParticleSystem->SetBeamTargetPoint(0, Target->GetActorLocation(), 0);
			}
		}
	}
}

bool FAttackTarget::IsValid() const
{

	return false;
}

bool FAttackTarget::IsValidAttackTarget() const
{
	return Point != FVector::ZeroVector;
}

void FAttackTargetSystem::Init(UWorld* InNewWorld, AMobyGameCharacter* Instigator)
{
	World = InNewWorld;
	Owner = Instigator;
}

void FAttackTargetSystem::ServerTick(float DeltaSeconds)
{
	if (GetInstigator())
	{
		RefreshValue += DeltaSeconds;
		if (RefreshValue >= 1.0f)
		{
			//范围检测
			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMobyGameCharacter::StaticClass(), OutActors);
			
			//添加
			for (auto& Tmp : OutActors)
			{
				if (Tmp != GetInstigator())
				{
					if (AMobyGameCharacter* InCharacter = Cast<AMobyGameCharacter>(Tmp))
					{
						if (!InCharacter->IsDie())
						{
							//队伍不一样
							if (GetInstigator()->GetTeam() != InCharacter->GetTeam())
							{
								float Distance = FVector::Dist(Tmp->GetActorLocation(), GetInstigator()->GetActorLocation());
								if (Distance < 1000.f)
								{
									if (!IsInRange(InCharacter))
									{
										Targets.Add(FAttackTarget(GetInstigator()->GetActorLocation(), InCharacter));
										AddFuntion(InCharacter);
									}
								}
							}
						}
					}
				}
			}

			//给对方的伤害
			for (auto& Tmp : Targets)
			{
				if (Tmp.Target.IsValid())
				{
					if (!Tmp.Target->IsDie())
					{
						UGameplayStatics::ApplyDamage(
							Tmp.Target.Get(),
							250.f,
							GetInstigator()->GetController(),
							GetInstigator(),
							UBulletDamageType::StaticClass());
					}
				}
			}
			
			//获取离开或者死亡的 敌方玩家
			TArray<FAttackTarget> RemoveObject;
			for (auto &Tmp : Targets)
			{
				if (Tmp.Target.IsValid())
				{
					if (!Tmp.Target->IsDie())
					{
						float Distance = FVector::Dist(Tmp.Target->GetActorLocation(), GetInstigator()->GetActorLocation());
						if (Distance > 1000.f)
						{
							RemoveObject.Add(Tmp);
						}
					}
					else
					{
						RemoveObject.Add(Tmp);
					}
				}
				else
				{
					RemoveObject.Add(Tmp);
				}
			}

			//移除
			for (auto &Tmp : RemoveObject)
			{
				Targets.Remove(Tmp);
				RemoveFuntion(Tmp.Target.Get());
			}
		}
	}
}

bool FAttackTargetSystem::IsInRange(AMobyGameCharacter* InCharacter)
{
	for (auto& Tmp : Targets)
	{
		if (Tmp.Target == InCharacter)
		{
			return true;
		}
	}

	return false;
}

void FAttackTargetSystem::ClientTick(float DeltaSeconds)
{
	for (auto& Tmp : Targets)
	{
		Tmp.Tick(DeltaSeconds);
	}
}

UParticleSystemComponent* FAttackTargetSystem::SpawnParticle(const FVector& InLocation)
{
	return UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ParticleSystemInstance, InLocation);
}

//产生激光射线
void FAttackTargetSystem::ClientAdd(AMobyGameCharacter* InCharacter)
{
	if (Owner.IsValid() && InCharacter)
	{
		Targets.Add(FAttackTarget(Owner->GetActorLocation(), InCharacter));
		FAttackTarget &NewTarget = Targets.Last();

		NewTarget.ParticleSystem = SpawnParticle(Owner->GetActorLocation());
	}
}

void FAttackTargetSystem::ClientRemove(AMobyGameCharacter* InCharacter)
{
	FAttackTarget InRemoveTarget(FVector::ZeroVector, NULL);

	for (auto &Tmp :Targets)
	{
		if (Tmp.Target == InCharacter)
		{
			InRemoveTarget = Tmp;
			break;
		}
	}

	if (InRemoveTarget.IsValidAttackTarget())
	{
		Targets.Remove(InRemoveTarget);
	}
}
