#pragma once

#include "CoreMinimal.h"
#include "Core/MobyGameTableBase.h"
#include "../MobyGameType.h"
#include "SlotAttribute.generated.h"

//增益效果
UENUM(BlueprintType)
enum class ESlotAttributeGainType :uint8
{
	SLOTATTRIBUTETYPE_ADD			UMETA(DisplayName = "Add"),
	SLOTATTRIBUTETYPE_SUBTRACT		UMETA(DisplayName = "Subtract"),
};

//属性类型
UENUM(BlueprintType)
enum class ESlotAttributeType :uint8
{
	SLOTATTRIBUTETYPE_SKILL			UMETA(DisplayName = "Skill"),		//技能
	SLOTATTRIBUTETYPE_CONTINUED		UMETA(DisplayName = "Continued"),	//持续
	SLOTATTRIBUTETYPE_PERMANENT		UMETA(DisplayName = "Permanent"),	//永久
};

//属性类型
UENUM(BlueprintType)
enum class ESlotAttributeValueType :uint8
{
	SLOTATTRIBUTEVALUE_VALUE			UMETA(DisplayName = "Value"),		//值
	SLOTATTRIBUTEVALUE_PERCENTAGE		UMETA(DisplayName = "Percentage"),	//百分比
};


USTRUCT(BlueprintType)
struct FSlotAttributeValue
{
	GENERATED_USTRUCT_BODY()

	FSlotAttributeValue();

	UPROPERTY(EditDefaultsOnly, Category = "Attribute Value")
	ESlotAttributeGainType GainType;

	//百分比或者非百分比
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Value")
	ESlotAttributeValueType ValueType;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute Value")
	float Value;
};


class ABullet;
USTRUCT(BlueprintType)
struct FSlotAttribute :public FMobyGameTableBase
{
	GENERATED_USTRUCT_BODY()

	FSlotAttribute();

	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	ESlotAttributeType Type;

	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	int32 Lv;

	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue Health;

	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue Mana;

	//消耗的蓝
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	float ConsumeMana;

	//消耗的怒气
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	float ConsumeAnger;

	//基础攻击
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue PhysicalAttack;

	//暴击
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue CriticalStrike;

	//怒气
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue Anger;

	//护甲
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue Armor;

	//魔法攻击
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue MagicAttack;

	//魔法防御
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue MagicDefense;

	//物理穿透
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue PhysicalPenetration;

	//魔法穿透
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue MagicPenetration;

	//移动速度
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue WalkSpeed;

	//攻击速度
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue AttackSpeed;

	//经验值
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue EmpircalValue;

	//奖励的经验值
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue EmpircalReward;

	//道具或者技能间隔
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue CD;

	//持续时间
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	float Time;

	//buff 被动技能
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	int32 BuffID;

	//buff 主动技能
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	int32 SkillID;

	UPROPERTY(EditDefaultsOnly, Category = "Slot Add Level")
	int32 AddLevelID;

	//表中配
	UPROPERTY(EditDefaultsOnly, Category = "Slot Add Level")
	TArray<int32> LimitCondition;

	UPROPERTY(EditDefaultsOnly, Category = "Skill Effect")
	ESkillEffectType EffectType;

	const FSlotAttribute* AddLevelValue;
	const FSlotAttribute* Skill;
	const FSlotAttribute* Buff;

	void UpdateLv();
};