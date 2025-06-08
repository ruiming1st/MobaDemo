#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Core/MobyGameTableBase.h"
#include "CharacterAttribute.generated.h"

struct FSlotAttribute_Interior;
struct FSlotAttribute;

USTRUCT(BlueprintType)
struct FCharacterAttribute : public FMobyGameTableBase
{
	GENERATED_USTRUCT_BODY()

	FCharacterAttribute();


	//�ȼ�
	UPROPERTY()
	int32 Lv;

	//
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MaxMana;

	UPROPERTY()
	float Mana;

	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MaxAnger;

	UPROPERTY()
	float Anger;

	//�������ֵ
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MaxHealth;

	//����ֵ
	UPROPERTY()
	float Health;

	//��������
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float PhysicalAttack;

	//����
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float Armor;

	//����
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float CriticalStrike;

	//ħ������
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MagicAttack;

	//ħ������
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MagicDefense;

	//����͸
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float PhysicalPenetration;

	//ħ����͸
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MagicPenetration;

	//�ƶ��ٶ�
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float WalkSpeed;

	//�����ٶ�
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float AttackSpeed;

	//�����ֵ
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MaxEmpircalValue;

	UPROPERTY()
	float EmpircalValue;

	//������Χ
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float RangeAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Character Reward")
	float EmpircalReward;

	UPROPERTY(EditDefaultsOnly, Category = "Character Reward")
	int32 GoldReward;

	UPROPERTY(EditDefaultsOnly, Category = "Character Level")
	float Coefficient;

	UPROPERTY(EditDefaultsOnly, Category = "Character Level")
	int32 AddLevelID;

	const FSlotAttribute* AddLevelValue;

public:
	void Init();
	void Reset();

	float GetHealthPercentage() const;
	float GetManaPercentage() const;
	float GetEmpircalPercentage() const;
	float GetAngerPercentage() const;

	void SetBuff(TSharedRef<FSlotAttribute_Interior> InBuff);
	void UpdateLevel();
	bool IsLvUpperLimit() const;
public:
	float GetPhysicalAttack() const;
	float GetAttackSpeed() const;
	float GetWalkSpeed() const;
	float GetMagicPenetration() const;
	float GetPhysicalPenetration() const;
	float GetMagicDefense() const;
	float GetMaxMana() const;
	float GetMaxAnger() const;
	float GetMaxHealth() const;
	float GetCriticalStrike() const;
	float GetArmor() const;
	float GetMagicAttack() const;

	float GetEmpircalReward() const;
	int32 GetGoldReward() const;

	bool UseMana(float InMana);
	bool UseAnger(float InAnger);
private:
	TWeakPtr<FSlotAttribute_Interior> WeakPtr;
};