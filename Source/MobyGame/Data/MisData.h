// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MisData.generated.h"

USTRUCT(BlueprintType)
struct FPlayerLocation
{
	GENERATED_USTRUCT_BODY()

	FPlayerLocation();

	//Ѱ�������Դ
	UPROPERTY()
	int32 CharacterID;

	//��λ��
	UPROPERTY()
	int64 PlayerID;

	//λ��
	UPROPERTY()
	FVector Location;
};

FORCEINLINE bool operator==(const FPlayerLocation& Lhs, const FPlayerLocation& Rhs)
{
	return Rhs.PlayerID == Lhs.PlayerID;
}

//��Ҫͬ�����ͻ���
USTRUCT(BlueprintType)
struct FSlotData
{
	GENERATED_USTRUCT_BODY()

	FSlotData();

	//�˴�ID�Ǳ������ID
	UPROPERTY()
	int32 SlotID;

	UPROPERTY()
	UTexture2D* SlotICO;

	UPROPERTY()
	float CD;

	//���Ϊ-1����־����Ʒ�����ص������缼�ܺ�װ��
	UPROPERTY()
	int32 Number;

	UPROPERTY()
	bool bCancelBuy;

	void Reset();
};

USTRUCT(BlueprintType)
struct FSlotDataNetPackage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<int32> SlotIDs;

	UPROPERTY()
	TArray<FSlotData> Slots;
};

//��ɫ����Э��
UENUM(BlueprintType)
enum class ECharacterAttributeType :uint8
{
	ATTR_NONE,
	ATTR_LV,
	ATTR_MANA,
	ATTR_MAX_MANA,
	ATTR_MAX_HEALTH,
	ATTR_HEALTH,
	ATTR_PHYSICAL_ATTACK,
	ATTR_ARMOR,
	ATTR_MAGIC_ATTACK,
	ATTR_MAGIC_DEFENSE,
	ATTR_PHYSICAL_PENETRATION,
	ATTR_MAGIC_PENETRATION,
	ATTR_WALK_SPEED,
	ATTR_ATTACK_SPEED,
	ATTR_MAX_EXP,
	ATTR_EXP,
};


UENUM(BlueprintType)
enum class EKillType : uint8
{
	//control+shift+u/l ��Сдת��
	KILL,
	DOUBLEKILL,
	TRIPLEKILL,
	QUADRAKILL,
	PENTAKILL,
	BLOOD,
	INVINCIBLE,
	LEGENDARY,
	ACE
};

USTRUCT(BlueprintType)
struct FKillNetPackage
{
	GENERATED_USTRUCT_BODY()

	FKillNetPackage();

	UPROPERTY()
	EKillType KillType;

	UPROPERTY()
	UTexture2D* KillerIcon;

	UPROPERTY()
	UTexture2D* KilledIcon;

	UPROPERTY()
	FString KillerName;

	UPROPERTY()
	FString KilledName;

};

USTRUCT(BlueprintType)
struct FPlayerKillInfoNetPackage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 Kill;

	UPROPERTY()
	int32 Killed;

	UPROPERTY()
	int32 Assists;

	UPROPERTY()
	int32 KillDogFace;
};

USTRUCT(BlueprintType)
struct FPlayerInfoNetPackage
{
	GENERATED_USTRUCT_BODY()
	FPlayerInfoNetPackage();
	UPROPERTY()
	int64 PlayerID;

	UPROPERTY()
	UTexture2D* PlayerIcon;

	UPROPERTY()
	int32 CharacterLevel;

	UPROPERTY()
	FPlayerKillInfoNetPackage  KillInfo;

	UPROPERTY()
	float DeathTime;
};

USTRUCT(BlueprintType)
struct FBuffNetPackage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 SlotID;

	UPROPERTY()
	float MaxCD;
};

USTRUCT(BlueprintType)
struct FSkillUpNetPackage
{
	GENERATED_USTRUCT_BODY()

	FSkillUpNetPackage();

	UPROPERTY()
	int32 Lv;

	UPROPERTY()
	int32 SlotID;

	UPROPERTY()
	bool bHidAllSlot;

	UPROPERTY()
	bool bEnableCurrentSlot;
};


USTRUCT(BlueprintType)
struct FPlayerTeamNetPackage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FPlayerInfoNetPackage PlayerInfo;

	UPROPERTY()
	FSlotDataNetPackage SlotDataNetPackage;
};

USTRUCT(BlueprintType)
struct FLookPlayerInfoNetPackage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int64 PlayerID;

	UPROPERTY()
	FSlotDataNetPackage SlotDataNetPackage;
};

