// Copyright Epic Games, Inc. All Rights Reserved.
//服务器玩家
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../MobyGameType.h"
#include "../../Table/CharacterAttribute.h"
#include "MobyGameCharacter.generated.h"

class UWidgetComponent;
class UArrowComponent;
class ABullet;
UCLASS(Blueprintable)
class AMobyGameCharacter : public ACharacter
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* Widget;

	//开火点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* OpenFirePoint;

public:
	AMobyGameCharacter();

	UFUNCTION()
	virtual void InitCharacter();

	void InitWidgetInfo();
	
	void AddCurrentBullet(ABullet* InInstance);
	void CallBulletEndOpenFire();

	FVector GoHome();
	//动画的播放
	void NormalAttack(TWeakObjectPtr<AMobyGameCharacter> InTarget);


	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	//广播动画
	UFUNCTION(NetMulticast, unreliable)
	void MultiCastPlayerAnimMontage(UAnimMontage* InMontage, float InPlayRate = 1.0f, FName StartSectionName = NAME_None);

	UFUNCTION(NetMulticast, unreliable)
	void MultiCastPlayerStopAnimMontage();

	//同步给客户端PlayerID，因为是个不变的量，所以同步一次即可，不需要变量复制
	UFUNCTION(NetMulticast, unreliable)
	void MultiCastRegisterCharacter(const int64& InPlayerID);

	UFUNCTION(NetMulticast, unreliable)
	void MultiCastHitParticle(const FVector& InHitLocation, UParticleSystem* InParInstance);

	//更新广播血条蓝条
	UFUNCTION(NetMulticast, unreliable)
	void MultiCastWidgetInfo(float InHPPercentage, float InManaPercentage);

	UFUNCTION(NetMulticast, unreliable)
	void MultiCastWidgetInfo_Health(float InHPPercentage);

	UFUNCTION(NetMulticast, unreliable)
	void MultiCastWidgetInfo_Mana(float InManaPercentage);

	UFUNCTION(NetMulticast, unreliable)
	void MultiCastReborn();

	FCharacterAttribute* GetCharacterAttribute();
	void AddBuff(const FSlotAttribute* SlotData, int64 InPlayerID);
	void RegisterCharacter(const int64& InPlayerID, int32 CharacterID, FName InName);

	UFUNCTION(NetMulticast, unreliable)
	void SetClientWidgetVisibility(bool bVisibility);

	UFUNCTION(NetMulticast, reliable)
	void ServerCallClientTakeDamage(bool bDie);

	void SetWidgetVisibility(bool bVisibility);


	virtual void Hurt(bool bDie);

	UFUNCTION()
	virtual void Die();
public:
	void ResetSpeed(float InSpeed);

public:
	//开火点
	FVector GetFirePointLocation();
	FRotator GetFirePointRotation();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	FORCEINLINE int64 GetPlayerID() { return PlayerID; }
	FORCEINLINE ETeamType GetTeam() { return TeamType; }
	FORCEINLINE ECharacterType GetCharacterType() { return CharacterType; }
	FORCEINLINE UArrowComponent* GetFirePoint() { return OpenFirePoint; }
	FORCEINLINE UWidgetComponent* GetWidget() { return Widget; }

	bool IsDie();

	class AMobyGamePlayerState *GetPlayerState();
	void SetTeam(ETeamType InTeamType);
	void SetCharacterType(ECharacterType InCharacterType);

	void LockRot(bool bInLockRot);
	void ResetCurrentRot();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
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

public:
	void SetNewHealth(float InHP);
	void SetNewMana(float InMP);
	void SetNewLv(float InLv);
	void SetTitle(FName InName);

	void SetVertigo(bool bSet);
	void SetSilent(bool bSet);
	void SetLimit(bool bSet);

	FVector GetHomePoint();
public:
	UPROPERTY(Replicated)
	FName PlayerName;
private:
	bool bAttacking;
	uint8 AttackCount;

	int64 PlayerID;
	bool bLockRot;
	FRotator CurrentRot;

private:
	UPROPERTY(Replicated)
	ETeamType TeamType;

	ECharacterType CharacterType;

	//防止子弹销毁变成野指针
	TArray<TWeakObjectPtr<ABullet>> BulletWeakPtrs;
};

