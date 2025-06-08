#pragma once

#include "CoreMinimal.h"
#include "GeneratorSystem.h"

class ACharacterSpawnPoint;
class AMobyGameCharacter;

//小兵生成器 成就系统
struct FCoreSystem :public FGeneratorSystem
{
	typedef FCoreSystem Super;

	FCoreSystem();

	virtual void Tick(float DeltaSeconds);

	FORCEINLINE int32 GetCurrentLv()const { return CurrentLv; }
private:
	int32 CurrentLv;
};