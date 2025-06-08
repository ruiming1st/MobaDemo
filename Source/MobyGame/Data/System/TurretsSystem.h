// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/GeneratorSystem.h"

//水晶
//塔类

class ACharacterSpawnPoint;
class AMobyGameArchitectureCharacter;

enum class ETurretDeathType : uint8
{
	TURRET_DIE,
	TURRET_NO_DIE,
	TURRET_NO_SET,
	TURRET_ERROR,
};

//塔
struct FTurret
{
	FTurret();

	ECharacterType Type;
	FVector SpawnPoint;
	TWeakObjectPtr<AMobyGameArchitectureCharacter> TurretsWeakPtr;
	float Time;

	bool IsValid() const; //判断这个塔是否有意义
};

FORCEINLINE bool operator==(const FTurret& Lhs, const FTurret& Rhs)
{
	return Rhs.TurretsWeakPtr == Lhs.TurretsWeakPtr;
}

//水晶 会复活
struct FCrystal :public FTurret
{
	FCrystal();

	float SpawnTime;
	float MaxSpawnTime;
};

//路
struct FTurrets
{
	FTurrets();

	//塔
	TArray<FTurret> Turrets;

	//高地水晶
	FCrystal Crystal;
};

//队伍
struct FTurretsGroup
{
	FTurretsGroup();

	//上
	FTurrets UP;
	//中
	FTurrets Mid;
	//下
	FTurrets Down;

	//主水晶
	FCrystal MainCrystal;

	//最强塔
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
	bool CheckTurretDeath(FTurrets* InTurret);//只是判断塔 不包含水晶
	ETurretDeathType CheckTurretsDeath(FTurrets* InTurret);//判断塔 水晶 是不是死了

	void CheckCrystalLimit(FTurretsGroup &InGroup);
	void CheckMainCrystalLimit(FTurretsGroup& InGroup);
	bool CheckGroupDeath(FTurretsGroup& InGroup);

	//最远的塔排在最前面 最近的塔排在最后面
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
