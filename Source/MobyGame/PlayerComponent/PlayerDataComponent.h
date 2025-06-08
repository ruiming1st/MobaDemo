// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../MobyGameType.h"
#include "../Table/SlotAttribute.h"
#include "../Data/MisData.h"
#include "../Data/Buff.h"
#include "PlayerDataComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOBYGAME_API UPlayerDataComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class AMobyGamePlayerState;
	friend struct FCharacterAttribute;
public:	
	// Sets default values for this component's properties
	UPlayerDataComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = PlayerName)
	FString GetPlayerName();

	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	FName PlayerName;

	//主机玩家的队伍信息同步
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Player data")
	ETeamType Team;

	//组件复制，参见https://docs.unrealengine.com/4.27/zh-CN/InteractiveExperiences/Networking/Actors/Components/ 或者在生成PlayerID时调用RPC同步到客户端
	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	int64 PlayerID;
	
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Player data")
	int32 Gold;

	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	int32 Kill;

	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	int32 Killed;

	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	int32 Assists;

	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	int32 KillDogFace;

	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	int32 SkillPoint;

	//InventoryID + FSlotData
	//Key是SlotID
	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	TMap<int32, FSlotData> InventorySlots;//TMap不支持RPC同步

	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	TMap<int32, FSlotData> SkillSlots;

private:
	//这里是实际管理实例，CharacterAttribute.h中是弱引用，监视资源有效性
	//SlotAttributes即为所有FSlotAttribute的集合
	TSharedPtr<FSlotAttribute_Interior> SlotAttributes;

	//队列 计算技能、物品、装备、属性
	TMap<int32, FSlotData*> SlotQueue;

	//队列 专门计算技能 物品 装备 属性
	TMap<int32, FSlotData*> SkillSlotQueue;

	//通过判断距离商店距离维护一个可以退货的Map
	TMap<int32, FSlotData*> SellSlotQueue;
};
