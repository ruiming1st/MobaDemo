// Fill out your copyright notice in the Description page of Project Settings.
#include "MobyGameArchitectureCharacter.h"

AMobyGameArchitectureCharacter::AMobyGameArchitectureCharacter()
	:Super()
	, bPossibleAttack(false)
{

}

void AMobyGameArchitectureCharacter::SetPossibleAttack(bool bNewPossibleAttack)
{
	bPossibleAttack = bNewPossibleAttack;

	//血条的显示 广播 是否显示血条
	SetClientWidgetVisibility(bNewPossibleAttack);
}

void AMobyGameArchitectureCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() != ROLE_Authority)
	{
		SetWidgetVisibility(false);
	}
}

float AMobyGameArchitectureCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bPossibleAttack)
	{
		return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}

	return 0.f;
}

void AMobyGameArchitectureCharacter::Die()
{
	Super::Die();

	if (AMobyGameState *InGameState = GetWorld()->GetGameState<AMobyGameState>())
	{
		InGameState->RemoveTurret(this, GetTeam());
	}
}