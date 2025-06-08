// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/CoreSystem.h"

class ACharacterSpawnPoint;

struct FSpawnMinionsTimer
{
	typedef TFunction<void()> TFunc;
	FSpawnMinionsTimer();

	void Tick(float DeltaSeconds);

	TFunc Func;

	void Reset();
private:
	float Time;
	uint8 Count;
};

//小兵生成器 成就系统
struct FMinionsSystem :public FCoreSystem
{
	FMinionsSystem();

	void Tick(float DeltaSeconds);
	void Init(const TArray<ACharacterSpawnPoint*> &InGameCharacters);

protected:
	void SpawnWarrior();
	void SpawnRemote();
	void SpawnBig();
	void SpawnSuper(ETeamType InType);

private:

	FSpawnMinionsTimer Normal;
	FSpawnMinionsTimer SuperRed;
	FSpawnMinionsTimer SuperBlue;

	TArray<FVector> RedLocations;
	TArray<FVector> BlueLocations;

};