// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/GeneratorSystem.h"

class ACharacterSpawnPoint;

enum class EChangeTypeTime :uint8
{
	STAGE_I,
	STAGE_II,
	STAGE_III,
	STAGE_IV,
	STAGE_V,
	STAGE_VI,
};

struct FPlayerCharacter
{
	FPlayerCharacter();

	FVector SpawnPoint;
	TWeakObjectPtr<AMobyGameCharacter> Character;
	FName PlayerName;
	float Time;
	float MaxTime;

	void ResetDeathTime();
};

struct FPlayerCharacterGroup
{
	FPlayerCharacterGroup();

	TArray<FPlayerCharacter> Characters;
	EChangeTypeTime ChangeTimeType;
};

struct FPlayerSystem :public FGeneratorSystem
{
	FPlayerSystem();

	virtual void Tick(float DeltaSeconds);

	void Init(const TArray<ACharacterSpawnPoint*>& InGameCharacters);

	float GetRebornTime(const AMobyGameCharacter* InCharacter);

	AMobyGameCharacter* SpawnCharacter(int64 InPlayerID, int32 InCharacterID, ETeamType Type);
protected:
	AMobyGameCharacter* Spawn(int64 InPlayerID, int32 InCharacterID, const FVector& InLocation, ETeamType Type, FName& InName, int32 InCurrentLv = 1);

	void CheckGroup(FPlayerCharacterGroup& InGroup, float DeltaSeconds);
	AMobyGameCharacter* SpawnCharacterInGroup(FPlayerCharacterGroup& InGroup, int64 InPlayerID, int32 InCharacterID, ETeamType InTeam);
	void ChangeTime(FPlayerCharacterGroup& InGroup, float InNewTime);
protected:
	FPlayerCharacterGroup Red;
	FPlayerCharacterGroup Blue;
};