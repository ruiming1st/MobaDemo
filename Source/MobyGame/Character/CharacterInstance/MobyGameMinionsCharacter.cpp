// Fill out your copyright notice in the Description page of Project Settings.

#include "MobyGameMinionsCharacter.h"
#include "ThreadManage.h"

AMobyGameMinionsCharacter::AMobyGameMinionsCharacter()
	:Super()
{
	WidgetTime = 0.f;
}

// Called when the game starts or when spawned
void AMobyGameMinionsCharacter::BeginPlay()
{
	Super::BeginPlay();

	//将我们的小兵的血条隐藏起来
	SetWidgetVisibility(false);
}

// Called every frame.
void AMobyGameMinionsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() != ROLE_Authority)
	{
		if (WidgetTime > 0.f)
		{
			WidgetTime -= DeltaSeconds;
		}
		else
		{
			SetWidgetVisibility(false);
		}
	}
}

void AMobyGameMinionsCharacter::Hurt(bool bDie)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		if (!bDie)
		{
			SetWidgetVisibility(true);
			WidgetTime = 40.f;
		}
		else
		{
			WidgetTime = 0.f;
		}
	}
}
