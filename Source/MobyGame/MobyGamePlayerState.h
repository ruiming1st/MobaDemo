// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerComponent/PlayerDataComponent.h"
#include "Engine/DataTable.h"
#include "Data/System/AssistSystem.h"
#include "Table/CharacterAttribute.h"
#include "MobyGamePlayerState.generated.h"

struct FSlotTable;
struct FSlotAttribute;
struct FSlotDataNetPackage;
struct FKillNetPackage;
struct FSkillUpNetPackage;
/**
 * 
 */
//单播代理无法同时绑定技能和物品，因此需要多播代理
DECLARE_MULTICAST_DELEGATE_OneParam(FSimpleOneKeyDelegate, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FSimpleOneKeysDelegate, const TArray<int32>&)

DECLARE_DELEGATE_OneParam(FSimpleUpdateSkillLvDelegate, const FSkillUpNetPackage&)
DECLARE_DELEGATE_OneParam(FSimpleUpdateBuffInfosDelegate, const TArray<FBuffNetPackage>&)
DECLARE_DELEGATE_TwoParams(FSimpleTeamKillInfoDelegate, int32, int32)
DECLARE_DELEGATE_OneParam(FSimplePlayerTeamDelegate, const TArray<FPlayerTeamNetPackage>&)
DECLARE_DELEGATE_OneParam(FSimplePlayerKillInfoDelegate, const FPlayerKillInfoNetPackage&)
DECLARE_DELEGATE_OneParam(FHideOrShowTopPanelDelegate, bool)
DECLARE_DELEGATE_OneParam(FSimpleBindLookPlayerInfoDelegate, const FLookPlayerInfoNetPackage&)
DECLARE_DELEGATE_OneParam(FSimpleBindPlayerKillMsgDelegate, const FKillNetPackage&)
DECLARE_DELEGATE_OneParam(FSimpleDieDelegate, float)

DECLARE_DELEGATE_OneParam(FSimpleShowSkillLvDelegate, const TArray<int32>&)
DECLARE_DELEGATE_OneParam(FSimpleLockSkillDelegate, bool)

DECLARE_DELEGATE_OneParam(FGameOverDelegate, bool)

UCLASS()
class MOBYGAME_API AMobyGamePlayerState : public APlayerState
{
	GENERATED_BODY()

	friend class AMobyGameGameMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseAttrubute", meta = (AllowPrivateAccess = "true"))
	UPlayerDataComponent* PlayerData;
public:
	UFUNCTION(Client, reliable)
	void ServerCallClientInitPlayerData(const int64 &InPlayerID);
public:
	FSimpleOneKeysDelegate InitSlotsDelegate;
	FSimpleOneKeyDelegate UpdateSlotDelegate;
	FSimpleOneKeyDelegate StartUpdateCDSlotDelegate;
	FSimpleOneKeyDelegate EndUpdateCDSlotDelegate;

	FSimpleUpdateBuffInfosDelegate UpdateBuffInfosDelegate;
public:
	FSimpleBindLookPlayerInfoDelegate LookPlayerInfoDelegate;
	FHideOrShowTopPanelDelegate				HideOrShowTopPanelDelegate;

public:
	FGameOverDelegate GameOverDelegate;
public:
	FSimpleBindPlayerKillMsgDelegate KillMsgDelegate;
	FSimplePlayerTeamDelegate SimplePlayerTeamDelegate;
	FSimplePlayerKillInfoDelegate SimplePlayerKillInfoDelegate;
	FSimpleTeamKillInfoDelegate TeamKillInfoDelegate;

	//技能
public:
	FSimpleUpdateSkillLvDelegate UpdateSkillLvDelegate;
	FSimpleShowSkillLvDelegate ShowSkillUpDelegate;
	FSimpleLockSkillDelegate SimpleLockSkillDelegate;

	//角色死亡和重生
public:
	FSimpleDieDelegate SimpleDieDelegate;
	FSimpleDelegate SimpleBornDelegate;
public:
	AMobyGamePlayerState();

protected:
	virtual void BeginPlay() override;
	void TickData(float DeltaSeconds);

	//计算
	void Buff(float DeltaSeconds);
	void Skill(float DeltaSeconds);
	void Gold(float DeltaSeconds);
	void Sale(float DeltaSeconds);
	void Recovery(float DeltaSeconds);
	void Assist(float DeltaSeconds);
	//模板类型
public:
	const FSlotTable* GetSlotTableTemplate(int32 InID);
	const FSlotAttribute* GetSlotAttributeTemplate(int32 InID);
	const TArray<FSlotTable*>* GetSlotTablesTemplate();
	const TArray<FSlotAttribute*>* GetSlotAttributesTemplate();

