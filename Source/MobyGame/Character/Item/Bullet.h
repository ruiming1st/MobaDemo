// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MobyGame/MobyGameType.h"
#include "../../Data/System/AttackTargetSystem.h"
#include "Bullet.generated.h"


class UParticleSystem;
class UProjectileMovementComponent;
class UBoxComponent;
struct FSlotAttribute;
UCLASS()
class MOBYGAME_API ABullet : public AActor
{
	GENERATED_BODY()

	//碰撞盒子
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttribute", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxDamage;

	//开火点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttribute", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* OpenFirePoint;

	//子弹
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttribute", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* Bullet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttribute", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttribute", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootBullet;
	
	UPROPERTY(EditDefaultsOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
	TArray<UParticleSystem*> HitParticles;

	UPROPERTY(EditDefaultsOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
	EBulletAttackType AttackType;

public:	
	// Sets default values for this actor's properties
	ABullet();
	UParticleSystem* GetHitParticles();

	void SetBoxSize(const FVector& InSize);
	void SetSingleTarget(bool bInSigleTarget);
	void SetCheck(bool bInCheck) { bCheck = bInCheck; }
	void SetRelativePosition(const FVector& InPosition);
	void SetOpenFireActive(bool bActive);

	void SetSlotAttribute(const FSlotAttribute* InSlotAttribute);

	FORCEINLINE EBulletType GetBulletType() { return Type; }
	void SetBulletType(EBulletType InType);

	void UpdateBullet();
public:
	UFUNCTION(NetMulticast, unreliable)
	void MultiCastEndOpenFireParticle();

	UFUNCTION(NetMulticast, reliable)
	void MultiCastRemoveTarget(AMobyGameCharacter* InCharacter);

	UFUNCTION(NetMulticast, reliable)
	void MultiCastAddTarget(AMobyGameCharacter* InCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	EBulletType Type;

	bool bCheck;
	bool bSingleTarget;
	const FSlotAttribute* SlotAttribute;

	FAttackTargetSystem AttackTargetSystem;
};
