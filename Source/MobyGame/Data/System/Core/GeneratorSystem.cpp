#include "GeneratorSystem.h"
#include "../../../Character/CharacterSpawnPoint.h"
#include "../../../Common/MethodUnit.h"

FGeneratorSystem::FGeneratorSystem()
	: World(NULL)
{

}



void FGeneratorSystem::Tick(float DeltaSeconds)
{

}

void FGeneratorSystem::SetWorld(UWorld* InWorld)
{
	World = InWorld;
}

AMobyGameCharacter* FGeneratorSystem::Spawn(int64 InPlayerID, int32 InCharacterID, const FVector& InLocation, ETeamType Type, int32 InCurrentLv)
{
	AMobyGameCharacter* MobyGameCharacter = NULL;
	if (AMobyGameState* InGameState = MethodUnit::GetGameState(GetWorld()))
	{
		const FCharacterTable* InTable = InGameState->GetCharacterTableTemplate(InCharacterID);
		if (InTable)
		{
			if (InTable->CharacterClass)
			{
				MobyGameCharacter = GetWorld()->SpawnActor<AMobyGameCharacter>(InTable->CharacterClass, InLocation, FRotator::ZeroRotator);
				if (MobyGameCharacter)
				{
					MobyGameCharacter->RegisterCharacter(InPlayerID, InCharacterID, FName());
					MobyGameCharacter->SetTeam(Type);
					MobyGameCharacter->SetCharacterType(InTable->CharacterType);
					MobyGameCharacter->MultiCastRegisterCharacter(InPlayerID);

					if (FCharacterAttribute* InCharacterAttribute = InGameState->GetCharacterAttribute(InPlayerID))
					{
						for (int32 i = 1; i < InCurrentLv; i++)
						{
							InCharacterAttribute->UpdateLevel();
						}
					}
				}
			}
		}
	}

	return MobyGameCharacter;
}
