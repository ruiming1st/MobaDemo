#pragma once

#include "CoreMinimal.h"
#include "GeneratorSystem.h"

class ACharacterSpawnPoint;
class AMobyGameCharacter;

//С�������� �ɾ�ϵͳ
struct FCoreSystem :public FGeneratorSystem
{
	typedef FCoreSystem Super;

	FCoreSystem();

	virtual void Tick(float DeltaSeconds);

	FORCEINLINE int32 GetCurrentLv()const { return CurrentLv; }
private:
	int32 CurrentLv;
};