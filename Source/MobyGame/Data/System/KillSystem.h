// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KillSystem.generated.h"


//��ɱ
USTRUCT(BlueprintType)
struct FKillSystem
{
	GENERATED_USTRUCT_BODY()

	typedef TFunction<void(const int64&, const int64&)> TKillFunction;

	//ִ������ҵ����ʱ��
	void AddKiller(const int64& InPlayerID);
	bool IsExistPlayer(const int64& InPlayerID);
	void Tick(float DeltaSeconds);
	void Kill(const int64& InKillerPlayer, const int64& InKilledPlayer);
	void Death(const int64& InKilledPlayer);

private:
	void AddKiller(const int64& InKillerPlayer, const int64& InKilledPlayer);
public:
	FKillSystem();
	//��¼����ɾ�
	struct FKillModel
	{
		FKillModel(const int64& InKiller);
		int32 KillNumber;
		const int64 Killer;
		int64 Killed;
	};

	//��¼������ɱ
	struct FKill
	{
		FKill(const int64& InKillerPlayer, const int64& InKilledPlayer);

		void ResetTime();

		int64 CurrentKiller;
		int64 CurrentKilled;
		int64 KillNumber;
		float KillTime;
	};
public:
	TArray<FKillModel> Players;
	TArray<FKill> CurrentKillState;
	//�ɾ�
public:
	int64 Blood;
	TArray<int64> DoubleKills;
	TArray<int64> TripleKills;
	TArray<int64> QuadraKills;
	TArray<int64> PentaKills;
	TArray<int64> Invincible;
	TArray<int64> Legendary;

public:
	TKillFunction AceFunction;
	TKillFunction BloodFunction;
	TKillFunction KillFunction;
	TKillFunction DoubleKillsFunction;
	TKillFunction TripleKillsFunction;
	TKillFunction QuadraKillsFunction;
	TKillFunction PentaKillsFunction;
	TKillFunction InvincibleFunction;
	TKillFunction LegendaryFunction;
};


FORCEINLINE bool operator==(const FKillSystem::FKill& Lhs, const int64& Rhs)
{
	return Lhs.CurrentKiller == Rhs;
}


FORCEINLINE bool operator==(const FKillSystem::FKill& Lhs, const FKillSystem::FKill& Rhs)
{
	return Rhs.CurrentKiller == Lhs.CurrentKiller;
}

FORCEINLINE bool operator==(const FKillSystem::FKillModel Lhs, const FKillSystem::FKillModel& Rhs)
{
	return Rhs.Killer == Lhs.Killer;
}