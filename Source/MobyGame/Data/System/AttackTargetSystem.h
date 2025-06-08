// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class AMobyGameCharacter;
class UParticleSystemComponent;
class UParticleSystem;

struct FAttackTarget
{
	FAttackTarget(const FVector& InLocation, AMobyGameCharacter* InTaget);
	~FAttackTarget();

	//计算射线
	void Tick(float DeltaSeconds);
	bool IsValid()const;
	bool IsValidAttackTarget() const;

	TWeakObjectPtr<AMobyGameCharacter> Target;
	FVector Point;

	FGuid ID;
	TWeakObjectPtr<UParticleSystemComponent> ParticleSystem;
};

//移除的时候 remove函数需要用到==操作符比较对象
FORCEINLINE bool operator==(const FAttackTarget& R, const FAttackTarget& L)
{
	return R.ID == L.ID;
}

struct FAttackTargetSystem
{
	typedef TFunction<void(AMobyGameCharacter*)> TFun;
	FAttackTargetSystem();
	~FAttackTargetSystem();

	//公共
public:
	void Init(UWorld *InNewWorld, AMobyGameCharacter* Instigator);
	AMobyGameCharacter* GetInstigator() {return Owner.Get();}
	FORCEINLINE UWorld* GetWorld() {return World;	}
	TFun AddFuntion;
	TFun RemoveFuntion;

	//Server
public:
	void ServerTick(float DeltaSeconds);
	bool IsInRange(AMobyGameCharacter* InCharacter);

	//Client
public:
	void ClientTick(float DeltaSeconds);
	UParticleSystemComponent* SpawnParticle(const FVector& InLocation);
	void ClientAdd(AMobyGameCharacter* InCharacter);
	void ClientRemove(AMobyGameCharacter* InCharacter);

private:
	TArray<FAttackTarget> Targets;
	static UParticleSystem* ParticleSystemInstance;

	float RefreshValue;
	UWorld* World;
	TWeakObjectPtr<AMobyGameCharacter> Owner;
};