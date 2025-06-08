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


	//等级
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

	//最大生命值
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MaxHealth;

	//生命值
	UPROPERTY()
	float Health;

	//基础攻击
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float PhysicalAttack;

	//护甲
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float Armor;

	//暴击
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float CriticalStrike;

	//魔法攻击
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MagicAttack;

	//魔法防御
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MagicDefense;

	//物理穿透
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float PhysicalPenetration;

	//魔法穿透
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MagicPenetration;

	//移动速度
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float WalkSpeed;

	//攻击速度
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float AttackSpeed;

	//最大经验值
	UPROPERTY(EditDefaultsOnly, Category = "Character Attribute")
	float MaxEmpircalValue;

	UPROPERTY()
	float EmpircalValue;

	//攻击范围
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