// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Table/CharacterTable.h"
#include "Data/MisData.h"
#include "Table/CharacterAttribute.h"
#include "Data/System/KillSystem.h"
#include "MobyGame/Data/MisData.h"
#include "Data/System/KillSystem.h"
#include "Data/System/MinionsSystem.h"
#include "Data/System/MonsterSystem.h"
#include "Data/System/PlayerSystem.h"
#include "Data/System/TurretsSystem.h"
#include "Character/CharacterInstance/MobyGameCharacter.h"
#include "MobyGameState.generated.h"


enum class ECharacterAttributeType : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FSimpleUpdateAttributesDelegate, int64)
DECLARE_MULTICAST_DELEGATE_TwoParams(FSimpleUpdateAttributeDelegate, int64, const ECharacterAttributeType)

DECLARE_MULTICAST_DELEGATE_TwoParams(FSimpleEndBuffInfoDelegate, int64, int32)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FSimpleUpdateBuffInfoDelegate, int64, int32, float)

/**
 * 
 */
UCLASS()
class MOBYGAME_API AMobyGameState : public AGameStateBase
{
	GENERATED_BODY()

	friend class AMobyGameGameMode;
public:
	FSimpleUpdateAttributesDelegate SimpleUpdateAttributesDelegate;
	FSimpleUpdateAttributeDelegate SimpleUpdateAttributeDelegate;

	//buff
public:
	FSimpleUpdateBuffInfoDelegate	UpdateBuffInfoDelegate;//�ಥ
	FSimpleEndBuffInfoDelegate      EndBuffInfoDelegate;
public:
	AMobyGameState();

public:
	virtual void BeginPlay() override;

public:
	void Death(const int64& InPlayerID);
	float GetRebornTime(const AMobyGameCharacter* InCharacter);

	void GameOver(const ETeamType VictoryTeam);
	//��Ϸ�Ƿ����
	FORCEINLINE bool IsGameOver() const { return bGameOver; }
	FORCEINLINE bool IsSpawnSuper(const ETeamType TeamType) const
	{
		if (TeamType == ETeamType::TEAM_BLUE)
		{
			return bBlueSpawnSuper;
		}
		else if (TeamType == ETeamType::TEAM_RED)
		{
			return bRedSpawnSuper;
		}

		return false;
	}

	void SetSpawnSuper(const ETeamType TeamType, bool bNewState);

public:
	//����ָ����⿽��
	const TArray<FCharacterTable*>* GetCharacterTablesTemplate();
	const TArray<FCharacterAttribute*>* GetCharacterAttributesTemplate();

	//��ɫID��Ӧ������
	const FCharacterTable* GetCharacterTableTemplate(const int32 &InID);
	const FCharacterAttribute* GetCharacterAttributeTemplate(const int32 &InID);

	//���ID�Ͷ�Ӧ�Ľ�ɫ����
	const TMap<int64, FCharacterAttribute>* GetCharacterAttributes();
	FCharacterAttribute* GetCharacterAttribute(int64 InPlayerID);

	void UpdateCharacterAILocation(int64 InPlayerID, const FVector& InLocation);
	void AddCharacterAILocation(int64 InPlayerID, int32 InCharacterID, const FVector& InLocation);
	void RemoveCharacterAILocation(int64 InPlayerID);
	
	void AddCharacterData(int64 PlayerID, int32 CharacterID);
	void RemoveCharacterData(int64 PlayerID);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetCharacterAILocation(int64 InID, FVector& InLocation);

	int32 GetCharacterID(int64 InPlayerID);
	const TArray<FPlayerLocation>& GetPlayerLocations();

	int32 GetCharacterSkillAttributeID(const UAnimMontage* InAnimMontage);

	const FCharacterTable* GetCharacterTableBySkillID(const int32 InSkillID);
	const FSkill* GetSkillBySkillID(const int32 InSkillID);
	//RPC
public:
	void RequestCharacterAttribute(int32 InPlayerID, int32 InUpdatePlayerID, const ECharacterAttributeType Type);

	void RemoveTurret(TWeakObjectPtr<AMobyGameArchitectureCharacter> InTurretsWeakPtr, const ETeamType InType);

	void ResponseCharacterAttribute(int32 InPlayerID, const ECharacterAttributeType Type, float InValue);
	void ResponseCharacterAttributes(int64 InPlayerID, const FCharacterAttribute& InAttr);

	void SettlementDeath(const int64& InKillPlayerID, const int64& InKilledPlayerID);
	bool IsPlayer(const int64& InPlayerID);

	//buff
public:
	UFUNCTION(NetMulticast, unreliable)
	void ServerCallClientAddBuff(int64 InPlayerID, int32 InSlotID, float MaxCD);

	UFUNCTION(NetMulticast, unreliable)
	void ServerCallClientEndBuff(int64 InPlayerID, int32 InSlotID);

	UFUNCTION(NetMulticast, unreliable)
	void NetMulticastCharacterAttribute(APawn* InPawn, float InValue, const ECharacterAttributeType Type);
protected:
	//gamestate���������Զ����Ƶ����пͻ��ˣ����������Ҫ�����Ҫͬ���ı���
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

protected:
	void IterationTeamKillNumber(ETeamType Type);
protected:
	void UpdateKillMsg(const FKillNetPackage& InKillNetPackage);

	void Kill(const int64& InKillerPlayer, const int64& InKilledPlayer, EKillType InType);
	void SerialKill(const int64& InKillerPlayer, const int64& InKilledPlayer, EKillType InType);
	void AchievementKill(const int64& InKillerPlayer, EKillType InType);
protected:
	//��ֹGC����
	UPROPERTY()
	UDataTable* CharacterTablePtr;

	UPROPERTY()
	UDataTable* CharacterAttributePtr;

private:
	TArray<FCharacterTable*> CacheCharacterTables;
	TArray<FCharacterAttribute*> CacheCharacterAttribute;

	//��Ϸ״̬��Ϣ��ͬ���������λ�ã�ͬʱ�ṩ��MiniMap����С��ͼ
	UPROPERTY(Replicated)
	TArray<FPlayerLocation> PlayerLocation;

	//���ؽ�ɫ���Ի���أ�����ڷ������Ͼ��ǽ�ɫ����
	UPROPERTY()
	TMap<int64, FCharacterAttribute> CharacterAttributes;

	FKillSystem KillSystem;
	FMinionsSystem MinionsSystem;
	FMonsterSystem MonsterSystem;
	FPlayerSystem PlayerSystem;
	FTurretsSystem TurretsSystem;

	UPROPERTY()
	TMap<ETeamType, int32> TeamKillInfo;

	bool bGameOver;
	bool bRedSpawnSuper;
	bool bBlueSpawnSuper;
};
