// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "../Item/Bullet.h"
#include "../../MobyGameType.h"
#include "AnimNotify_Attack.generated.h"

/**
 * 
 */
UCLASS()
class MOBYGAME_API UAnimNotify_Attack : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_Attack();
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	TSubclassOf<ABullet> BulletClass;

	//技能ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	int32 SlotID;

	//技能ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	int32 SkillID;

	//如果只攻击目标 那么就开启它 如果希望范围内敌人都受到伤害就保持默认
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	bool bCheck;

	//群体攻击or单体攻击
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bSingleTarget;

	//将子弹绑定到角色开火点上
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bBind;

	//碰撞体大小
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector BoxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	EBulletType BulletType;

	//碰撞体相对位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector RelativeLocation;

	//子弹存在的生命周期
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float LifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	FName InSocketName;

};
