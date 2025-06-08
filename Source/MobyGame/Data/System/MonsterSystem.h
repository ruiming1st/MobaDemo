// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/CoreSystem.h"

class ACharacterSpawnPoint;
class AMobyGameCharacter;

struct FMonster
{
	FMonster();

	FVector SpawnPoint;
	TWeakObjectPtr<AMobyGameCharacter> MonsterWeakPtr;
};

struct FMonsterGroup
{
	FMonsterGroup();

	ECharacterType Type;
	bool bDieGroup;
	float SpawnTime;
	float MaxSpawnTime;
	TArray<FMonster>Monster;
};

//MonsterÉú³ÉÆ÷ 
struct FMonsterSystem :public FCoreSystem
{
	FMonsterSystem();

	void Tick(float DeltaSeconds);

	void Init(const TArray<ACharacterSpawnPoint*>& InGameCharacters);

	void SpawnMonster(FMonsterGroup & Instance);

	void AllocationGroup(ACharacterSpawnPoint *InPoint);
protected:
	TArray<FMonsterGroup> Group;
};