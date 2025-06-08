// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "../MobyGameType.h"
#include "CharacterSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API ACharacterSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterType")
	ETeamType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterType")
	ECharacterType CharacterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterType")
	ERoadType RoadType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterType")
	float SpawnTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterType")
	FName PlayerName;
public:
	ACharacterSpawnPoint();

	FORCEINLINE FName GetPlayerName() const { return PlayerName; }
	FORCEINLINE float GetSpawnTime() const { return SpawnTime; }
	FORCEINLINE ERoadType GetRoadType() const { return RoadType; }
	FORCEINLINE ETeamType GetType() const { return Type; }
	FORCEINLINE ECharacterType GetCharacterType() const { return CharacterType; }

};
