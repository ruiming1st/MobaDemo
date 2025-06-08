// Fill out your copyright notice in the Description page of Project Settings.


#include "MobyGameTurretsCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../AI/AIController/Core/MobyGameAIController.h"
#include "../AI/AIController/MinionsAIController.h"

AMobyGameTurretsCharacter::AMobyGameTurretsCharacter()
{
	TowersRotator = FVector2D::ZeroVector;
}

void AMobyGameTurretsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMobyGameTurretsCharacter, TowersRotator);
}


const TArray<FAssistPlayer>& AMobyGameTurretsCharacter::GetAssists()
{
	return AssistSystem.GetAssists();
}

void AMobyGameTurretsCharacter::AddAssist(const int64& InPlayerID)
{
	AssistSystem.AddAssist(InPlayerID);
}

const FAssistPlayer* AMobyGameTurretsCharacter::GetLastAssist()
{
	return AssistSystem.GetLastAssist();
}

void AMobyGameTurretsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AssistSystem.Tick(DeltaSeconds);
}

