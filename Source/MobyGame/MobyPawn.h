// Fill out your copyright notice in the Description page of Project Settings.
//±¾µØÍæ¼Ò

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "MobyPawn.generated.h"

class AMobyGamePlayerCharacter;
class UPlayerDataComponent;
class AMobyGamePlayerState;
UCLASS()
class MOBYGAME_API AMobyPawn : public APawn
{
	GENERATED_BODY()

	friend class AMobyGameGameMode;
public:
	// Sets default values for this pawn's properties
	AMobyPawn();

	/** The default pawn class used by players. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<APawn> DefaultPawnClass;

public:

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* RootBox;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EEndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector GetCharacterLocation();
	void PressSpaceBar();
	void ReleaseSpaceBar();
public:
	UFUNCTION(Server,reliable)
	void CallServerClientViewportPostion(const FVector &InNewPos);

	UFUNCTION(server, reliable)
	void CharacterMoveToOnServer(const FVector &DirectionLocation);

	UFUNCTION(server, reliable)
	void CharacterMoveToTargetAndAttackOnServer(const FVector &DirectionLocation, const APawn *InPawn);

	UFUNCTION(Client, reliable)
	void ServerCallSetMobyGameCharacter(AMobyGamePlayerCharacter* InMobyGameCharacter);
public:
	UFUNCTION(Client, reliable)
	void ServerCallClientSpawnDrawText(const FVector& Location, float Value, float Percentage,const FLinearColor& Color);

	UFUNCTION(Client, reliable)
	void ServerCallClientSpawnDrawGoldText(const FVector& Location, int32 Value, float Percentage, const FLinearColor& Color);

	UFUNCTION(Client, reliable)
	void ServerCallClientSpawnDrawBuffValueText(const FVector& Location, float Value, float Percentage, const FLinearColor& Color);

	UFUNCTION(Client, reliable)
	void ServerCallClientSpawnDrawExpText(const FVector& Location, int32 Value, float Percentage, const FLinearColor& Color);

	UFUNCTION(Client, reliable)
	void ServerCallClientSetPawnLocation(const FVector& InLocation);

	void SkillAttack(int32 InSkillID);

public:
	FORCEINLINE AMobyGamePlayerCharacter* GetControllerCharacter() { return MobyGamePlayerCharacter; }

public:
	int64 GetPlayerID();
	const UPlayerDataComponent* GetPlayerData();

	AMobyGamePlayerState* GetPlayerState();
protected:
	//·ÀÖ¹GC
	UPROPERTY()
	AMobyGamePlayerCharacter* MobyGamePlayerCharacter;

	bool bPressSpaceBar;
	float PosTime;
};
