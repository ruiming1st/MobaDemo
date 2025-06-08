// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDataComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerDataComponent::UPlayerDataComponent()
{
	PlayerName = "Sakura";
	Team = ETeamType::TEAM_RED;
	PlayerID = FMath::RandRange(1000000, 888889999);
	Gold = 5500;
	Kill = 0;
	Killed = 0;
	Assists = 0;
	KillDogFace = 0;
	SkillPoint = 0;
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// bReplicates = true;
	SetIsReplicatedByDefault(true);
	// ...

	//创建实例
	SlotAttributes = MakeShareable(new FSlotAttribute_Interior());
}


// Called when the game starts
void UPlayerDataComponent::BeginPlay()
{
	Super::BeginPlay();
	FString NumberString;
	TArray<FString> InfoTest;
	FFileHelper::LoadFileToStringArray(InfoTest, *(FPaths::ProjectDir() / TEXT("PlayerInfo.txt")));
	
	if (InfoTest.Num() == 2)
	{
		PlayerName = *InfoTest[0];
		Team = (ETeamType)FCString::Atoi(*InfoTest[0]);
	}
}


void UPlayerDataComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//只同步给自己
	DOREPLIFETIME_CONDITION(UPlayerDataComponent, Gold, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPlayerDataComponent, Team, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(UPlayerDataComponent, PlayerID, COND_OwnerOnly);
}

// Called every frame
void UPlayerDataComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FString UPlayerDataComponent::GetPlayerName()
{
	return PlayerName.ToString();
}

