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

	//��������
	void Tick(float DeltaSeconds);
	bool IsValid()const;
	bool IsValidAttackTarget() const;

	TWeakObjectPtr<AMobyGameCharacter> Target;
	FVector Point;

	FGuid ID;
	TWeakObjectPtr<UParticleSystemComponent> ParticleSystem;
};

//�Ƴ���ʱ�� remove������Ҫ�õ�==�������Ƚ϶���
FORCEINLINE bool operator==(const FAttackTarget& R, const FAttackTarget& L)
{
	return R.ID == L.ID;
}

struct FAttackTargetSystem
{
	typedef TFunction<void(AMobyGameCharacter*)> TFun;
	FAttackTargetSystem();
	~FAttackTargetSystem();

	//����
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