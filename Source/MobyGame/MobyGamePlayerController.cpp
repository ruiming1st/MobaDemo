// Copyright Epic Games, Inc. All Rights Reserved.

#include "MobyGamePlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"
#include "MobyPawn.h"
#include "Tool/ScreenMoveUnits.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/HUD.h"
#include "SimpleThread.h"
#include "MobyPawn.h"

AMobyGamePlayerController::AMobyGamePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AMobyGamePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	//Client
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		// keep updating the destination every tick while desired
		if (bMoveToMouseCursor)
		{
			MoveToMouseCursor();
		}
	}

	FScreenMoveUnits().ListenScreenMove(this, 10.f);
}

void AMobyGamePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AMobyGamePlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AMobyGamePlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("MouseRight", IE_Pressed, this, &AMobyGamePlayerController::OnRightMouseButtonPressed);
	InputComponent->BindAction("MouseRight", IE_Released, this, &AMobyGamePlayerController::OnRightMouseButtonReleased);



	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMobyGamePlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMobyGamePlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AMobyGamePlayerController::OnResetVR);
}

void AMobyGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//GThread不受UObject系统管理，如果不清除数据容易导致在反复快速重启编辑器情况下，上一局游戏的协程数据继承到下一局游戏，会导致崩溃
	GThread::Destroy();
}

void AMobyGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//锁定视口
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		//SetInputModeGameAndUI();
	}

}

void AMobyGamePlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMobyGamePlayerController::SetInputModeGameAndUI()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);

	SetInputMode(InputMode);
}

void AMobyGamePlayerController::MoveToMouseCursor()
{
	if(AMobyPawn* MyPawn = Cast<AMobyPawn>(GetPawn()))
	{
		FVector WorldOrigin;
		FVector WorldDirection;

		if (GetClientScreenWorldPosition(WorldOrigin, WorldDirection))
		{
			VerifyMouseWorldPositionClickOnServer(WorldOrigin, WorldDirection);
		}
	}
}

void AMobyGamePlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	if (AMobyPawn* MyPawn = Cast<AMobyPawn>(GetPawn()))
	{
		// Trace to see what is under the touch location
		FHitResult HitResult;
		GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
		if (HitResult.bBlockingHit)
		{
			// We hit something, move there
			MyPawn->CharacterMoveToOnServer(HitResult.ImpactPoint);
		}
	}
}

bool AMobyGamePlayerController::GetClientScreenWorldPosition(FVector &InWorldOrigin, FVector &InWorldDirection)
{
	// Trace to see what is under the mouse cursor	
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FVector2D MousePosition;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
		{
			if (GetHUD() != NULL && GetHUD()->GetHitBoxAtCoordinates(MousePosition, true))
			{
				return false;
			}

			if (UGameplayStatics::DeprojectScreenToWorld(this, MousePosition, InWorldOrigin, InWorldDirection) == true)
			{
				return true;
			}
		}
	}

	return false;
}

void AMobyGamePlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AMobyGamePlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}



void AMobyGamePlayerController::OnRightMouseButtonPressed()
{
	FVector WorldOrigin;
	FVector WorldDirection;

	if (GetClientScreenWorldPosition(WorldOrigin, WorldDirection))
	{
		GetInfoByClickTargetOnServer(WorldOrigin, WorldDirection);
	}
}

void AMobyGamePlayerController::OnRightMouseButtonReleased()
{

}

void AMobyGamePlayerController::ClientCallServerSetSkillRotTarget_Implementation(const FVector& TargetPoint)
{
	if (AMobyPawn* InPawn = GetPawn<AMobyPawn>())
	{
		FRotator R = FRotationMatrix::MakeFromX(TargetPoint - InPawn->GetControllerCharacter()->GetActorLocation()).Rotator();
		InPawn->GetControllerCharacter()->SetActorRotation(R);
	}
}

void AMobyGamePlayerController::GetInfoByClickTargetOnServer_Implementation(const FVector& WorldOrigin, const FVector& WorldDirection)
{
	if (AMobyPawn* MyPawn = Cast<AMobyPawn>(GetPawn()))
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), false);

		if (AMobyGamePlayerState* InPlayerState = GetPlayerState<AMobyGamePlayerState>())
		{
			if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ECC_GameTraceChannel1, CollisionQueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					if (AMobyGameCharacter* InTarget = Cast<AMobyGameCharacter>(HitResult.Actor))
					{
						if (MyPawn->GetControllerCharacter() != InTarget)
						{
							InPlayerState->UpdateTargetInfo(InTarget->GetPlayerID());
							InPlayerState->ServerCallClientUpdateHideOrShowUI(true);
						}
					}
				}
			}
			else if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, ECC_Visibility, CollisionQueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					InPlayerState->ServerCallClientUpdateHideOrShowUI(false);
				}
			}
		}
	}
}

void AMobyGamePlayerController::VerifyMouseWorldPositionClickOnServer_Implementation(const FVector &WorldOrigin, const FVector &WorldDirection)
{
	if (AMobyPawn* MyPawn = Cast<AMobyPawn>(GetPawn()))
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(ClickableTrace), false);
		auto TracePos = [&](ECollisionChannel InChannel)->bool
		{
			return GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, WorldOrigin + WorldDirection * HitResultTraceDistance, InChannel, CollisionQueryParams);
		};

		if (TracePos(ECC_GameTraceChannel1))
		{
			if (HitResult.bBlockingHit)//检测到就攻击 
			{
				if (MyPawn->GetControllerCharacter() != Cast<AMobyGamePlayerCharacter>(HitResult.Actor))
				{
					MyPawn->CharacterMoveToTargetAndAttackOnServer(HitResult.ImpactPoint, Cast<APawn>(HitResult.Actor));

					return;
				}

			}
		}
		if (TracePos(ECC_Visibility))
		{
			MyPawn->CharacterMoveToOnServer(HitResult.ImpactPoint);
		}
	}
}

//验证位置是否比地图大
bool AMobyGamePlayerController::VerifyMouseWorldPositionClickOnServer_Validate(const FVector &WorldOrigin, const FVector &WorldDirection)
{
	return true;
}


