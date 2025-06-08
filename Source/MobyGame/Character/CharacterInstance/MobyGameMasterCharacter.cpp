// Fill out your copyright notice in the Description page of Project Settings.

#include "MobyGameMasterCharacter.h"

AMobyGameMasterCharacter::AMobyGameMasterCharacter()
{

}

void AMobyGameMasterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AMobyGameMasterCharacter::InitCharacter()
{
	Super::InitCharacter();

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Instigator = Cast<APawn>(this);
	GetWorld()->SpawnActor<ABullet>(Bullet, GetFirePointLocation(), GetActorRotation(), ActorSpawnParameters);
}
