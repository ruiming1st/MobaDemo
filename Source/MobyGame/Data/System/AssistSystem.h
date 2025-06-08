// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssistSystem.generated.h"

//Öú¹¥
USTRUCT(BlueprintType)
struct FAssistPlayer
{
	GENERATED_USTRUCT_BODY()

	FAssistPlayer();

	UPROPERTY()
	int64 PlayerID;

	UPROPERTY()
	float AssistTime;
};

USTRUCT(BlueprintType)
struct FAssistSystem
{
	GENERATED_USTRUCT_BODY()

public:
	void Tick(float DeltaSeconds);
	const TArray<FAssistPlayer> &GetAssists() { return Assists; }
	void AddAssist(const int64& InPlayerID);
	const FAssistPlayer* GetLastAssist();

private:
	TArray<FAssistPlayer> Assists;
};

FORCEINLINE bool operator==(const FAssistPlayer& Lhs, const FAssistPlayer& Rhs)
{
	return Rhs.PlayerID == Lhs.PlayerID;
}
