// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/GeneratorSystem.h"

//ˮ��
//����

class ACharacterSpawnPoint;
class AMobyGameArchitectureCharacter;

enum class ETurretDeathType : uint8
{
	TURRET_DIE,
	TURRET_NO_DIE,
	TURRET_NO_SET,
	TURRET_ERROR,
};

//��
struct FTurret
{
	FTurret();

	ECharacterType Type;
	FVector SpawnPoint;
	TWeakObjectPtr<AMobyGameArchitectureCharacter> TurretsWeakPtr;
	float Time;

	bool IsValid() const; //�ж�������Ƿ�������
};

FORCEINLINE bool operator==(const FTurret& Lhs, const FTurret& Rhs)
{
	return Rhs.TurretsWeakPtr == Lhs.TurretsWeakPtr;
}

//ˮ�� �Ḵ��
struct FCrystal :public FTurret
{
	FCrystal();

	float SpawnTime;
	float MaxSpawnTime;
};

//·
struct FTurrets
{
	FTurrets();

	//��
	TArray<FTurret> Turrets;

	//�ߵ�ˮ��
	FCrystal Crystal;
};

//����
struct FTurretsGroup
{
	FTurretsGroup();

	//��
	FTurrets UP;
	//��
	FTurrets Mid;
	//��
	FTurrets Down;

	//��ˮ��
	FCrystal MainCrystal;

	//��ǿ��
	FTurret Eye;
};

struct FTurretsSystem :public FCoreSystem
{
	typedef FCoreSystem Super;

	FTurretsSystem();

	virtual void Tick(float DeltaSeconds);
	void Init(const TArray<ACharacterSpawnPoint*>& InGameCharacters);

	void RemoveTurret(TWeakObjectPtr<AMobyGameArchitectureCharacter> InTurretsWeakPtr, const ETeamType InType);
protected:
	void AllocationMainCrystalGroup(ACharacterSpawnPoint* InPoint, FTurretsGroup& TG);
	void AllocationCrystalGroup(ACharacterSpawnPoint* InPoint, FTurretsGroup& TG);
	void AllocationTurretsGroup(ACharacterSpawnPoint* InPoint, FTurretsGroup& TG);
	void AllocationEyeGroup(ACharacterSpawnPoint* InPoint, FTurretsGroup& TG);
private:
	AMobyGameArchitectureCharacter *SpawnTurret(FTurret *InTurret, const ETeamType InType, int32 InCharacterID);
	void SpawnGroup(FTurretsGroup& InGroup, const ETeamType InType);
	void SpawnTurrets(FTurrets& InGroup, const ETeamType InType);

	void CheckGroup(FTurretsGroup &InGroup, float DeltaSeconds, const ETeamType InType);

	void CheckGroupLv(FTurretsGroup& InGroup, float DeltaSeconds, const ETeamType InType);

	void CheckRecovery(FTurret* InTurret, float DeltaSeconds);
	bool CheckDeath(FTurrets* InTurret, float DeltaSeconds, int32 InCharacterID, const ETeamType InType);
	bool CheckMainCrystalDeath(FCrystal* InTurret);
	bool CheckTurretDeath(FTurrets* InTurret);//ֻ���ж��� ������ˮ��
	ETurretDeathType CheckTurretsDeath(FTurrets* InTurret);//�ж��� ˮ�� �ǲ�������

	void CheckCrystalLimit(FTurretsGroup &InGroup);
	void CheckMainCrystalLimit(FTurretsGroup& InGroup);
	bool CheckGroupDeath(FTurretsGroup& InGroup);

	//��Զ����������ǰ�� ����������������
	void Sorting(TArray<FTurret>& Turrets, const FVector& InLocation);
	void RecursiveSorting(const FVector& InLocation, TArray<FTurret>& InTurrets, TArray<FTurret>& Turrets);
private:
	void SetPossibleAttack(FTurret* InTurret, bool bPossibleAttack);
	void SetTurretPossibleAttack(FTurrets* InTurret, bool bPossibleAttack);
	void SetPossibleAttackCrystal(FTurret* InTurret, bool bComparisonValue, bool bValue);
private:
	void Allocation(ACharacterSpawnPoint* InPoint, FTurret *InSlot);

protected:
	FTurretsGroup Red;
	FTurretsGroup Blue;

	bool bInitialize;
};