	//对Slot实例的操作
public:
	FSlotAttribute* GetSlotAttribute(int32 InSlotID);
	bool IsCDValid(int32 InSlotID);
	FSlotData* GetSlotData(int32 InSlotID);
	FSlotData* GetSlotData(int32 InSlotID, int32 &InKey);

public:
	bool AddSlotAttributes(int32 InKey, int32 InSlotID);
	bool AddSlotAttributes(int32 InKey, const FSlotAttribute* InPlayerSlotAttribute);

	bool RecursionAddSlotAttributes(int32 InSlotID);

	//物品操作
public:
	bool AddSlotToInventory(int32 InSlotID);
	bool IsInventorySlotValid();
	bool IsInventorySlotValid(int32 InInventoryID);
	TMap<int32, FSlotData>* GetInventorySlotData();
	FSlotData* GetInventorySlotData(int32 InInventoryID);
	void GetInventoryAllKeys(TArray<int32>& InKeys);
	void CheckInventory(int32 InventoryID);
	void RecursionCreateInventorySlot();
	bool IsExistInInventory(int32 InItemID);

	//skill 技能操作
public:
	FSlotData* GetSkillSlotData(int32 InInventoryID);
	void RecursionCreateSkillSlot();
	void InitSlotSkill();
	void GetSkillAllKeys(TArray<int32>& InKeys);

	int32 GetSkillID(int32 InSlotID);
	FSlotAttribute* GetSlotAttributeBySkillID(int32 InSkillID);
	//RPC相关参数
public:
	void GetInventorySlotNetPackage(FSlotDataNetPackage& InNetPackage);
	void GetSkillSlotNetPackage(FSlotDataNetPackage& InNetPackage);

public:
	void UpdateTargetInfo(const int64 &InPlayerID);
	void SetLockSkill(bool bCharacterLockSkill);
	bool IsSkillTip(int32 InSlotID);
	void ShowAttackTip(int32 InSlotID, bool bShow);

	int32 GetItemAndCheckGold(const FSlotTable *InTable);
public:
	UFUNCTION(Client, reliable)
	void ServerCallClientUpdateHideOrShowUI(bool bHideOrNot);

	//RPC函数 Inventory
public:
	//做交换和移动物品
	UFUNCTION(Server, reliable)
	void ClientCallServerUpdateInventory(int32 InMoveInventoryID, int32 InMovedInventoryID);
	
	UFUNCTION(Server, reliable)
	void UpdateSkillLv(int32 InUIID);

	//使用
	UFUNCTION(Server, reliable)
	void Use(int32 InUIID);

	UFUNCTION(Server, reliable)
	void ReleaseSkill();

	//购买
	UFUNCTION(Server, reliable)
	void Buy(int32 InSlotID);

	//卖掉 打折
	UFUNCTION(Server, reliable)
	void Sell(int32 InSlotID, int32 InItemID);

	//取消订单
	UFUNCTION(Server, reliable)
	void CancellationOfOrder(int32 InSlotID, int32 InItemID);
public:
	//GameInfo
	UFUNCTION(Server, reliable)
	void GoHome();

	UFUNCTION(Server, reliable)
	void StartGoHomeAnim();

	UFUNCTION(Server, reliable)
	void EndGoHomeAnim(); 


	//服务器通知客户端
	//RTC
public:
	UFUNCTION(Client, reliable)
	void ServerCallClientLockSkill(bool bCharacterLockSkill);
protected:
	//服务器调用但在客户端执行，使用UFUNCTION(Client)
	UFUNCTION(Client, reliable)
	void ServerCallClientInitInventory(const FSlotDataNetPackage& InInventorySlots);

	UFUNCTION(Client, reliable)
	void ServerCallClientUpdateSlot(int32 InID, const FSlotData& InNetSlotData);

	UFUNCTION(Client, reliable)
	void ServerCallClientStartUpdateCD(int32 InID, const FSlotData& InNetSlotData);

	UFUNCTION(Client, reliable)
	void ServerCallClientEndUpdateCD(int32 InID, const FSlotData& InNetSlotData);

public	:
	UFUNCTION(Client, reliable)
	void ServerCallClientUpdateKillInfo(const FPlayerKillInfoNetPackage& InNetPackage);

	UFUNCTION(Client, reliable)
	void ServerCallClientUpdateTeamKillInfo(int32 InFriendly, int32 InEnemy);

