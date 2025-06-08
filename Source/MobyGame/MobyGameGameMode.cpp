// Copyright Epic Games, Inc. All Rights Reserved.

#include "MobyGameGameMode.h"
#include "MobyGamePlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "MobyGameState.h"
#include "MobyPawn.h"
#include "MobyGameHUD.h"
#include "MobyGamePlayerState.h"
#include "Character/CharacterInstance/MobyGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Character/CharacterSpawnPoint.h"
#include "Common/MethodUnit.h"
#include "ThreadManage.h"
#include "Character/CharacterInstance/MobyGameMonsterCharacter.h"
#include "Character/AI/AIController/MonsterAIController.h"

AMobyGameGameMode::AMobyGameGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// use our custom PlayerController class
	PlayerControllerClass = AMobyGamePlayerController::StaticClass();
	// set default pawn class to our Blueprinted character
	GameStateClass = AMobyGameState::StaticClass();
	PlayerStateClass = AMobyGamePlayerState::StaticClass();
	HUDClass = AMobyGameHUD::StaticClass();

	//�ı��ȡ·��
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/MobyPlayer"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}

void AMobyGameGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AMobyGameGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MethodUnit::ServerCallAllPlayerState<AMobyGamePlayerState>(GetWorld(),
	[&](AMobyGamePlayerState* InPlayerState)->MethodUnit::EServerCallType
	{
		InPlayerState->TickData(DeltaTime);
		return MethodUnit::EServerCallType::INPROGRESS;
	});

	if (AMobyGameState* InGameState = GetGameState<AMobyGameState>())
	{
		//������ϵͳ
		InGameState->KillSystem.Tick(DeltaTime);
		//С��������ϵͳ
		InGameState->MinionsSystem.Tick(DeltaTime);
		//����������ϵͳ
		InGameState->MonsterSystem.Tick(DeltaTime);
		//���������
		InGameState->PlayerSystem.Tick(DeltaTime);
		//��������
		InGameState->TurretsSystem.Tick(DeltaTime);
	}
}

//#define Dbug_mb 1
//#if WITH_EDITOR
static bool Debug_Tema = false;
static bool Debug_Character = false;
//#endif
void AMobyGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (AMobyGameState* InGameState = GetGameState<AMobyGameState>())
	{
		if (AMobyGamePlayerState* InPlayerState = NewPlayer->GetPlayerState<AMobyGamePlayerState>())
		{
			if (UPlayerDataComponent* InData = InPlayerState->GetPlayerData())
			{
				//��Ҫȥ��DB������������
				//...
				//��ֵID
				InPlayerState->ServerCallClientInitPlayerData(InPlayerState->GetPlayerData()->PlayerID);

				InGameState->KillSystem.AddKiller(InData->PlayerID);

				//��������
				//������������û�к����ǵ�ID��һ���Ľ�ɫ ����оͿ�����
				//...
				
				if (AMobyPawn* InPawn = InPlayerState->GetPawn<AMobyPawn>())
				{
					TArray<FString> NumberString;
					FFileHelper::LoadFileToStringArray(NumberString, *(FPaths::ProjectDir() / TEXT("CharacterID.txt")));

					int32 CharacterID = FCString::Atoi(*NumberString[0]);
//#if WITH_EDITOR
					InPlayerState->GetPlayerData()->Team = (ETeamType)Debug_Tema; // FCString::Atoi(*NumberString[1]);
					if (Debug_Character)
					{
						CharacterID = 2;
					}
					else
					{
						CharacterID = 0;
					}

//#else
					//InPlayerState->GetPlayerData()->Team = (ETeamType)FCString::Atoi(*NumberString[1]);
//#endif
					if (InPlayerState->GetPlayerData()->Team == ETeamType::TEAM_RED)
					{
						InPlayerState->GetPlayerData()->PlayerName = "SakuraRed";
					}
					else
					{
						InPlayerState->GetPlayerData()->PlayerName = "SakuraBlue";
					}

					InPawn->MobyGamePlayerCharacter = Cast<AMobyGamePlayerCharacter>(InGameState->PlayerSystem.SpawnCharacter(
						InPlayerState->GetPlayerData()->PlayerID,
						CharacterID,
						InPlayerState->GetPlayerData()->Team));

					if (InPawn->MobyGamePlayerCharacter)
					{
						//������ӿ�λ��
						InPawn->SetActorLocation(InPawn->MobyGamePlayerCharacter->GetActorLocation());

						//�þ�ͷ���Ž�ɫ
						InPawn->ServerCallSetMobyGameCharacter(InPawn->MobyGamePlayerCharacter);
					}

//#if WITH_EDITOR
					if (Debug_Tema)
					{
						Debug_Tema = false;
					}
					else
					{
						Debug_Tema = true;
					}

					if (Debug_Character)
					{
						Debug_Character = false;
					}
					else
					{
						Debug_Character = true;
					}
//#endif

					//����Ѫ��λ�� �����̵�λ��
					InPlayerState->UpdateSpawnPointLocation();
				}
			}
		}
	}
}

