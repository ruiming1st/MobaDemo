#pragma once

#include "CoreMinimal.h"
#include "Core/MobyGameTableBase.h"
#include "../MobyGameType.h"
#include "SlotAttribute.generated.h"

//����Ч��
UENUM(BlueprintType)
enum class ESlotAttributeGainType :uint8
{
	SLOTATTRIBUTETYPE_ADD			UMETA(DisplayName = "Add"),
	SLOTATTRIBUTETYPE_SUBTRACT		UMETA(DisplayName = "Subtract"),
};

//��������
UENUM(BlueprintType)
enum class ESlotAttributeType :uint8
{
	SLOTATTRIBUTETYPE_SKILL			UMETA(DisplayName = "Skill"),		//����
	SLOTATTRIBUTETYPE_CONTINUED		UMETA(DisplayName = "Continued"),	//����
	SLOTATTRIBUTETYPE_PERMANENT		UMETA(DisplayName = "Permanent"),	//����
};

//��������
UENUM(BlueprintType)
enum class ESlotAttributeValueType :uint8
{
	SLOTATTRIBUTEVALUE_VALUE			UMETA(DisplayName = "Value"),		//ֵ
	SLOTATTRIBUTEVALUE_PERCENTAGE		UMETA(DisplayName = "Percentage"),	//�ٷֱ�
};


USTRUCT(BlueprintType)
struct FSlotAttributeValue
{
	GENERATED_USTRUCT_BODY()

	FSlotAttributeValue();

	UPROPERTY(EditDefaultsOnly, Category = "Attribute Value")
	ESlotAttributeGainType GainType;

	//�ٷֱȻ��߷ǰٷֱ�
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

	//���ĵ���
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	float ConsumeMana;

	//���ĵ�ŭ��
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	float ConsumeAnger;

	//��������
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue PhysicalAttack;

	//����
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue CriticalStrike;

	//ŭ��
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue Anger;

	//����
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue Armor;

	//ħ������
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue MagicAttack;

	//ħ������
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue MagicDefense;

	//����͸
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue PhysicalPenetration;

	//ħ����͸
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue MagicPenetration;

	//�ƶ��ٶ�
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue WalkSpeed;

	//�����ٶ�
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue AttackSpeed;

	//����ֵ
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue EmpircalValue;

	//�����ľ���ֵ
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue EmpircalReward;

	//���߻��߼��ܼ��
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	FSlotAttributeValue CD;

	//����ʱ��
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	float Time;

	//buff ��������
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	int32 BuffID;

	//buff ��������
	UPROPERTY(EditDefaultsOnly, Category = "Slot Attribute")
	int32 SkillID;

	UPROPERTY(EditDefaultsOnly, Category = "Slot Add Level")
	int32 AddLevelID;

	//������
	UPROPERTY(EditDefaultsOnly, Category = "Slot Add Level")
	TArray<int32> LimitCondition;

	UPROPERTY(EditDefaultsOnly, Category = "Skill Effect")
	ESkillEffectType EffectType;

	const FSlotAttribute* AddLevelValue;
	const FSlotAttribute* Skill;
	const FSlotAttribute* Buff;

	void UpdateLv();
};