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

	//改变读取路径
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
		//运行子系统
		InGameState->KillSystem.Tick(DeltaTime);
		//小兵生成器系统
		InGameState->MinionsSystem.Tick(DeltaTime);
		//怪物生成器系统
		InGameState->MonsterSystem.Tick(DeltaTime);
		//玩家生成器
		InGameState->PlayerSystem.Tick(DeltaTime);
		//塔生成器
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
				//需要去拉DB服务器的数据
				//...
				//赋值ID
				InPlayerState->ServerCallClientInitPlayerData(InPlayerState->GetPlayerData()->PlayerID);

				InGameState->KillSystem.AddKiller(InData->PlayerID);

				//断线重连
				//遍历场景内有没有和我们的ID是一样的角色 如果有就控制它
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
						//服务端视口位置
						InPawn->SetActorLocation(InPawn->MobyGamePlayerCharacter->GetActorLocation());

						//让镜头跟着角色
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

					//更新血池位置 更新商店位置
					InPlayerState->UpdateSpawnPointLocation();
				}
			}
		}
	}
}

