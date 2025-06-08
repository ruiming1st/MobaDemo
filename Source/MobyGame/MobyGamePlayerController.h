// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MobyGamePlayerController.generated.h"

UCLASS()
class AMobyGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMobyGamePlayerController();

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void BeginPlay() override;
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	void SetInputModeGameAndUI();

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);

	bool GetClientScreenWorldPosition(FVector &InWorldOrigin, FVector &InWorldDirection);
protected:
	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	void OnRightMouseButtonPressed();
	void OnRightMouseButtonReleased();
public:
	UFUNCTION(server, reliable)
	void ClientCallServerSetSkillRotTarget(const FVector& TargetPoint);

protected:
	UFUNCTION(server, reliable, WithValidation)
	void VerifyMouseWorldPositionClickOnServer(const FVector &WorldOrigin, const FVector &WorldDirection);

	UFUNCTION(server, reliable)
	void GetInfoByClickTargetOnServer(const FVector& WorldOrigin, const FVector& WorldDirection);
};


