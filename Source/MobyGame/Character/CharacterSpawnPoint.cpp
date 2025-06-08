// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSpawnPoint.h"

ACharacterSpawnPoint::ACharacterSpawnPoint()
{
	bNetLoadOnClient = false;
	SetReplicates(false);
	SpawnTime = 30.f;
}