	UFUNCTION(Client, reliable)
	void ServerCallClientUpdateLookPlayerInfo(const FLookPlayerInfoNetPackage& InNetPackage);

	//技能
protected:
	UFUNCTION(Client, reliable)
	void ServerCallClientInitSkill(const FSlotDataNetPackage& InSkillSlots);

	UFUNCTION(Client, reliable)
	void ServerCallClientUpLvSkill(const FSkillUpNetPackage& InUpNetPackage);

public:
	UFUNCTION(Client, reliable)
	void ShowSkillUp(const TArray<int32>& InUpNetPackage);

	UFUNCTION(Client, reliable)
	void ResponseCharacterAttribute(int32 InPlayerID, const ECharacterAttributeType Type, float InValue);

	UFUNCTION(Client, reliable)
	void ResponseCharacterAttributes(int64 InPlayerID, const FCharacterAttribute& InAttr);

	UFUNCTION(Client, reliable)
	void ServerCallClientUpdateKillMsg(const FKillNetPackage& KillNetPackage);

public:
	UFUNCTION(Client, reliable)
	void ServerCallClientUpdateShopPos(const FVector &InLocation);

public:
	UFUNCTION(Client, reliable)
	void ResponseAllCharacterTeamsAttribute(const TArray<FPlayerTeamNetPackage>& InPackage);

	UFUNCTION(Server, reliable)
	void RequestAllCharacterTeamsAttribute();

	//buff
public:
	UFUNCTION(Client, reliable)
	void ServerCallClientAddBuffs(const TArray<FBuffNetPackage>& InBuffNetPackage);

	//角色死亡和重生
public:
	UFUNCTION(Client, reliable)
	void ServerCallClientDie(float InDeathTime);

	UFUNCTION(Client, reliable)
	void ServerCallClientGameOver(bool bVictory);

	void Reborn();

public:
	void GetPlayerKillInfo(FPlayerKillInfoNetPackage& InNetPackage);
	void UpdateKillInfo();
	void GetBuffInfo(TArray<FBuffNetPackage> &InNetPakage);

	void ShowSkillLv();
	//奖励系统
public:
	void AddGold(int32 InNewGold);
	void AddSkillSlotPoint(int32 InSlotID);

	UFUNCTION(Client, reliable)
	void ServerCallClientAddGold(int32 InNewGold, const FVector& InLocation);
public:
	void AddBuff(const FSlotAttribute* InBuffAttribute, int32 InKeyID);

	//需要施法者
	void AddBuff(const FSlotAttribute* InBuffAttribute, int64 InCasterPlayerID);
	void AddSkill(FSlotAttribute* InSkillAttribute, int64 InCasterPlayerID);

	void CallClientAddBuff(const FSlotAttribute* InBuffAttribute);
	void UpdateSpawnPointLocation();
	bool IsScopeStore();//是不是在商店指定的范围内(客户端调用)

	//仅提供内部调用
private:
	void RecursionCreateSlot(TMap<int32, FSlotData>& InCreate, const TMap<int32, FSlotData>& InCompare);
	void GetAllSlotKeys(const TMap<int32, FSlotData>& InData, TArray<int32>& InIDs);
	void Transaction(int32 InSlotID, int32 InItemID, float Discount = 1.0f);
	void RecursivelyGetItemID(const FSlotTable *InTable, TArray<int32> &ItemIds);
	//助攻系统
public:
	const TArray<FAssistPlayer>& GetAssists();
	void AddAssist(const int64& InPlayerID);
	const FAssistPlayer* GetLastAssist();

	FCharacterAttribute* GetCharacterAttribute();
	//数据属性操作
public:
	//检测属性是不是持续类型 也就是buff
	bool IsSlotAttributesContinued(int32 InSlotID);
	bool IsSlotAttributesSkill(int32 InSlotID);

	bool UseMana(const FSlotAttribute* InSlotAttribute);
protected:
	UPROPERTY()
	UDataTable* SlotTablePtr;

	UPROPERTY()
	UDataTable* SlotAttributePtr;

public:
	FORCEINLINE UPlayerDataComponent* GetPlayerData() { return PlayerData; }

public:
	//Buff
	TMap<int32, FSlotAttribute> BuffSlotQueue;

private:
	TArray<FSlotTable*> CacheSlotTables;
	TArray<FSlotAttribute*> CacheSlotAttribute;
	FAssistSystem AssistSystem;
private:
	float GoldTime;
	float BuffTime;
	float RecoveryTime;
	bool BuffFlag;
	FVector ShopLocation;
	FVector BloodLocation;

	//锁住技能
	bool bLockSkill;
};
