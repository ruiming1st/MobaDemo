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

	//������ҵĶ�����Ϣͬ��
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Player data")
	ETeamType Team;

	//������ƣ��μ�https://docs.unrealengine.com/4.27/zh-CN/InteractiveExperiences/Networking/Actors/Components/ ����������PlayerIDʱ����RPCͬ�����ͻ���
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
	//Key��SlotID
	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	TMap<int32, FSlotData> InventorySlots;//TMap��֧��RPCͬ��

	UPROPERTY(EditDefaultsOnly, Category = "Player data")
	TMap<int32, FSlotData> SkillSlots;

private:
	//������ʵ�ʹ���ʵ����CharacterAttribute.h���������ã�������Դ��Ч��
	//SlotAttributes��Ϊ����FSlotAttribute�ļ���
	TSharedPtr<FSlotAttribute_Interior> SlotAttributes;

	//���� ���㼼�ܡ���Ʒ��װ��������
	TMap<int32, FSlotData*> SlotQueue;

	//���� ר�ż��㼼�� ��Ʒ װ�� ����
	TMap<int32, FSlotData*> SkillSlotQueue;

	//ͨ���жϾ����̵����ά��һ�������˻���Map
	TMap<int32, FSlotData*> SellSlotQueue;
};
