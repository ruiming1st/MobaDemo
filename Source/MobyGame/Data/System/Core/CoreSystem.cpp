#include "CoreSystem.h"
#include "../../../Character/CharacterSpawnPoint.h"
#include "../../../Common/MethodUnit.h"

FCoreSystem::FCoreSystem()
	: FGeneratorSystem()
	, CurrentLv(1)
{

}

void FCoreSystem::Tick(float DeltaSeconds)
{
	if (CurrentLv < 18)//×îÖÕÏÞÖÆ
	{
		if (GetWorld())
		{
			if (GetWorld()->GetRealTimeSeconds() >= 60 * 3.f)
			{
				CurrentLv = 4;
			}
			else if (GetWorld()->GetRealTimeSeconds() >= 60 * 10.f)
			{
				CurrentLv = 8;
			}
			else if (GetWorld()->GetRealTimeSeconds() >= 60 * 18.f)
			{
				CurrentLv = 12;
			}
			else if (GetWorld()->GetRealTimeSeconds() >= 60 * 25.f)
			{
				CurrentLv = 14;
			}
			else if (GetWorld()->GetRealTimeSeconds() >= 60 * 30.f)
			{
				CurrentLv = 18;
			}
		}
	}
}