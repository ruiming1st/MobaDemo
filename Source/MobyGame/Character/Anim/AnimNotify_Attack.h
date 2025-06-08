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

	//����ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	int32 SlotID;

	//����ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	int32 SkillID;

	//���ֻ����Ŀ�� ��ô�Ϳ����� ���ϣ����Χ�ڵ��˶��ܵ��˺��ͱ���Ĭ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	bool bCheck;

	//Ⱥ�幥��or���幥��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bSingleTarget;

	//���ӵ��󶨵���ɫ�������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bBind;

	//��ײ���С
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector BoxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	EBulletType BulletType;

	//��ײ�����λ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector RelativeLocation;

	//�ӵ����ڵ���������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float LifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	FName InSocketName;

};
