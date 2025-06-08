#pragma once

#include "CoreMinimal.h"


class ACharacterSpawnPoint;
class AMobyGameCharacter;

struct FGeneratorSystem
{
	typedef FGeneratorSystem Super;

	FGeneratorSystem();
	virtual ~FGeneratorSystem() {}

	virtual void Tick(float DeltaSeconds);

public:
	void SetWorld(UWorld* InWorld);
	FORCEINLINE UWorld* GetWorld() { return World; }

protected:
	AMobyGameCharacter* Spawn(int64 InPlayerID, int32 InCharacterID, const FVector& InLocation, ETeamType Type, int32 InCurrentLv = 1);

private:
	UWorld* World;

	int32 CurrentLv;
};