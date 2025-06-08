// Fill out your copyright notice in the Description page of Project Settings.
#include "UI_GoHome.h"
#include "Components/ProgressBar.h"
#include "MobyGame/Character/CharacterInstance/MobyGamePlayerCharacter.h"
#include "MobyGame/MobyPawn.h"

UUI_GoHome::UUI_GoHome(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	CountDown = 0.f;
	bStartCountdown = false;
}

void UUI_GoHome::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bStartCountdown)
	{
		CountDown += InDeltaTime;
		GoHomeBar->SetPercent(CountDown / 8.f);
		if (CountDown >= 8.f)
		{
			CancelGoingHome();

			//向服务器发送回家请求
			GoHome();
		}

		CheckCharacterMove();

		if (IsDie())
		{
			Cancel();
		}
	}
}

void UUI_GoHome::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetFirstPlayerController()->InputComponent->BindAction(TEXT("GoingHome"), IE_Pressed, this, &UUI_GoHome::GoingHome);
}

bool UUI_GoHome::IsDie()
{
	if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
	{
		if (FCharacterAttribute *InAttribute = InPlayerState->GetCharacterAttribute())
		{
			if (InAttribute->Health <= 0.f)
			{
				return true;
			}
		}
	}

	return false;
}

void UUI_GoHome::ReadyGoingHome()
{
	if (!IsDie())
	{
		bStartCountdown = true;
		CountDown = 0.f;

		if (auto StartGoHome = GetNameWidgetAnimation("StartGohome"))
		{
			PlayAnimation(StartGoHome);
		}

		//通知服务器开始
		StartGoHomeAnim();
	}
}

void UUI_GoHome::Cancel()
{
	bStartCountdown = false;
	CountDown = 8.f;

	if (auto EndGoHome = GetNameWidgetAnimation("EndGoHome"))
	{
		PlayAnimation(EndGoHome);
	}
}

void UUI_GoHome::CancelGoingHome()
{
	if (CountDown < 8.f)
	{
		//取消
		EndGoHomeAnim();
	}

	Cancel();
}

void UUI_GoHome::StartGoHomeAnim()
{
	if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
	{
		InPlayerState->StartGoHomeAnim();
	}
}

void UUI_GoHome::EndGoHomeAnim()
{
	if (AMobyGamePlayerState* InPlayerState = GetPlayerState())
	{
		InPlayerState->EndGoHomeAnim();
	}
}

void UUI_GoHome::GoingHome()
{
	if (bStartCountdown)
	{
		CancelGoingHome();
	}
	else
	{
		ReadyGoingHome();
	}
}

void UUI_GoHome::GoHome()
{
	if (AMobyGamePlayerState *InPlayerState = GetPlayerState())
	{
		InPlayerState->GoHome();
	}
}

void UUI_GoHome::CheckCharacterMove()
{
	if (AMobyPawn *InPawn = GetWorld()->GetFirstPlayerController()->GetPawn<AMobyPawn>())
	{
		if (AMobyGamePlayerCharacter *InCharacter =  InPawn->GetControllerCharacter())
		{
			if (InCharacter->GetVelocity().Size() > 0.f)
			{
				CancelGoingHome();
			}
		}
	}
}
